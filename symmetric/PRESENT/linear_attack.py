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
import copy

linear_expressions = [0] * 16


################################################################################
# PRESENT functions
################################################################################


# Simply XOR of state and key
def addRoundKey(state, key):
    return state ^ key


# Permutation layer - using P-Box
def pLayer(state):
    res = 0
    for i in range(64):
        res += ((state >> i) & 0x01) << PBox(i)

    return res


# Key scheduler using the key and the specific round number
def key_schedule(key, nr):
    roundkeys = []
    key = string2number(key)

    for i in xrange(1, 32+1): # (K1 ... K32)
        roundkeys.append(key >>16)
        key = ((key & (2**19-1)) << 61) + (key >> 19)
        key = (SBox(key >> 76) << 76)+(key & (2**76-1))
        key ^= i << 15

    return roundkeys


def string2number(i):
    """ Convert a string to a number

    Input: string (big-endian)
    Output: long or integer
    """
    return int(i.encode('hex'),16)


def number2string_N(i, N):
    """Convert a number to a string of fixed size

    i: long or integer
    N: length of string
    Output: string (big-endian)
    """
    s = '%0*x' % (N*2, i)
    return s.decode('hex')


# SBoxLayer for the given state
def sBoxLayer(state):
    res = 0

    for i in xrange(16):
        res += SBox(( state >> (i*4)) & 0xF) << (i*4)

    return res


# PBox used in pLayer
def PBox(i):
    p = [0, 16, 32, 48, 1, 17, 33, 49, 2, 18, 34, 50, 3, 19, 35, 51,
         4, 20, 36, 52, 5, 21, 37, 53, 6, 22, 38, 54, 7, 23, 39, 55,
         8, 24, 40, 56, 9, 25, 41, 57, 10, 26, 42, 58, 11, 27, 43, 59,
         12, 28, 44, 60, 13, 29, 45, 61, 14, 30, 46, 62, 15, 31, 47, 63]

    return p[i]


# Actually encrypting m with a key
def enc(m, key):
    round_key = key_schedule(key, 32)

    state = string2number(m)
    for i in range(31):
        state = addRoundKey(state, round_key[i])
        state = sBoxLayer(state)
        state = pLayer(state)

    state = addRoundKey(state, round_key[31])

    return number2string_N(state,8)


def SBox(i):
    # Example S-Box of paper by Howard M. Heys "Linear and Differential
    # Cryptanalysis"
    # SBox = [0xe, 0x4, 0xd, 0x1, 0x2, 0xf, 0xb, 0x8,
    #        0x3, 0xa, 0x6, 0xc, 0x5, 0x9, 0x0, 0x7]

    # S-Box of PRESENT
    SBox = [0xc, 0x5, 0x6, 0xb, 0x9, 0x0, 0xa, 0xd,
            0x3, 0xe, 0xf, 0x8, 0x4, 0x7, 0x1, 0x2]

    return SBox[i]


# Returns the individual bit of an byte (byteval) at index (idx)
def get_bit(byteval, idx):
    return ((byteval&(1<<idx))!=0)

def test_encryption():
    print "Testing PRESENT encryption..."
    print "m:", "FFFFFFFFFFFFFFFF"
    print "key:", "00000000000000000000"
    print "expected cipher:", "a112ffc72f68417b"
    print

    m = "FFFFFFFFFFFFFFFF".decode('hex')
    key = "00000000000000000000".decode('hex')
    expected_cipher = "a112ffc72f68417b"
    res = enc(m, key)

    if res.encode('hex') == expected_cipher:
        print " Encryption works as expected!"
        print
        print


################################################################################
# Task functions
################################################################################


# Create all possible linear expressions
# @param affine holds the actual activated bits in the expression
# @param a temporary needed to set the positions that are activated
# @param i all possible 16 values
# @param j check all possible 4 positions which are 1
def create_linear_expression():
    affine = [0]*16
    for i in range(16):
        a = list()
        for j in range(4):
            if get_bit(i, j):
                a.append(j)
        affine[i] = a

    return affine


# Example - 3.3 Analyzing the Cipher Components
def calculate_bias(x, y):
    # Using linear expression:
    #
    # X: Input
    # Y: Output
    # A0*X0 + A1*X1 + A2*X2 + A3*X3 + A4*X4 = B0*Y0 + B1*Y1 + B2*Y2 + B3*Y3 + B4*Y4

    counter = 0
    for i in range(16): # input for sbox
        s = 0

        for j in x:
            s ^= get_bit(i, j) # calculate X side of linear expression
        for j in y:
            s ^= get_bit(SBox(i), j) # calculate Y side of linear expression

        if s == 0:
            # both sides are equal, add 1 to bias
            counter += 1

    return counter-8 # bias*16; -8 is to calculate 1/2 into bias


# Iterate over all possible (m, c) pairs and return linear approximation table
def test_all_bias():
    linear_approximation_table = [[0 for x in range(16)] for y in range(16)]

    for i in range(16):
        for j in range(16):
            linear_approximation_table[i][j] = \
                calculate_bias(linear_expressions[i], linear_expressions[j])

    return linear_approximation_table


# Print linear approximation table containing all bias
# Dividing an element value by 16 gives the probability bias for the particular
# linear combination of input and output bits
def print_counter_table():
    linear_approximation_table = test_all_bias()

    for i in range(16):
        sys.stdout.write("\r\n")
        sys.stdout.flush()
        for j in range(16):
            sys.stdout.write(" " + str(linear_approximation_table[i][j]) + "\t")
            sys.stdout.flush()
    sys.stdout.write("\r\n")
    sys.stdout.flush()


def print_counter_table_one_bit_in_out():
    linear_approximation_table = test_all_bias()
    X = [1, 2, 4, 8]
    Y = [1, 2, 4, 8]

    for i in X:
        sys.stdout.write("\r\n")
        sys.stdout.flush()
        for j in Y:
            sys.stdout.write(" " + str(linear_approximation_table[i][j]) + "\t")
            sys.stdout.flush()
    sys.stdout.write("\r\n")
    sys.stdout.flush()


def print_linear_characteristic_for_some_rounds():
    print "Input: (1, 0, 0, 0), (0, ...) ..."
    print "Permutation laesst die Maske fix"
    print "<(1,0,0,0), x> = <(1,0,0,0), S(x)> hat einen bias von -2 (siehe oben)"
    print "1 runden iterative charakteristik von (8,0,0...) -> (8,0,0,...)"



def linear_hull(inMask, outMask, R):
    return linear_hull0(inMask, outMask, R-1)

    
def linear_hull0(inMask, outMask, R):
    print "inmask:", inMask, "outMask:", outMask, "R:", R
    if (inMask % 4 == 3) or (outMask % 4 == 3):
        return 0
    
    if R == 0:
        if (inMask // 4) == (outMask // 4):
            return 1
        else:
            return 0

    sum_ = 0
    for i in range(3):
        sbox = inMask//4

        linear_layer = PBox(sbox*4+i)        
        if (linear_layer % 4) == 3:
            continue

        sum_ = sum_ + linear_hull0(linear_layer, outMask, R-1)

    print
    print "inmask:", inMask, "outMask:", outMask, "R:", R, "Sum:", sum_
    return sum_

            
    
# Generate all possible linear expressions and initiate linear approximation
# table
if __name__ == '__main__':
    print "Linear Attack on PRESENT"

    print "- Compute the linear approximation table for the PRESENT S-Box"
    linear_expressions = create_linear_expression()
    # P1.1 - Task 1
    print_counter_table()
    print "- done.\r\r"
    print
    print
    print "- Find all biased linear approximation with a one bit input and output mask."
    # P1.1 - Task 2
    print_counter_table_one_bit_in_out()
    print "- done.\r\r"
    print
    print
    print "- Find a linear characteristic for some rounds with only one active S-Box per round."
    # P1.1 - Task 3
    print_linear_characteristic_for_some_rounds()
    print "- done.\r\r"
    print
    print
    print "- Compute its bias."
    # P1.1 - Task 4
    print "Bias wurde bereits berechnet und ist bei: -2/16"
    print "- done.\r\r"
    print
    print
    print "- Why do all these characteristic have the same absolute bias?"
    # P1.1 - Task 5
    print "Weil die SBox gut ausbalanciert ist und PRESENT sicher gegen diese Art von Angriff"
    print "- done.\r\r"
    print
    print
    print """- Find the total number of linear characteristics in the linear
    hull over r rounds with only one active S-Box per round (For any given
    one-bit input and output mask)."""
    # P1.1 - Task 6
    print linear_hull(0, 0, 4)
    print "- done.\r\r"
    print
    print
    print """- Execute some experiments to see how the bias is distributed over
    the key."""
    # P1.1 - Task 7
    #print_linear_characteristic_for_some_rounds()
    print "- done.\r\r"
    print
    print

    # Test if PRESENT is implemented correctly by encrypting for a known (m,c)
    # pair
    test_encryption()
