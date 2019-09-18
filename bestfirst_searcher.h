/*
 *  bestfirst_searcher.h
 *  IICT
 *
 *  Created by Sicco Verwer on 3/28/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _BESTFIRST_SEARCHER_H_
#define _BESTFIRST_SEARCHER_H_

using namespace std;

#include <fstream>
#include "timed_automaton.h"
#include "globals.h"
#include "score.h"

//#include <multiset>
#include <list>
#include <queue>
#include <map>
#include <set>

class refinement;
struct refinement_compare;
struct reflist_compare;
struct timed_tail_compare;
typedef multiset<refinement*, refinement_compare> refset;
typedef multiset<timed_tail*, timed_tail_compare> sorted_tails;
typedef list<refinement*> reflist;

timed_automaton* TA = 0;
int SIZE = 1;

int get_num_transitions(){
	return SIZE;
	int result = 0;
	state_list states = TA->get_states();
	for(state_list::iterator it = states.begin(); it != states.end(); ++it){
		for(int i = 0; i < MAX_SYMBOL; ++i){
			timed_state* target = 0;
			const interval_list* list = (*it)->get_intervals(i);
			for(const interval_node* node = list->get_head(); node != 0; node = node->next()){
				if(target == 0 || node->get_interval()->get_target() != target) result++;
					target = node->get_interval()->get_target();
			}
		}
	}
	return result;
};

int get_num_split_transitions(){
	return SIZE;
	int result = 0;
	state_list states = TA->get_states();
	for(state_list::iterator it = states.begin(); it != states.end(); ++it){
		for(int i = 0; i < MAX_SYMBOL; ++i){
			const interval_list* list = (*it)->get_intervals(i);
			if(list->get_head() != list->get_tail())
				result++;
		}
	}
	return result;
};

class refinement
{
public:
	int state;
	int target;

	int symbol;
	double time;
	double score;
	
	bool split;

	interval* in;
	int size;

	int ref_count;
	
	refinement(int s, interval* i, int t, int sy, double ti, bool sp);

	inline void print() const{
		if(!split){
			if(target != 0)
				cerr << "point( " << state << " [" << symbol << ", " << time << "]->" << target << " )";
			else
				cerr << "new( " << state << " [" << symbol << ", " << time << "]-> new )";
		} else {
				cerr << " split( " << state << " [" << symbol << " , " << time << "] )";
		}
		cerr << " score: " << score << endl;
	};
	
	void refine();
	void undo_refine();
	
	inline void set_score(){
		size = get_num_transitions();
		score = CONSISTENT_TAILS - INCONSISTENT_TAILS + CONSISTENT_FINALS - TIMED_INCONSISTENCY + TIMED_CONSISTENCY;
	};

	inline void set_score(double s){
		score = s;
	};
	
	inline const double get_score() const{
		return score;
	};
};


double sum_of_interval_squares(double a, double b, double c){
	return (a - b)*(a - b) + (b - c)*(b - c);
};

bool less_refinement(const refinement *ref1, const refinement *ref2){
	if(ref1->score > ref2->score) return 1;
	if(ref1->score < ref2->score) return 0;
	if(ref1->size < ref2->size) return 1;
	if(ref1->size > ref2->size) return 0;
	if(!ref1->split && !ref2->split){
		interval* in1 = TA->get_state(ref1->state)->get_interval(ref1->symbol, ref1->time);
		interval* in2 = TA->get_state(ref2->state)->get_interval(ref2->symbol, ref2->time);
		return sum_of_interval_squares(in1->get_end(), ref1->time, in1->get_begin())
			   < sum_of_interval_squares(in2->get_end(), ref2->time, in2->get_begin());
	}
	return 0;
};

struct refinement_compare{
  bool operator()(const refinement *ref1, const refinement *ref2) const{
		if(ref1->score > ref2->score) return 1;
		if(ref1->score < ref2->score) return 0;
		if(ref1->size < ref2->size) return 1;
		if(ref1->size > ref2->size) return 0;
		if(!ref1->split && !ref2->split){
			interval* in1 = TA->get_state(ref1->state)->get_interval(ref1->symbol, ref1->time);
			interval* in2 = TA->get_state(ref2->state)->get_interval(ref2->symbol, ref2->time);
			return sum_of_interval_squares(in1->get_end(), ref1->time, in1->get_begin())
				   < sum_of_interval_squares(in2->get_end(), ref2->time, in2->get_begin());
		}
		return 0;
  }
};

struct reflist_compare{
  bool operator()(const reflist *reflist1, const reflist *reflist2) const{
        refinement* ref1 = *reflist1->rbegin();
        refinement* ref2 = *reflist2->rbegin();
        if(ref1->score < ref2->score) return 0;
        if(ref1->score > ref2->score) return 1;
        if(ref1->size > ref2->size) return 0;
        if(ref1->size < ref2->size) return 1;
		return 0;
  }
};

struct timed_tail_compare{
  bool operator()(timed_tail* tail1, timed_tail* tail2) const{
		return tail1->get_time_value() < tail2->get_time_value();
  }
};

bool run_polynomial_search(timed_input *, int, int);

#endif /* _POLYNOMIAL_SEARCHER_H_ */
