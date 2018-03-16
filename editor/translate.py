BPM = 120.000
BPM_2 = 75.000
BPM_3 = 128.000
BPM_4 = 130.000
SEC_BEAT = 60. / BPM
N_DANCER = 10
N_PART = 26

'''

A 0, 10
B 1, 11, 8
C 2, 12
D 3, 13
E 4, 14
F 5, 15
G 6
H 7
I 8
J 9

M 0
N 1
P 5
Q 4
R 3
S 2

m 10
n 11
p 15
q 14
r 13
s 12

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
    '''if bar >= 55:
        sec = 43 * 4 * SEC_BEAT + 12 * 4 * SEC_BEAT_2 + ((bar - 55) * 4 + beat + frac) * SEC_BEAT_3
    elif bar >= 43:
        sec = 43 * 4 * SEC_BEAT + ((bar - 43) * 4 + beat + frac) * SEC_BEAT_2
    else:
    '''
    if bar >= 45:
        sec = 45 * 4 * SEC_BEAT + ((bar - 45) * 4 + beat + frac) * SEC_BEAT_4
    else:   
        sec = (bar * 4 + beat + frac) * SEC_BEAT
    return sec

idx_chr_map = [
    ['A','M'],
    ['B','N'],
    ['C','S'],
    ['D','R'],
    ['E','Q'],
    ['F','P'],
    ['G'],
    ['H'],
    ['B','I'],
    ['J'],
    ['A','m'],
    ['B','n'],
    ['C','s'],
    ['D','r'],
    ['E','q'],
    ['F','p'],
    [],
    [],
    [],
    [],
    [],
    [],
    [],
    [],
    [],
    []
]

def parse_single_part(s):
    res = []
    for i in range(N_DANCER):
        for j in range(N_PART):
            if chr(ord('1')+i) in s and ('Z' in s or ('X' in s and j != 6) or any(c in s for c in idx_chr_map[j])):
                res.append((i, j))
    return res

def parse_parts(s):
    res = []
    parts = s.split('+')
    for p in parts:
        res += parse_single_part(p)
    return list(set(res))

def translate(fname):
    lst = [x.strip() for x in open(fname)]
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
    lst = [x.strip() for x in open(fname)]
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
            num = int(tokens[0]) - 1
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
        time.sleep(2)
