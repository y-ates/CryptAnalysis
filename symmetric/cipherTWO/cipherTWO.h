/******************************************************************************
 ** Copyright (C) 2016 Yakup Ates <Yakup.Ates@rub.de>

 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** any later version.

 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.

 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef _cipherTWO_H
#define _cipherTWO_H

#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <stdlib.h>

class cipherTWO{
private:
	const static char* version;
	const static char SBox[];
	const static char SBox_inv[];
	const static int k_cnt = 3;
	int k[k_cnt];

	int* getKey();

	
public:
	const static char* get_version();
	static int get_k_cnt();
	char getSBox(int index);
	char getSBox_inv(int index);
	std::vector<int> attack(int* m, int* c, int silent);
	std::vector<char> get_influence_SBox(char diff);
	int encrypt(int m, int k[k_cnt]);
	int decrypt(int c, int k[k_cnt], int silent);
	void setKey(int* k_new);
	void usage(char* programname);
	void error_handler(char* programname, int signal);
	
};

#endif
