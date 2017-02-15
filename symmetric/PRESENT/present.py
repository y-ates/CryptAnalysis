#!/usr/bin/env python
#
# Copyright (C) 2017 Yakup Ates <Yakup.Ates@rub.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import sys
import random
from prettytable import PrettyTable

def rotate(word):
    return word[19:] + word[:19]


def get_SBox(i):
    SBox = [0xc, 0x5, 0x6, 0xb, 0x9, 0x0, 0xa, 0xd,
            0x3, 0xe, 0xf, 0x8, 0x4, 0x7, 0x1, 0x2]

    return SBox[i]


def get_pBox(i):
    p = [0, 16, 32, 48, 1, 17, 33, 49, 2, 18, 34, 50, 3, 19, 35, 51,
         4, 20, 36, 52, 5, 21, 37, 53, 6, 22, 38, 54, 7, 23, 39, 55,
         8, 24, 40, 56, 9, 25, 41, 57, 10, 26, 42, 58, 11, 27, 43, 59,
         12, 28, 44, 60, 13, 29, 45, 61, 14, 30, 46, 62, 15, 31, 47, 63]

    return p[i] 


def sBoxLayer(state):
    res = [0] * 64
    
    for i in range(64):
        res[i] = get_SBox(state[i])

    return res


def pLayer(state):
    res = [0] * 64
    for i in range(64):
        res[i] = state[get_pBox(i)]

    return res


def addRoundKey(state, key):
    res = [0] * 64

    for i in range(64):
        #res[i] = state[i] ^ key[i]
        #print len(key), len(state)
        res[i] = int(state[i]) ^ int(key[i])

    return res


def key_schedule(key, nr):
    if nr > 32:
        print "There are not that many rounds."
        sys.exit(0)
        
    key = rotate(key)
    tmp1 = 0
    out = 0
    tmp = ''
    
    tmp = ''.join(map(str, key[76:]))
    tmp1 = get_SBox(int(tmp, 2))
    tmp1 = [int(x) for x in list('{0:0b}'.format(tmp1))]
    
    for bit in tmp1:
        out = (out << 1) | bit

    tmp1 = list('{:04b}'.format(out))
    key[76:] = tmp1[:4]

    tmp = ''.join(map(str, key[15:19]))
    tmp = int(tmp, 2) ^ nr
    tmp = list('{:04b}'.format(tmp))
    key[15:19] = tmp
    
    return key[:64]


def enc(m, key):
    print "m:", m
    print "k:", key
    round_key = [0]*32
    state = [0]*64
    state = int(m, 16)
    state = list(bin(state)[2:])
    c = 0
    
    for i in range(32):
        round_key[i] = key_schedule(key, i)

    print round_key[0], len(round_key[0])
    print state, len(state)
    for i in range(31):
        state = addRoundKey(state, round_key[i])
        state = sBoxLayer(state)
        state = pLayer(state)
        
    state = addRoundKey(state, round_key[31])

    return state


def main():
    key = [random.randint(0, 1) for i in range(80)]
    
    #print key_schedule(key, 1)
    #print enc('0xFFFFFFFFFFFFFFFF', key)
    res =  enc('0xFFFFFFFFFFFFFFFF', key)
    #print enc('0x0000000000000000', ['0' for i in range(80)])
    #print enc('0x0000000000000000', key)
    #res = enc('0xFFFFFFFFFFFFFFFF', ['1' for i in range(80)])

    tmp = [0]*64
    for i in range(64):
        tmp[i] = bin(res[0])[2:]
        
    tmp = ''.join(map(str, tmp))
    n = 8
    tmp1 = [tmp[i:i+n] for i in range(0, len(tmp), n)]

    #tmp = hex(int(tmp1[0], 2))
    #print len(tmp1)
    #for i in range(len(tmp1)):
    #    print hex(int(tmp1[i], 2))
    
    #print tmp1, hex(int(tmp1[0]))
    #print type(tmp[0])
    print hex(int(tmp, 2))
    #print hex()
    
    #print res, "len:", len(res)
    
    
def get_bit(byteval,idx):
    return ((byteval&(1<<idx))!=0);
    
def test(x, y):
    S = [0xe, 0x4, 0xd, 0x1, 0x2, 0xf, 0xb, 0x8, 0x3, 0xa, 0x6, 0xc, 0x5, 0x9, 0x0, 0x7]
    #S = [0xc, 0x5, 0x6, 0xb, 0x9, 0x0, 0xa, 0xd,
    #     0x3, 0xe, 0xf, 0x8, 0x4, 0x7, 0x1, 0x2]

    res = [0]*16

    output = [[0]*16, [0]*16]
    cnt = 0
    for i in range(16):
        tmp = 0
        for j in x:
            tmp ^= get_bit(i, j)
        for j in y:
            tmp ^= get_bit(S[i], j)
               
        if tmp == 0:
            cnt += 1

    print "cnt:", cnt
    return cnt

def test2():
    #test([1,2], [0,1,3])
    #test([0,3], [2])
    test([0,1], [0,3])
    

if __name__ == '__main__':
    test2()
    #main()
