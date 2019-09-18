
using namespace std;

#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>

void create_sampled_data(fstream& dat_file, ofstream& sample_file, int rate){
	int length, alph_size;
	dat_file >> length >> alph_size;
	sample_file << length << alph_size << endl;
	for(int i = 0; i < length; ++i){
		ostringstream ostr("", ios::ate);
		bool final;
		int string_length;
		int sampled_string_length = 0;
		dat_file >> final >> string_length;
		for(int j = 0; j < string_length; ++j){
			char symbol;
			double time_value;
			dat_file >> symbol >> time_value;
			int amount = (int)((time_value / (double)rate) + 0.5);
			for(int k = 0; k < amount; ++k){
				sampled_string_length++;
				ostr << " " << symbol;
			}
		}
		sample_file << final << " " << sampled_string_length << ostr.str() << endl;
	}		
}

int main(int argc, const char* argv[]){
	fstream dat_file(argv[1]);
	if(!dat_file.is_open()) return 0;
	
	int rate = atoi(argv[2]);
	
	ostringstream ostr(argv[1], ios::ate);
	ostr << ".s" << rate;
	
	ofstream sample_file(ostr.str().c_str());

	create_sampled_data(dat_file, sample_file, rate);

	return 1;	
}

