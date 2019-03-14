import numpy as np

N_DANCER = 8
N_PART = 5
SHAPE = [88, 48, 60, 36, 36]
MAX_FRAME_N = 2100

BPM_1 = 120.000
BPM_2 = 150.000
BPM_3 = 128.000
BPM_4 = 180.000

SEC_BEAT_1 = 60. / BPM_1
SEC_BEAT_2 = 60. / BPM_2
SEC_BEAT_3 = 60. / BPM_3
SEC_BEAT_4 = 60. / BPM_4

chr2Num = {
    'A' : 0, 
    'B' : 1, 
    'C' : 2,
    'D' : 3,
	'E' : 4
}

def bbf2sec(bbf):
    tokens = bbf.split('-')
    bar = int(tokens[0]) - 1
    beat = int(tokens[1]) - 1
    frac = 0
    sec = 0
    if len(tokens) >= 3:
        a, b = tokens[2].split('/')
        frac = float(a) / float(b)
    if bar <= 41 :
        sec = ( bar * 4 + beat + frac ) * SEC_BEAT_1
    elif bar <= 64 :
        sec = 82.00 + ((bar-41)*4+beat+frac) * SEC_BEAT_2
    elif bar <= 86:
        sec = 118.80 + ((bar-64)*4+beat+frac) * SEC_BEAT_3
    else :
        sec = 160.05 + ((bar-86)*4+beat+frac) * SEC_BEAT_4
    
    return sec

def translate_ws(fname):
    lst = [x.strip() for x in open(fname, encoding='utf-8')]
    res = []
    for i in range(N_DANCER):
        v = []
        for j in range(N_PART):
            v.append([])
        res.append(v)

    for line in lst:
        if line.strip() == '' or line[0] == '#':
            continue
        tokens = line.split()
        start = (int)(bbf2sec(tokens[0])*10)
        parts = parse_parts(tokens[1])
        anim = (tokens[2])

        for i, j in parts:
            res[i][j].append((start, anim))

    return res

def parse_single_part(s):
	res = []
	dancers = []
	for x in range(len(s)):
		if ord(s[x]) <= ord('9'):
			dancers.append(ord(s[x])-ord('0'))
		else:
			for y in dancers:
				res.append( (y,chr2Num[s[x]]) )
	# print (res)
	return res

def parse_parts(s):
    res = []
    parts = s.split('+')
    for p in parts:
        res += parse_single_part(p)
    return list(set(res))

if __name__ == '__main__':
	import json
	import time
	import os

	res = translate_ws('test.ws')
	# res.shape = (N_DANCER, N_PART, n_animation)
	frames = []
	for i in range(len(res)):
		frames.append([])
		for j in range(len(res[0])):
			frames[i].append(np.zeros((MAX_FRAME_N,SHAPE[j],3),dtype=np.int))
			for tup in res[i][j]:
				anim = np.load( os.path.join('test_npy', tup[1]+'.npy') )
				start = tup[0]
				end = start + anim.shape[0]
				frames[i][j][start:end] = anim
			frames[i][j] = frames[i][j].tolist()
	with open('data_ws.json', 'w') as outfile:
		json.dump(frames, outfile)