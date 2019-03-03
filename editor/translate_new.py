BPM = 120.000
BPM_2 = 150.000
BPM_3 = 128.000
BPM_4 = 180.000
SEC_BEAT = 60. / BPM
N_DANCER = 10 #hasn't change
N_PART = 16
'''
2019_eenight_bpm
00:13.89 - 01:19.76	BPM = 120
01:24.96 - 01:55.33	BPM = 150
01:55.33 - 02:36.61	BPM = 128
02:36.61 - end		BPM = 180

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

def bbf2sec(bbf):
    tokens = bbf.split('-')
    bar = int(tokens[0]) - 1
    beat = int(tokens[1]) - 1
    frac = 0
    sec = 0
    SEC_BEAT_4 = 60. / BPM_4
    SEC_BEAT_3 = 60. / BPM_3
    SEC_BEAT_2 = 60. / BPM_2
    SEC_BEAT = 60. / BPM
    if len(tokens) >= 3:
        a, b = tokens[2].split('/')
        frac = float(a) / float(b)
    if bar >= 59:
        sec = 59 * 4 * SEC_BEAT + ((bar - 59) * 4 + beat + frac) * SEC_BEAT_4
    else:   
        sec = (bar * 4 + beat + frac) * SEC_BEAT
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
    'L' : 10,
    'M' : 11,
    'N' : 12,
    'O' : 13,
    'P' : 14,
    'Q' : 15
}
def parse_single_part(s):
	res = []
	dancers = []
	for x in range(len(s)):
		if ord(s[x]) <= 57:
			dancers.append(ord(s[x])-48)
		else:
			for y in dancers:
				res.append( (y,chr2Num[s[x]]) )
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
        res = translate('test.in')
        s = json.dumps(res)
        f = open('light.js', 'w')
        f.write("var Data = \"")
        f.write(s)
        f.write("\";")
        #print('done')
        f.close()
        
        res = translate_pos('test.pos')
        s = json.dumps(res)
        f = open('pos.js', 'w')
        f.write("var Pos = \"")
        f.write(s)
        f.write("\";")
        f.close()
        time.sleep(0.4)
