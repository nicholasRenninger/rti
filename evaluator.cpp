/*
 *  main.cpp
 *  IICT
 *
 *  Created by Sicco Verwer on 10/11/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

using namespace std;

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "timed_data.h"
#include "timed_automaton.h"
#include "timed_structures.h"

void evaluate_timed_automaton(timed_automaton* aut, timed_input* in, int start_index){
	int true_pos = 0;
	int false_pos = 0;
	int true_neg = 0;
	int false_neg = 0;
	
	for(int i = 0; i < start_index; i++){
		timed_word* w = in->get_word(i);
		timed_state* s = aut->get_root();
		for(int k = 0; k < w->get_length() && s != 0; k++){
			s = s->get_target(w->get_symbols()[k], w->get_time_values()[k]);
		}
		if(s == 0) continue;
		
		if(w->is_positive()&& s->is_final())
			true_pos++;
		else if(w->is_positive()&& !s->is_final())
			false_pos++;
		else if(s->is_final())
			false_neg++;
		else
			true_neg++;
	}
	cerr << "On test set:\ntp: " << true_pos << " fp: " << false_pos <<" tn: " << true_neg << " fn: " << false_neg << endl;
	
	true_pos = 0;
	false_pos = 0;
	true_neg = 0;
	false_neg = 0;
	
	for(int i = start_index; i < NUM_WORDS; i++){
		timed_word* w = in->get_word(i);
		timed_state* s = aut->get_root();
		for(int k = 0; k < w->get_length() && s != 0; k++){
			s = s->get_target(w->get_symbols()[k], w->get_time_values()[k]);
		}
		if(s == 0) continue;
		
		if(w->is_positive()&& s->is_final())
			true_pos++;
		else if(w->is_positive()&& !s->is_final())
			false_pos++;
		else if(s->is_final())
			false_neg++;
		else
			true_neg++;
	}
	cout << "tp: " << true_pos << " fp: " << false_pos <<" tn: " << true_neg << " fn: " << false_neg << endl;
}

int main(int argc, const char* argv[]){
	fstream aut_file(argv[1]);
	if(!aut_file.is_open()) return 0;
	timed_automaton* aut = new timed_automaton(aut_file);

	fstream test_file(argv[2]);
	if(!test_file.is_open()) return 0;
	timed_input* in = new timed_input(test_file);
	
	int start = atoi(argv[3]);
	
	evaluate_timed_automaton(aut, in, start);

	return 1;	
}
