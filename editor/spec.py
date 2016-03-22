#!/usr/bin/env python

# 8 bar Audio equaliser using MCP2307

import wave
from time import sleep, time
from struct import unpack
import numpy as np

# Set up audio
chunk = 2583 # Use a multiple of 8
chunk -= chunk % 7
data_in = wave.open('../music/disconnected_02.wav', 'rb')
sample_rate = data_in.getframerate()
no_channels = data_in.getnchannels()
nframe = data_in.getnframes()

def calculate_levels(data, chunk, sample_rate):
    # Convert raw data to numpy array
    data = unpack("%dh"%(len(data)/2),data)
    data = np.array(data, dtype='h')
    # Apply FFT - real data so rfft used
    fourier=np.fft.rfft(data)
    # Remove last element in array to make it the same size as chunk
    fourier=np.delete(fourier,len(fourier)-1)
    # Find amplitude
    power = np.log10(np.abs(fourier))*2
    power = (power - 12) * 3
    power = np.maximum(power, 0)
    power = np.minimum(power, 7)
    power = np.int_(power)
    # Araange array into 8 rows for the 8 bars on LED matrix
    matrix = np.zeros(7)

    def frange(l, r):
        lf = int(l * chunk / sample_rate)
        rf = int(r * chunk / sample_rate)
        return np.max(power[lf:rf])
    matrix[0] = frange(50, 100)
    matrix[1] = frange(100, 200)
    matrix[2] = frange(200, 400)
    matrix[3] = frange(400, 600)
    matrix[4] = frange(600, 900)
    matrix[5] = frange(900, 1400)
    matrix[6] = frange(1400, 2000)

    return matrix

def bbf(i):
    bar = 41 + i//32
    beat = (i%32)//8 + 1
    frac = i%8
    return '{}-{}-{}/8'.format(bar, beat, frac)

print("Processing.....")

lst = np.array([0]*7)
dtime = 40 * 4 * 60 / 128
stime = time() - dtime
data_in.setpos(int(dtime * sample_rate))

dord = '5263417'
parts = ['x', 'LM', 'JK', 'I', 'H', 'DAE', 'CF', 'BG']

stk = []
for i in range(7):
    stk.append([(-1, 0)])
out = []

NUM = 16*8 - 4

for i in range(NUM):
    # Read data from device
    data = data_in.readframes(chunk)
    # data_in.pause(1) # Pause capture whilst RPi processes data
    if len(data) == chunk * 4:
        # catch frame error
        try:
            print(bbf(i), bbf(i+1))
            matrix=calculate_levels(data, chunk,sample_rate)
            if i >= NUM-8:
                matrix *= 0
            lst = np.maximum(lst - 1, matrix)
            fr = data_in.tell()
            print(lst, fr/sample_rate, '/', nframe/sample_rate)
            for x in lst:
                print('#'*x)
            print('============')

            for x in range(7):
                val = int(lst[x])
                while stk[x][-1][1] > val:
                    start, ov = stk[x][-1]
                    end = i
                    out.append('{}\t{}\t{}{}'.format(bbf(start), bbf(end), dord[x], parts[ov]))
                    stk[x].pop()
                while stk[x][-1][1] < val:
                    stk[x].append((i, stk[x][-1][1]+1))

        except Exception as e:
            print(e)
        dt = time() - stime - data_in.tell() / sample_rate
        # if dt < 0:
            # sleep(-dt)
    else:
        break

for x in range(7):
    while stk[x][-1][1] > 0:
        start, ov = stk[x][-1]
        end = NUM
        out.append('{}\t{}\t{}{}'.format(bbf(start), bbf(end), dord[x], parts[ov]))
        stk[x].pop()

print(len(out))

out.sort()

f = open('spec.out', 'w')
for x in out:
    f.write(x + '\n')
f.close()
