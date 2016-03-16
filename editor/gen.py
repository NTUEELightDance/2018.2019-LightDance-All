f = open('test2.in', 'w')

for i in range(1, 120):
    for j in range(4):
        c = 'A' if j%2==0 else 'B'
        d = 'B' if j%2==0 else 'A'
        f.write('{}-{} {}-{} 1{}+2{}\n'.format(i, j+1, i, j+2, c, d))
f.close()
