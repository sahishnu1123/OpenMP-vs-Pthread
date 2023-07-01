import sys

base = 3
side = base*base
def pattern(r,c): return (base*(r%base) + r//base+c)%side

from random import sample
import string

def shuffle(s): return sample(s,len(s))

rBase = range(base)

rows = [g*base + r for g in shuffle(rBase) for r in shuffle(rBase)]
cols = [g*base + c for g in shuffle(rBase) for c in shuffle(rBase)]

nums = shuffle(range(1, base*base + 1))

board = [[nums[pattern(r,c)] for c in cols] for r in rows]

file1 = open("input.txt", "w")

file1.write(str(16))
file1.write(' ')
file1.write(str(side))
file1.write('\n')

for line in board:
    for i in range(side):
        file1.write(str(line[i]))
        file1.write(' ')
    file1.write('\n')
