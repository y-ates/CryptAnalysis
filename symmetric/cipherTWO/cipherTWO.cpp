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

#include "cipherTWO.h"

const char cipherTWO::SBox[] = {
	6, 4, 12, 5, 0, 7, 2, 14,
	1, 15, 3, 13, 8, 10, 9, 11
};

const char cipherTWO::SBox_inv[] = {
	4, 8, 6, 10, 1, 3, 0, 5,
	12, 14, 13, 15, 2, 11, 7, 9
};


const char* cipherTWO::version = "1.0";


char cipherTWO::getSBox(int index){
	return SBox[index%16];
}

char cipherTWO::getSBox_inv(int index){
	return SBox_inv[index%16];
}

int cipherTWO::get_k_cnt(){
	return k_cnt;
}

const char* cipherTWO::get_version(){
	return version;
}

int* cipherTWO::getKey(){
	return k;
}

void cipherTWO::setKey(int* k_new){
	for(int i=0; i<k_cnt; ++i){
		k[i] = k_new[i] % 16;
	}
}

int cipherTWO::encrypt(int m, int k[k_cnt]){
	char u, v, w, x, c;

	for(int i=0; i<k_cnt; ++i){
		k[i] %= 16;
	}

	m %= 16;	
	u = m ^ k[0];
	u %= 16;
	
	v = getSBox(u);
	v %= 16;

	w = v ^ k[1];
	w %= 16;

	x = getSBox(w);
	x %= 16;
	
	c = x ^ k[2];
	c %= 16;

	std::cout << "Encrypting with:"
		  << std::endl
		  << "---"
		  << std::endl
		  << "m : " << int(m)
		  << std::endl
		  << "k0: " << int(k[0])
		  << std::endl
		  << "k1: " << int(k[1])
		  << std::endl
		  << "k2: " << int(k[2])
		  << std::endl
		  << "---"
		  << std::endl << std::endl
		  << "Results:"
		  << std::endl
		  << "---"
		  << std::endl
		  << "u: " << int(u)
		  << std::endl
		  << "v: " << int(v)
		  << std::endl
		  << "w: " << int(w)
		  << std::endl
		  << "x: " << int(x)
		  << std::endl
		  << "c: " << int(c)
		  << std::endl
		  << "---"
		  << std::endl;
	
	return int(c);
}

int cipherTWO::decrypt(int c, int k[k_cnt], int silent=0){
	char u, v, w, x, m;

	for(int i=0; i<k_cnt; ++i){
		k[i] %= 16;
	}

	c %= 16;	
	x = c ^ k[2];
	x %= 16;
	
	w = getSBox_inv(x);
	w %= 16;

	v = w ^ k[1];
	v %= 16;

	u = getSBox_inv(v);
	u %= 16;
       
	m = u ^ k[0];
	m %= 16;

	if(silent == 0){
		std::cout << "Decrypting with:"
		  << std::endl
		  << "---"
		  << std::endl
		  << "c : " << int(c)
		  << std::endl
		  << "k0: " << int(k[0])
		  << std::endl
		  << "k1: " << int(k[1])
		  << std::endl
		  << "k2: " << int(k[2])
		  << std::endl
		  << "---"
		  << std::endl << std::endl
		  << "Results:"
		  << std::endl
		  << "---"
		  << std::endl
		  << "u: " << int(u)
		  << std::endl
		  << "v: " << int(v)
		  << std::endl
		  << "w: " << int(w)
		  << std::endl
		  << "x: " << int(x)
		  << std::endl
		  << "m: " << int(m)
		  << std::endl
		  << "---"
		  << std::endl;
	}
	
	return int(m);
}

std::vector<char> cipherTWO::get_influence_SBox(char diff=15){
	std::ios state(NULL);
	state.copyfmt(std::cout);
	
	const int table_size = 16;
	const char sep = ' ';
	const int entry_width = 12;
	char v0[table_size], v1[table_size], u0[table_size], u1[table_size];
	std::vector<char> v_diff(table_size);
	
	for(int i=0; i<table_size; ++i){
		u0[i] = i;
		u0[i] %= 16;
		u1[i] = i ^ diff;
		u1[i] %= 16;

		v0[i] = getSBox(i);
		v1[i] = getSBox(u1[i]);

		v_diff[i] = v0[i] ^ v1[i];
	}

	std::cout << std::left << std::setw(entry_width) << std::setfill(sep)
		  << "u0";
	std::cout << std::left << std::setw(entry_width) << std::setfill(sep)
		  << "u1=u0^15";
	std::cout << std::left << std::setw(entry_width) << std::setfill(sep)
		  << "v0=S(u0)";
	std::cout << std::left << std::setw(entry_width) << std::setfill(sep)
		  << "v1=S(u1)";
	std::cout << std::left << std::setw(entry_width) << std::setfill(sep)
		  << "v1^v2" << std::endl;
	std::cout << "-------------------------------------------------------"
		  << std::endl;
	
	for(int i=0; i<table_size; ++i){
		std::cout << std::left << std::setw(entry_width)
			  << std::setfill(sep) << int(u0[i]);
		std::cout << std::left << std::setw(entry_width)
			  << std::setfill(sep) << int(u1[i]);
		std::cout << std::left << std::setw(entry_width)
			  << std::setfill(sep) << int(v0[i]);
		std::cout << std::left << std::setw(entry_width)
			  << std::setfill(sep) << int(v1[i]);
		std::cout << std::left << std::setw(entry_width)
			  << std::setfill(sep) << int(v_diff[i])
			  << std::endl;
	}
	// reset std::cout state
	std::cout.copyfmt(state);

	//std::vector<int> v_diff_copy(v_diff);
	sbox_statistics(v_diff, table_size, 0);

	return v_diff;
}

std::vector<int> cipherTWO::sbox_statistics(std::vector<char> v_diff_copy, int table_size, int silent=1){
	int max_occ[table_size] = {0};
	int max = 0;
	int element = 0;
	std::vector<int> result(2);
	
	std::sort(v_diff_copy.begin(), v_diff_copy.end());
	for(int i=0; i<table_size; ++i){
		max_occ[int(v_diff_copy[i])] += 1;
	}

	for(int i=0; i<table_size; ++i){
		if(max_occ[i] > max){
			max = max_occ[i];
			element = i;
		}
	}

	if(silent == 0){
		std::cout << std::endl << "[+] NOTE: Element "
			  << element << " occurs in " << max << "/"
			  << table_size << " times."
			  << std::endl;

	}
	
	result[0] = element;
	result[1] = max;

	return result;
}

std::vector<int> cipherTWO::attack(int* m, int* c, int silent=0){
	const int table_size = 16;
	int v_diff = 0;
	int pair_cnt;
	std::vector<int> possible_keys;

	int m_size = sizeof(m)/sizeof(int);
	int c_size = sizeof(c)/sizeof(int);
	if(m_size == 0 || c_size == 0)
		return (std::vector<int>)1;
	
	if(m_size != c_size){
		if(m_size > c_size){
			pair_cnt = c_size;
		} else {
			pair_cnt = m_size;
		}
	} else {
		pair_cnt = m_size;
	}
	
	if(silent == 0){
		std::cout << "Attacking with:"
			  << std::endl
			  << "---"
			  << std::endl;
		for(int i=0; i<m_size; ++i){
			std::cout << "m" << i << ": "
				  << int(m[i]) << std::endl;
			std::cout << "c" << i << ": " << int(c[i])
				  << std::endl;
		}
		std::cout << "---"
			  << std::endl;
	}

	for(int i=0; i<pair_cnt; ++i){
		m[i] %= 16;
		c[i] %= 16;

		//m_diff ^= m[i];
	}

	

	int v_[pair_cnt];
	int u_[pair_cnt];
	int w_[pair_cnt];
	int x_[pair_cnt];
	for(int i=0; i<table_size; ++i){
		int c_diff = 0;
		
		for(int j=0; j<pair_cnt; ++j){
			x_[j] = c[j] ^ i;
			w_[j] = getSBox_inv(v_[j]);
		}

		for(int j=0; j<pair_cnt; ++j){
			c_diff ^= w_[j];
		}

		if(c_diff == v_diff)
			possible_keys.push_back(i);
	}

	if(silent == 0){
		int keys_size = possible_keys.size();

		if(keys_size > 0){
			std::cout << std::endl
				  << "Results:"
				  << std::endl
				  << "---"
				  << std::endl;

			if(keys_size == 1){
				std::cout << "Possible key: ";
			} else {
				std::cout << "Possible keys: ";
			}

			for(int i=0; i<keys_size; ++i){
				if(i != keys_size-1)
					std::cout << possible_keys[i] << ", ";
				else
					std::cout << possible_keys[i] << std::endl;
			}

			std::cout << "---" << std::endl;			
		} else {
			std::cout << "[-]"
				  << " Warning: "
				  << "I found no possible keys... :-("
				  << std::endl;
		}

	}
	
	return possible_keys;
}


/*
 * Signals:
 * 0 - too few arguments
 * 1 - too many arguments, run if possible but return warning.
 * 2 - weird character type. Stop and return error.
 */
void cipherTWO::error_handler(char* programname, int signal){
	switch(signal){
	case 0:
		usage(programname);
		std::cout << std::endl;
		std::cout << "[-] Error:"
			  << " You gave me too few arguments."
			  << std::endl;
		
		break;
	case 1:
		//usage(programname);
		std::cout << std::endl;
		std::cout << "[-] Warning:"
			  << " You entered more arguments than expected."
			  << " I will only choose the first few..."
			  << std::endl << std::endl;
		break;
	case 2:
		usage(programname);
		std::cout << std::endl;
		std::cout << "[-] Error:"
			  << " You gave me weird characters."
			  << " I can't handle these characters. Stopping."
			  << std::endl;
		break;
	}
}

void cipherTWO::usage(char* programname){
	std::cout << programname << " " << get_version()
		  << " usage: "
		  << std::endl
		  << "###"
		  << std::endl
		  << "----"
		  << std::endl
		  << " [-e|--encrypt] <message byte> <key byte 0> <key byte 1> <key byte 2>:"
		  << "    Encrypt plaintext-byte with cipherTWO."
		  << std::endl
		  << " [-d|--decrypt] <cipher byte> <key byte 0> <key byte 1> <key byte 2>:"
		  << "    Decrypt cipher-byte with cipherTWO."
		  << std::endl
		  << " [-a|--attack] <message byte 0> <message byte 1> <cipher byte 0> <cipher byte 1>:"
		  << "    Attack cipherTWO via a differential attack. (Chosen-Plaintext Attack)"
		  << std::endl
		  << " [-as|--analyse-sbox] <character>:"
		  << "    Analyse S-Box for given character"
		  << std::endl
		  << " [-h|--help]:"
		  << "    Print this help message."
		  << std::endl
		  << "----"
		  << std::endl;
		  // << std::endl << std::endl
		  // << "License"
		  // << std::endl
		  // << "###"
		  // << std::endl
		  // << "Copyright (C) 2016 Yakup Ates <Yakup Ates@rub.de>"
		  // << std::endl
		  // << programname << " " << get_version()
		  // << " comes with ABSOLUTELY NO WARRANTY."
		  // << std::endl
		  // << "You may redistribute copies of "
		  // << programname << " " << get_version()
		  // << " under the terms of the GNU General Public License."
		  // << std::endl;
}

int main(int argc, char** argv){
	cipherTWO cipher;

	if(argc == 1){
		cipher.usage(argv[0]);
		return 1;
	}
	
	if((std::string(argv[1]) == "-h") || (std::string(argv[1]) == "--help")){
		cipher.usage(argv[0]);		
	} else if((std::string(argv[1]) == "-e")
		  || (std::string(argv[1]) == "--encrypt")){
		int expect_arg_cnt = cipher.get_k_cnt()+3;
				
		if(argc < expect_arg_cnt){
			cipher.error_handler(argv[0], 0);
			return 1;
		} else if(argc > expect_arg_cnt){
			cipher.error_handler(argv[0], 1);
		}
		
		int key[cipher.get_k_cnt()] = {atoi(argv[3]), atoi(argv[4]),
					       atoi(argv[5])};
		//cipher.setKey(key);
		cipher.encrypt(atoi(argv[2]), key);
	} else if((std::string(argv[1]) == "-d")
		  || (std::string(argv[1]) == "--decrypt")){
		int expect_arg_cnt = cipher.get_k_cnt()+3;
				
		if(argc < expect_arg_cnt){
			cipher.error_handler(argv[0], 0);
			return 1;
		} else if(argc > expect_arg_cnt){
			cipher.error_handler(argv[0], 1);
		}
		
		int key[cipher.get_k_cnt()] = {atoi(argv[3]), atoi(argv[4]),
					       atoi(argv[5])};
		//cipher.setKey(key);
		cipher.decrypt(atoi(argv[2]), key);
	} else if((std::string(argv[1]) == "-a")
		  || (std::string(argv[1]) == "--attack")){
		int expect_arg_cnt = 6;
		int count_pair = 2;
				
		if(argc < expect_arg_cnt){
			cipher.error_handler(argv[0], 0);
			return 1;
		} else if(argc > expect_arg_cnt){
			cipher.error_handler(argv[0], 1);
		}

		int m[count_pair] = {atoi(argv[2]), atoi(argv[3])};
		int c[count_pair] = {atoi(argv[4]), atoi(argv[5])};
		cipher.attack(m, c);
	} else if((std::string(argv[1]) == "-as")
		  || (std::string(argv[1]) == "--analyse-sbox")){
		int expect_arg_cnt = 2;
				
		if(argc < expect_arg_cnt){
			cipher.error_handler(argv[0], 0);
			return 1;
		} else if(argc > expect_arg_cnt){
			int char_test = atoi(argv[2]);
			cipher.get_influence_SBox(char_test);
		} else if(argc > expect_arg_cnt+1){
			cipher.error_handler(argv[0], 1);
			int char_test = atoi(argv[2]);
			cipher.get_influence_SBox(char_test);
		} else {
			cipher.get_influence_SBox();
		}
	} else {
		cipher.usage(argv[0]);
	}
	
	return 0;
}
	
