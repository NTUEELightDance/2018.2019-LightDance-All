BPM_1 = 120.000
BPM_2 = 150.000
BPM_3 = 128.000
BPM_4 = 180.000

SEC_BEAT_1 = 60. / BPM_1
SEC_BEAT_2 = 60. / BPM_2
SEC_BEAT_3 = 60. / BPM_3
SEC_BEAT_4 = 60. / BPM_4

N_DANCER = 8
N_PART = 16
'''
2019_eenight_bpm (v9)
00:00.00 - 01:22.00	BPM = 120 (41*4拍)
01:22.00 - 01:58.80	BPM = 150 (23*4拍)
01:58.80 - 02:40.05	BPM = 128 (22*4拍)
02:40.05 - END		BPM = 180 (33*4拍)
'''

'''
2019_eenight_bpm (v8)
00:00.00 - 00:13.89 BPM = 64   (bar 14.816) 15
00:13.89 - 01:19.76	BPM = 120  (bar 131.74) 147
01:19.76 - 01:24.96 BPM = 94   (bar 8.146)  155
01:24.96 - 01:55.33	BPM = 150  (bar 75.925) 231
01:55.33 - 02:36.61	BPM = 128  (bar 88.064) 319
02:36.61 - end		BPM = 180  (bar )
'''

'''

A 0 
B 1 
C 2
D 3
E 4
F 5
G 6
H 7
I 8
J 9
L 10
M 11
N 12
O 13
P 14
Q 15


'''

'''
2019_eenight_bpm
00:00.00 - 01:22.00	BPM = 120 (41*4拍) 164
01:22.00 - 01:58.80	BPM = 150 (23*4拍) 256
01:58.80 - 02:40.05	BPM = 128 (22*4拍) 344
02:40.05 - END		BPM = 180 (33*4拍) 476
'''

def bbf2sec(bbf):
    tokens = bbf.split('-')
    bar = int(tokens[0]) - 1
    beat = int(tokens[1]) - 1
    frac = 0
    sec = 0
    if len(tokens) >= 3:
        a, b = tokens[2].split('/')
        frac = float(a) / float(b)
    if bar < 41 :
        sec = ( bar * 4 + beat + frac ) * SEC_BEAT_1
    elif bar < 64 :
        sec = 82.00 + ((bar-41)*4+beat+frac) * SEC_BEAT_2
    elif bar < 86:
        sec = 118.80 + ((bar-64)*4+beat+frac) * SEC_BEAT_3
    else :
        sec = 160.05 + ((bar-86)*4+beat+frac) * SEC_BEAT_4
    
    return sec

chr2Num = {
    'A' : 0, 
    'B' : 1, 
    'C' : 2,
    'D' : 3,
    'E' : 4,
    'F' : 5,
    'G' : 6,
    'H':  7,
    'I' : 8,
    'J' : 9,
    'K' : 10,
    'L' : 11,
    'M' : 12,
    'N' : 13,
    'O' : 14,
    'P' : 15
}
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

def translate(fname):
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
        start = bbf2sec(tokens[0])
        end = bbf2sec(tokens[1])
        parts = parse_parts(tokens[2])
        #print(start, end, parts)
        ltype = 1 # 1=ON, 2=Fade in, 3=Fade out
        if len(tokens) >= 4:
            if tokens[3] == 'FI':
                ltype = 2
            elif tokens[3] == 'FO':
                ltype = 3
        for i, j in parts:
            res[i][j].append((start, end, ltype))

    return res

def translate_pos(fname):
    lst = [x.strip() for x in open(fname, encoding='utf-8')]
    res = []
    for i in range(N_DANCER):
        res.append([])

    tm = 0
    sm = False
    for line in lst:
        if line.strip() == '' or line[0] == '#':
            continue
        # print (line)
        tokens = line.split()
        if len(tokens) <= 2:
            tm = bbf2sec(tokens[0])
            sm = (len(tokens) >= 2)
        else:
            num = int(tokens[0])
            bx = int(tokens[1])
            by = int(tokens[2])
            if not sm:
                res[num].append((tm, res[num][-1][1], res[num][-1][2]))
            res[num].append((tm, bx, by))

    return res

if __name__ == '__main__':
    import json
    import time

    while True:
        res = translate('tron.in')
        s = json.dumps(res)
        f = open('light.js', 'w')
        f.write("var Data = \"")
        f.write(s)
        f.write("\";")
        #print('done')
        f.close()

        res = translate_pos('tron.pos')
        s = json.dumps(res)
        f = open('pos.js', 'w')
        f.write("var Pos = \"")
        f.write(s)
        f.write("\";")
        f.close()
        time.sleep(0.4)
