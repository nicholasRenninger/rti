
#include "score.h"
#include "timed_data.h"
#include "timed_automaton.h"
#include "timed_structures.h"

int NUM_WORDS = 0;
int TOTAL_POSITIVE = 0;
int TOTAL_NEGATIVE = 0;
int CONSISTENT_FINALS = 0;
int INCONSISTENT_FINALS = 0;
double CONSISTENT_TAILS = 0;
double INCONSISTENT_TAILS = 0;
double TIMED_CONSISTENCY = 0;
double TIMED_INCONSISTENCY = 0;
bool CONSISTENT = true;
double SPLIT_CHANCE = 0.0;

void add_timed_inconsistencies(tail_list* list, tail_list* list2, timed_tail* tail){
	if(list->get_size() > 0){
		TIMED_INCONSISTENCY += 1;
		if(list2->get_size() == 0)
			TIMED_INCONSISTENCY += list->get_size();
	}
}

void add_timed_consistencies(tail_list* list, tail_list* list2, timed_tail* tail){
	if(list2->get_size() == 1)
		TIMED_CONSISTENCY += 1;
	if(list2->get_size() > 0)
		TIMED_CONSISTENCY += 1;
}

void del_timed_inconsistencies(tail_list* list, tail_list* list2, timed_tail* tail){
	if(list->get_size() > 0){
		TIMED_INCONSISTENCY -= 1;
		if(list2->get_size() == 0)
			TIMED_INCONSISTENCY -= list->get_size();
	}
}

void del_timed_consistencies(tail_list* list, tail_list* list2, timed_tail* tail){
	if(list2->get_size() == 1)
		TIMED_CONSISTENCY -= 1;
	if(list2->get_size() > 0)
		TIMED_INCONSISTENCY -= 1;
}

void add_identical_tail_score(tail_list* list, tail_list* list2){
 	if(list->get_size() > 0 && list2->get_size() > 0){
		INCONSISTENT_TAILS += list->get_size();
		INCONSISTENT_TAILS += list2->get_size();
	}
	if(list->get_size() > 1)  CONSISTENT_TAILS += list->get_size();
	if(list2->get_size() > 1) CONSISTENT_TAILS += list2->get_size();
}

void del_identical_tail_score(tail_list* list, tail_list* list2){
	if(list->get_size() > 0 && list2->get_size() > 0){
		INCONSISTENT_TAILS -= list->get_size();
		INCONSISTENT_TAILS -= list2->get_size();
	}
	if(list->get_size() > 1) CONSISTENT_TAILS -= list->get_size();
	if(list2->get_size() > 1) CONSISTENT_TAILS -= list2->get_size();
}

