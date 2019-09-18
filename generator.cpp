/*
 *  generator.cpp
 *  real-time automaton inference
 *
 *  Created by Sicco Verwer s.e.verwer@tudelft.nl on 08/06/07.
 *  Copyright 2007 Delft University of Technology. All rights reserved.
 *
 */

using namespace std;

#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>

#include "timed_data.h"
#include "timed_automaton.h"
#include "timed_structures.h"

void generate_output(timed_automaton* ta, int data_size, int test_size, string file){
	ostringstream filestr(file, ios::ate);
	filestr << ".aut";
	ofstream str(filestr.str().c_str());
	
	ta->print(str);
	str << endl;
	
	ostringstream filestr2(file, ios::ate);
	filestr2 << ".data";
	str.close();
	str.open(filestr2.str().c_str());

	str << data_size << " " << MAX_SYMBOL << endl;
	for(int i = 0; i < data_size; i++){
		double cont = 1.0;
		int length = 1;
		while(cont > 0.1){
			cont = (double)rand()/((double)(RAND_MAX)+(double)(1));
			length++;
		}
		
		ostringstream ostr("", ios::ate);
		timed_state* st = ta->get_root();
		for(int i = 0; i < length; i++){
			double r   = (double)rand()/((double)(RAND_MAX)+(double)(1));
			int time_value = (int)(r * (MAX_DELAY + 1));
			       r   = (double)rand()/((double)(RAND_MAX)+(double)(1));
			int symbol     = (int)(r * MAX_SYMBOL);
			
			ostr << " " << symbol << " " << time_value << " "; 
			st = st->get_target(symbol, time_value);
		}
		str << st->is_final() << " " << length << ostr.str() << endl;
	}
	str << endl;

	ostringstream filestr3(file, ios::ate);
	filestr3 << ".test";
	str.close();
	str.open(filestr3.str().c_str());

	str << test_size << " " << MAX_SYMBOL << endl;
	for(int i = 0; i < test_size; i++){
		double cont = 1.0;
		int length = 1;
		while(cont > 0.1){
			cont = (double)rand()/((double)(RAND_MAX)+(double)(1));
			length++;
		}
		
		ostringstream ostr("", ios::ate);
		timed_state* st = ta->get_root();
		for(int i = 0; i < length; i++){
			double r   = (double)rand()/((double)(RAND_MAX)+(double)(1));
			int time_value = (int)(r * (MAX_DELAY + 1));
			       r   = (double)rand()/((double)(RAND_MAX)+(double)(1));
			int symbol     = (int)(r * MAX_SYMBOL);
			
			ostr << " " << symbol << " " << time_value << " "; 
			st = st->get_target(symbol, time_value);
		}
		str << st->is_final() << " " << length << ostr.str() << endl;
	}
	str << endl;
}

void random_point_intervals(timed_automaton* aut, const interval_list* list)
{
	const interval_node* node = list->get_head();
	while(node != 0)
	{
		double r = (double)rand()/((double)(RAND_MAX)+(double)(1));
		int state_nr = (int)(r * aut->get_states().size()) + 1;
		timed_state* state = aut->get_state(state_nr);
		node->get_interval()->point(state);
		node = node->next();
	}
}

timed_automaton* generate_timed_automaton(int num_states, int num_splits)
{
	timed_automaton* aut = new timed_automaton();
	for(int i = 1; i < num_states; i++){
		timed_state *s = new timed_state();
		double r   = (double)rand()/((double)(RAND_MAX)+(double)(1));
		if(r >= 0.5) s->set_final(true);
		else s->set_final(false);
		
		aut->add_state(s);
	}
	
	state_list states = aut->get_states();
	
	for(int i = 0; i < num_splits; ++i){
		double r = (double)rand()/((double)(RAND_MAX)+(double)(1));
		int state_nr = (int)(r * num_states) + 1;
		timed_state* state = aut->get_state(state_nr);
				
		double rs = (double)rand()/((double)(RAND_MAX)+(double)(1));
		int symbol = (int)(rs * MAX_SYMBOL);

		double rt = (double)rand()/((double)(RAND_MAX)+(double)(1));
		double time_value = (int)(rt * MAX_DELAY);
			
		state->split(symbol, (double)time_value);
	}

	for(state_list::iterator state_it = states.begin(); state_it != states.end(); ++state_it){
		timed_state* state = *state_it;
		for(int i = 0; i < MAX_SYMBOL; i++){
			random_point_intervals(aut, state->get_intervals(i));
		}
	}
	return aut;
}

void generate_timed_data(int data_size, int test_size, int alph_size, int time_size, int num_states, int num_splits, string file){
	MAX_SYMBOL = alph_size;
	MIN_DELAY = 0;
	MAX_DELAY = time_size;
	MIN_TIME_STEP = 1;
	timed_automaton* aut = generate_timed_automaton(num_states, num_splits);
	generate_output(aut, data_size, test_size, file);	
}

int main(int argc, const char* argv[]){
	int data_size = atoi(argv[1]);
	int test_size = atoi(argv[2]);
	int alph_size = atoi(argv[3]);
	int time_size = atoi(argv[4]);
	int num_states = atoi(argv[5]);
	int num_splits = atoi(argv[6]);
	int set_nr     = atoi(argv[7]);

	ostringstream ostr("real-time.[", ios::ate);
	ostr << alph_size << "," << time_size << "]." << num_states << ":" << num_splits << "-" << data_size << "-" << set_nr; 
	
	cerr << "starting generation..." << endl;

	generate_timed_data(data_size, test_size, alph_size, time_size, num_states, num_splits, ostr.str());
	return 1;	
}
