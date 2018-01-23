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
# from prettytable import PrettyTable

linear_expressions = [0] * 16

# Simply XOR of state and key
def addRoundKey(state, key):
    return state ^ key


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
    # J0*X0 + J1*X1 + J2*X2 + J3*X3 + J4*X4 = J0*Y0 + J1*Y1 + J2*Y2 + J3*Y3 + J4*Y4

    counter = 0
    for i in range(16):
        s = 0
        
        for j in x:
            s ^= get_bit(i, j) # calculate X side of linear expression
        for j in y:
            s ^= get_bit(SBox(i), j) # calculate Y side of linear expression

        if s == 0:
            # both sides are equal, add 1 to bias
            counter += 1

    return counter-8

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

def test_one_bit_in_out():
    linear_approximation_table = [[0 for x in range(4)] for y in range(4)]
    
    X = [1, 2, 4, 8]
    Y = [1, 2, 4, 8]

    for i in range(4):
        for j in range(4):
            linear_approximation_table[i][j] = \
                calculate_bias(linear_expressions[i], linear_expressions[j])

    return linear_approximation_table


def print_counter_table_one_bit_in_out():
    linear_approximation_table = test_one_bit_in_out()
    
    for i in range(4):
        sys.stdout.write("\r\n")
        sys.stdout.flush()
        for j in range(4):
            sys.stdout.write(" " + str(linear_approximation_table[i][j]) + "\t")
            sys.stdout.flush()
    sys.stdout.write("\r\n")
    sys.stdout.flush()

    
# Generate all possible linear expressions and initiate linear approximation
# table 
if __name__ == '__main__':
    print "Linear Attack on PRESENT"
    print "Yakup Ates, 2018\r\n\r\n"

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
    
    
    
    
