#!/usr/bin/env python
#
# Copyright (C) 2018 Yakup Ates <Yakup.Ates@rub.de>
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
import itertools

def SBox(i):
    SBox = [0xe, 0x4, 0xd, 0x1, 0x2, 0xf, 0xb, 0x8,
            0x3, 0xa, 0x6, 0xc, 0x5, 0x9, 0x0, 0x7]

    return SBox[i]

def get_bit(byteval,idx):
    return ((byteval&(1<<idx))!=0);

# Example - 3.3 Analyzing the Cipher Components
def calculate_bias():
    # Using linear expression: X2 + X3 = Y1 + X3 + Y4

    for i in range(16):
        # input
        X2 = get_bit(i, 2)
        X3 = get_bit(i, 3)

        # output
        Y = SBox(i)
        Y1 = get_bit(i, 1)
        Y3 = get_bit(i, 3)
        Y4 = get_bit(i, 4)

        # calculate linear expression
        s = X2 ^ X3 ^ Y1 ^ Y3 ^ Y4

        counter = [0] * 16
        if s == 0:
            # candidate found
            print "Input:", "{0:b}".format(i)
            print "Output:", "{0:b}".format(Y)
            print " => X2 ^ X3 ^ Y1 ^ Y3 ^ Y4 = 0"

            counter[i] = counter[i] + 1
            print "Counter:", counter[i]
        

if __name__ == '__main__':
    calculate_bias()
    
