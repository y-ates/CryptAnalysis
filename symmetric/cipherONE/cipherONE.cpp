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

#include "cipherONE.h"


const char cipherONE::SBox[] = {
	6, 4, 12, 5, 0, 7, 2, 14,
	1, 15 , 3, 13, 8, 10, 9, 11
};

const char cipherONE::SBox_inv[] = {
	4, 8, 6, 10, 1, 3, 0, 5,
	12, 14, 13, 15, 2, 11, 7, 9,
};


const char* cipherONE::version = "1.0";


char cipherONE::getSBox(int index){
	return SBox[index%16];
}

char cipherONE::getSBox_inv(int index){
	return SBox_inv[index%16];
}

int cipherONE::get_k_cnt(){
	return k_cnt;
}

const char* cipherONE::get_version(){
	return version;
}

int* cipherONE::getKey(){
	return k;
}

void cipherONE::setKey(int* k_new){
	for(int i=0; i<k_cnt; ++i){
		k[i] = k_new[i] % 16;
	}
}

int cipherONE::encrypt(int m, int k[k_cnt]){
	char u, v, c;

	for(int i=0; i<k_cnt; ++i){
		k[i] %= 16;
	}

	m %= 16;	
	u = m ^ k[0];
	u %= 16;
	
	v = getSBox(u);
	v %= 16;
	
	c = v ^ k[1];
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
		  << "c: " << int(c)
		  << std::endl
		  << "---"
		  << std::endl;
	
	return int(c);
}

int cipherONE::decrypt(int c, int k[k_cnt], int silent=0){
	char u, v, m;

	for(int i=0; i<k_cnt; ++i){
		k[i] %= 16;
	}

	c %= 16;	
	v = c ^ k[1];
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
		  << "m: " << int(m)
		  << std::endl
		  << "---"
		  << std::endl;
	}
	
	return int(m);
}

std::vector<int> cipherONE::attack(int* m, int* c, int silent=0){
	const int pair_cnt = 2;
	const int table_size = 16;
	int m_diff = 0;	
	std::vector<int> possible_keys;

	if(silent == 0){
		std::cout << "Attacking with:"
			  << std::endl
			  << "---"
			  << std::endl
			  << "m0: " << int(m[0])
			  << std::endl
			  << "m1: " << int(m[1])
			  << std::endl
			  << "c0: " << int(c[0])
			  << std::endl
			  << "c1: " << int(c[1])
			  << std::endl
			  << "---"
			  << std::endl;
	}

	for(int i=0; i<pair_cnt; ++i){
		m[i] %= 16;
		c[i] %= 16;

		m_diff ^= m[i];
	}

	int v_[pair_cnt];
	int u_[pair_cnt];
	for(int i=0; i<table_size; ++i){
		int c_diff = 0;
		
		for(int j=0; j<pair_cnt; ++j){
			v_[j] = c[j] ^ i;
			u_[j] = getSBox_inv(v_[j]);
		}

		for(int j=0; j<pair_cnt; ++j){
			c_diff ^= u_[j];
		}

		if(c_diff == m_diff)
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
void cipherONE::error_handler(char* programname, int signal){
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

void cipherONE::usage(char* programname){
	std::cout << programname << " " << get_version()
		  << " usage: "
		  << std::endl
		  << "###"
		  << std::endl
		  << "----"
		  << std::endl
		  << " [-e|--encrypt] <message byte> <key byte 0> <key byte 1>:"
		  << "    Encrypt plaintext-byte with cipherONE."
		  << std::endl
		  << " [-d|--decrypt] <cipher byte> <key byte 0> <key byte 1>:"
		  << "    Decrypt cipher-byte with cipherONE."
		  << std::endl
		  << " [-a|--attack] <message byte 0> <message byte 1> <cipher byte 0> <cipher byte 1>:"
		  << "    Attack cipherONE via a differential attack. (Chosen-Plaintext Attack)"
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
	cipherONE cipher;

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
		
		int key[cipher.get_k_cnt()] = {atoi(argv[3]), atoi(argv[4])};
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
		
		int key[cipher.get_k_cnt()] = {atoi(argv[3]), atoi(argv[4])};
		//cipher.setKey(key);
		cipher.decrypt(atoi(argv[2]), key);
	} else if((std::string(argv[1]) == "-a")
		  || (std::string(argv[1]) == "--attack")){
		int expect_arg_cnt = 6;
				
		if(argc < expect_arg_cnt){
			cipher.error_handler(argv[0], 0);
			return 1;
		} else if(argc > expect_arg_cnt){
			cipher.error_handler(argv[0], 1);
		}

		int m[cipher.get_k_cnt()] = {atoi(argv[2]), atoi(argv[3])};
		int c[cipher.get_k_cnt()] = {atoi(argv[4]), atoi(argv[5])};
		cipher.attack(m, c);
	} else {
		cipher.usage(argv[0]);
	}
	
	return 0;
}
	
