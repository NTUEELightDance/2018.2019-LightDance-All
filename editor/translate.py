BPM = 128.000
SEC_BEAT = 60. / BPM
N_DANCER = 7
N_PART = 10

def bbf2sec(bbf):
    tokens = bbf.split('-')
    bar = int(tokens[0]) - 1
    beat = int(tokens[1]) - 1
    frac = 0
    if len(tokens) >= 3:
        a, b = tokens[2].split('/')
        frac = float(a) / float(b)
    sec = (bar * 4 + beat + frac) * SEC_BEAT
    return sec

def parse_single_part(s):
    res = []
    for i in range(N_DANCER):
        for j in range(N_PART):
            if chr(ord('1')+i) in s and chr(ord('A')+j) in s:
                res.append((i, j))
    return res

def parse_parts(s):
    res = []
    parts = s.split('+')
    for p in parts:
        res += parse_single_part(p)
    return list(set(res))

def translate(lst):
    res = []
    for i in range(N_DANCER):
        v = []
        for j in range(N_PART):
            v.append([])
        res.append(v)

    for line in lst:
        if(line[0] == '#'):
            continue
        tokens = line.split()
        start = bbf2sec(tokens[0])
        end = bbf2sec(tokens[1])
        parts = parse_parts(tokens[2])
        #print(start, end, parts)
        for i, j in parts:
            res[i][j].append((start, end))

    return res

if __name__ == '__main__':
    lst = [x.strip() for x in open('test.in')]
    res = translate(lst)

    print(res)
