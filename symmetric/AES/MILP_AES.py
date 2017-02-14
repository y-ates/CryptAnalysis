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
import cplex
from prettytable import PrettyTable


def main():
    prob_s = cplex.Cplex("create_CPLEX-LP_S_10R.txt")
    prob_ss = cplex.Cplex("create_CPLEX-LP_SS_10R.txt")

    prob_s.solve()
    prob_ss.solve()

    print "10 Rounds AES with STATE:", prob_s.solution.get_objective_value()
    print "10 Rounds AES with STATE-STATE:", prob_ss.solution.get_objective_value()

if __name__ == '__main__':
    main()
