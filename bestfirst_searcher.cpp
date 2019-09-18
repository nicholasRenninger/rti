/*
 *  delay_searcher.cpp
 *  IICT
 *
 *  Created by Sicco Verwer on 3/28/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "bestfirst_searcher.h"

using namespace std;

int best_result = -1;
priority_queue<reflist*, vector<reflist*>, reflist_compare> Q;
reflist* current_refinements;
int BEST_SCORE = -1;
int NODES = 0;

refinement::refinement(int s, interval* i, int t, int sy, double ti, bool sp){
	state = s;
	in = i;
	target = t;
	symbol = sy;
	time = ti;
	score = 0;
	size = 0;
	split = sp;
	ref_count = 0;
}

void refinement::refine(){
    CONSISTENT = true;
    if(!split){
      if(target != 0){
        interval_node* inter = TA->get_state(state)->get_intervals(symbol)->get_node(time);
        inter->get_interval()->point(TA->get_state(target));
        if(inter->next() != 0 && inter->next()->get_interval()->get_target() == TA->get_state(target))
          --SIZE;
        if(inter->prev() != 0 && inter->prev()->get_interval()->get_target() == TA->get_state(target))
          --SIZE;
      } else {
        interval* inter = TA->get_state(state)->get_interval(symbol, time);
        timed_state* new_state = new timed_state();
        TA->add_state(new_state);
        inter->point(new_state);
        SIZE += MAX_SYMBOL;
      }
    } else {
      TA->get_state(state)->split(symbol, time);
      ++SIZE;
    }
}

void refinement::undo_refine(){
    if(!split){
      if(target != 0){
        interval_node* inter = TA->get_state(state)->get_intervals(symbol)->get_node(time);
        inter->get_interval()->undo_point(TA->get_state(target));
        if(inter->next() != 0 && inter->next()->get_interval()->get_target() == TA->get_state(target))
          ++SIZE;
        if(inter->prev() != 0 && inter->prev()->get_interval()->get_target() == TA->get_state(target))
          ++SIZE;
      } else {
        interval* inter = TA->get_state(state)->get_interval(symbol, time);
        timed_state* new_state = inter->get_target();
        inter->undo_point(new_state);
        TA->del_state(new_state);
        SIZE -= MAX_SYMBOL;
        delete new_state;
      }
    } else {
      TA->get_state(state)->undo_split(symbol, time);
      --SIZE;
    }
    CONSISTENT = true;
}

double weighted_score(refset* refs){
	return  (*refs->begin())->get_score();
	double result = (*refs->begin())->get_score();
	double multiplier = 1.0;
	refset::iterator it = refs->begin();
	//while(it != refs->end()){
		double diff = (*it)->get_score();
		++it;
		if(it != refs->end())
			diff = diff - (*it)->get_score();
		result = result + multiplier * diff;
		multiplier = multiplier / 2;
	//}
	return result;
}

refset *get_best_refinements()
{
	refset *result = 0;
	
	for(int state_number = 1; state_number <= TA->num_states(); ++state_number){
		timed_state* state = TA->get_state(state_number);
		
		for(int symbol = 0; symbol < MAX_SYMBOL; ++symbol){
			const interval_list* it = state->get_intervals(symbol);
			const interval_node* in = it->get_head();

			while(in != 0){
				if(in->get_interval()->get_target() == 0 && !in->get_interval()->is_empty()){
					interval* inter = in->get_interval();
					
					refset *refinements = new refset();
					refinement* ref;
					
					timed_state* prev_state = 0;
					timed_state* next_state = 0;
					if(in->prev() != 0) prev_state = in->prev()->get_interval()->get_target();					
					if(in->next() != 0) next_state = in->next()->get_interval()->get_target();					

					for(int target_number = 1; target_number <= TA->num_states(); ++target_number){
						timed_state* target = TA->get_state(target_number);
						if(target == prev_state || target == next_state)
							continue;
						
						ref = new refinement(state_number, inter, target_number, symbol, inter->get_end(), false);
						CONSISTENT = true;
						ref->refine();
						if(!CONSISTENT){
							ref->undo_refine();
							delete ref;
						} else {
							ref->set_score();
							ref->undo_refine();
							refinements->insert(ref);
						}
					}

					ref = new refinement(state_number, inter, 0, symbol, inter->get_end(), false);
					CONSISTENT = true;
					ref->refine();
					if(!CONSISTENT){
						ref->undo_refine();
						delete ref;
					} else {
						ref->set_score();
						ref->undo_refine();
						refinements->insert(ref);
					}
					
          sorted_tails tails;
          for(tail_map::const_iterator it = inter->get_map()->begin(); it != inter->get_map()->end(); ++it){
            const tail_node* tn = (*it).second.first->get_head();
            while(tn != 0){
              tails.insert(tn->get_tail());
              tn = tn->next();
            }
            tn = (*it).second.second->get_head();
            while(tn != 0){
              tails.insert(tn->get_tail());
              tn = tn->next();
            }
          }
          if(!tails.empty()){
              interval* new_inter = new interval(inter->get_begin(), inter->get_end());
              refinement* best_split = 0;
            double time = (*tails.begin())->get_time_value();
            for(sorted_tails::iterator tail_it = tails.begin(); tail_it != tails.end(); ++tail_it){
              timed_tail* tail = (*tail_it);
              if(time < tail->get_time_value()){
                ref = new refinement(state_number, inter, 0, symbol, time, true);
                ref->set_score();
                //refinements->insert(ref);

                  if(best_split == 0 || less_refinement(ref, best_split))
                  best_split = ref;
                else
                  delete ref;

                time = tail->get_time_value();
              }
              if(tail->is_positive()){
                inter->del_pos_tail_from_map(tail);
                new_inter->add_pos_tail_to_map(tail);
              } else {
                inter->del_neg_tail_from_map(tail);
                new_inter->add_neg_tail_to_map(tail);
              }
            }
            inter->switch_map(new_inter);

            if(best_split != 0)
              refinements->insert(best_split);

            delete new_inter;
          }

					if(refinements->empty()){
						if(result != 0){
							for(refset::iterator it = result->begin(); it != result->end(); ++it)
								delete *it;
							delete result;
						}
						return refinements;
					}

					if(result == 0){
						result = refinements;
					} else {
/*
 * 						cout << "\nrefinements:\n";
						cout << "weighted score: " << weighted_score(refinements) << "\n";
						int i = 0;
						bool split = false;
						for(refset::iterator it = refinements->begin(); it != refinements->end(); ++it){
							if(split && (*it)->time != NEVER)
								continue;
							
							cout << i++ << ": ";
							(*it)->print();
							if((*it)->time != NEVER)
								split = true;
						}
						cout << endl;
						
						cout << "\nresults:\n";
						cout << "weighted score: " << weighted_score(result) << "\n";
						i = 0;
						split = false;
						for(refset::iterator it = result->begin(); it != result->end(); ++it){
							if(split && (*it)->time != NEVER)
								continue;
							
							cout << i++ << ": ";
							(*it)->print();
							if((*it)->time != NEVER)
								split = true;
						}
						cout << endl;
						cout << "change? 1=yes, 0=no" << endl;
						int number;
						cin >> number;

					 	if(number == 1){
							for(refset::iterator it = result->begin(); it != result->end(); ++it)
								delete *it;
							delete result;
							result = refinements;
						} else {
							for(refset::iterator it = refinements->begin(); it != refinements->end(); ++it)
								delete *it;
							delete refinements;
					 	}
					}
	*/
						if((*result->begin())->get_score() < (*refinements->begin())->get_score()){
							for(refset::iterator it = result->begin(); it != result->end(); ++it)
								delete *it;
							delete result;
							result = refinements;
						} else {
							for(refset::iterator it = refinements->begin(); it != refinements->end(); ++it)
								delete *it;
							delete refinements;
						}
					}
				}
				in = in->next();
			}
		}
	}
	return result;
}

refset *get_best_polynomial_refinements()
{
	refset *result = 0;
	
	for(int state_number = 1; state_number <= TA->num_states(); ++state_number){
		timed_state* state = TA->get_state(state_number);
		
		for(int symbol = 0; symbol < MAX_SYMBOL; ++symbol){
			const interval_list* it = state->get_intervals(symbol);
			const interval_node* in = it->get_head();

			while(in != 0){
				if(in->get_interval()->get_target() == 0 && !in->get_interval()->is_empty()){
					interval* inter = in->get_interval();
					
					refset *refinements = new refset();
					refinement* ref;
					
					for(int target_number = 1; target_number <= TA->num_states(); ++target_number){
						ref = new refinement(state_number, inter, target_number, symbol, inter->get_end(), false);
						CONSISTENT = true;
						ref->refine();
						if(!CONSISTENT){
							ref->undo_refine();
							delete ref;
						} else {
							ref->set_score();
							ref->undo_refine();
							refinements->insert(ref);
						}
					}

					ref = new refinement(state_number, inter, 0, symbol, inter->get_end(), false);
					CONSISTENT = true;
					ref->refine();
					if(!CONSISTENT){
						ref->undo_refine();
						delete ref;
					} else {
						ref->set_score();
						ref->undo_refine();
						refinements->insert(ref);
					}
					
					sorted_tails tails;
					set<double> split_points;
					for(tail_map::const_iterator it = inter->get_map()->begin(); it != inter->get_map()->end(); ++it){
						const tail_node* tn = (*it).second.first->get_head();
						const tail_node* tn2 = (*it).second.second->get_head();
						while(tn != 0 || tn2 != 0){
							if(tn == 0){
								tails.insert(tn2->get_tail());
								tn2 = tn2->next();
							} else if(tn2 == 0) {
								tails.insert(tn->get_tail());
								tn = tn->next();
							} else {
								double time1 = tn->get_tail()->get_time_value();
								double time2 = tn2->get_tail()->get_time_value();
								if(time1 < time2){
									tails.insert(tn->get_tail());
									tn = tn->next();
									if(tn == 0 || time2 < tn->get_tail()->get_time_value())
										split_points.insert(time1);
								} else if(time2 < time1){
									tails.insert(tn2->get_tail());
									tn2 = tn2->next();
									if(tn2 == 0 || time1 < tn2->get_tail()->get_time_value())
										split_points.insert(time2);
								} else {
									if(tn->next() != 0 || tn2->next() != 0)
										split_points.insert(time1);
									tails.insert(tn->get_tail());
									tn = tn->next();
									tails.insert(tn2->get_tail());
									tn2 = tn2->next();
								}
							}
						} 
					}
					if(!tails.empty() && !split_points.empty()){
	  					interval* new_inter = new interval(inter->get_begin(), inter->get_end());
	  					//refset best_splits;
  						set<double>::iterator time_it = split_points.begin();
						double time = (*time_it);
						for(sorted_tails::iterator tail_it = tails.begin(); tail_it != tails.end(); ++tail_it){
							timed_tail* tail = (*tail_it);
							if(time < tail->get_time_value()){
								ref = new refinement(state_number, inter, 0, symbol, time, true);
								ref->set_score();
								refinements->insert(ref);
								
/*								if(best_splits.empty())
									best_splits.insert(ref);
								else if(less_refinement(ref, (*best_splits.begin()))){
									for(refset::iterator it = best_splits.begin(); it != best_splits.end(); ++it)
										delete *it;
									best_splits.clear();
									best_splits.insert(ref);
								//}else if(!less_refinement((*best_splits.begin()), ref)){
									//best_splits.insert(ref);
								} else {
									delete ref;
								}*/
								++time_it;
								if(time_it == split_points.end())
									time = 11.0; //(MAX_TIME_VALUE)
								else
									time = *time_it;
							}
							if(tail->is_positive()){
								inter->del_pos_tail_from_map(tail);
								new_inter->add_pos_tail_to_map(tail);
							} else {
								inter->del_neg_tail_from_map(tail);
								new_inter->add_neg_tail_to_map(tail);
							}
						}
						inter->switch_map(new_inter);

						/*if(!best_splits.empty()){
							refinements->insert(best_splits.begin(), best_splits.end());
						}*/

						delete new_inter;
					}
	
					if(refinements->empty()){
						if(result != 0){
							for(refset::iterator it = result->begin(); it != result->end(); ++it)
								delete *it;
							delete result;
						}
						return refinements;
					}

					if(result == 0){
						result = refinements;
					} else {
/*
 						cout << "\nrefinements:\n";
						cout << "weighted score: " << weighted_score(refinements) << "\n";
						int i = 0;
						for(refset::iterator it = refinements->begin(); it != refinements->end(); ++it){
							cout << i++ << ": ";
							(*it)->print();
						}
						cout << endl;
						
						cout << "\nresults:\n";
						cout << "weighted score: " << weighted_score(result) << "\n";
						i = 0;
						for(refset::iterator it = result->begin(); it != result->end(); ++it){
							cout << i++ << ": ";
							(*it)->print();
						}
						cout << endl;
						cout << "change? 1=yes, 0=no" << endl;
						int number;
						cin >> number;

					 	if(number == 1){
							for(refset::iterator it = result->begin(); it != result->end(); ++it)
								delete *it;
							delete result;
							result = refinements;
						} else {
							for(refset::iterator it = refinements->begin(); it != refinements->end(); ++it)
								delete *it;
							delete refinements;
					 	}
*/						
						if(weighted_score(result) < weighted_score(refinements)){
							for(refset::iterator it = result->begin(); it != result->end(); ++it)
								delete *it;
							delete result;
							result = refinements;
						} else {
							for(refset::iterator it = refinements->begin(); it != refinements->end(); ++it)
								delete *it;
							delete refinements;
						}
					}
				}
				in = in->next();
			}
		}
	}
	return result;
}


int backtrack()
{
	//NODES++;
	
	SPLIT_CHANCE = (double)get_num_split_transitions() / (double)(TA->get_states().size() * MAX_SYMBOL);

 	//cout << "SPLIT_CHANCE: " << SPLIT_CHANCE << "\n";
	if(get_num_transitions() > 2 * best_result && best_result != -1)
		return get_num_transitions();
	
	//cerr << ".";
	//cout << "begin backtrack:\n" << TA->to_str_long() << "\n\n" << endl;
	refset* refinements = get_best_refinements();
	//cout << "after refinements:\n" << TA->to_str_long() << "\n\n" << endl;
	
	
	if(refinements == 0){
		if(best_result > get_num_transitions() || best_result == -1){
			cout << "SOLUTION:\n";
			TA->print();//cerr << TA->to_str_long();
			//cout << "nodes visited: " << NODES << "\n";
			cout << /*"END SOLUTION*/ "\n";
			cout.flush();
			//TA->print();
			best_result = get_num_transitions();
		}
		return get_num_transitions();
	}

	if(refinements->size() == 0){
		delete refinements;
		return 0;
	}
	

// 	cout << TA->to_str_long();
//	cout << "CONSISTENT FINALS: " << CONSISTENT_FINALS << " TIMED_INCONSISTENCY: " << TIMED_INCONSISTENCY << " TIMED_CONSISTENCY: " << TIMED_CONSISTENCY << "\n";
//	cout << "CONSISTENT TAILS: " << CONSISTENT_TAILS << " INCONSISTENT_TAILS: " << INCONSISTENT_TAILS << "\n";
/*	cout << "SPLIT_CHANCE: " << SPLIT_CHANCE << "\n";*/

/* 	cerr << "\nOPTIONS:\n";
	int i = 0;
	for(refset::iterator it = refinements->begin(); it != refinements->end(); ++it){
		cerr << i++ << ": ";
		(*it)->print();
	}
	cerr << endl;
	cin.get();
	cout << "choose a number." << endl;
	int number;
	cin >> number;

	i = 0;*/
	int result = 0;
	for(refset::iterator it = refinements->begin(); it != refinements->end(); ++it){
		//if(i++ != number)
			//continue;
		
		if(result == 0){
			//cerr << "\ttrying: \t";
			//(*it)->print();
			(*it)->refine();
			//cout << "after refining:\n" << TA->to_str_long() << "\n\n" << endl;
			result = backtrack();
			//cout << "before undo:\n" << TA->to_str_long() << "\n\n" << endl;
			(*it)->undo_refine();
			//cout << "after undo:\n" << TA->to_str_long() << "\n\n" << endl;
			//cin.get();
		}
		delete *it;
	}
	delete refinements;
	return result;
}

bool backtrack_range(double dev)
{
	NODES++;
	//cerr << "nodes: " << NODES << "  size: " << get_num_transitions() << endl;
	//TA->print();

	if(best_result != -1 && get_num_transitions() > best_result)
		return 0;

	cerr << "current size: " << get_num_transitions() << " best size: " << best_result << " #nodes: " << NODES << endl;
	
	SPLIT_CHANCE = (double)get_num_split_transitions() / (double)(TA->get_states().size() * MAX_SYMBOL);

 	//cout << "SPLIT_CHANCE: " << SPLIT_CHANCE << "\n";
	
	//cout << "begin backtrack:\n" << TA->to_str_long() << "\n\n" << endl;
	refset* refinements = get_best_polynomial_refinements();
	//cout << "after refinements:\n" << TA->to_str_long() << "\n\n" << endl;
	
	if(refinements == 0){
		cout << "SOLUTION:\n";
		TA->print();
		//cout << "nodes visited: " << NODES << "\n";
		//cout << "END SOLUTION \n";
		cout.flush();
		//TA->print();
		return 1;
	}

	if(refinements->size() == 0){
		delete refinements;
		return 0;
	}

 	cerr << TA->to_str_short();
//	cout << "CONSISTENT FINALS: " << CONSISTENT_FINALS << " TIMED_INCONSISTENCY: " << TIMED_INCONSISTENCY << " TIMED_CONSISTENCY: " << TIMED_CONSISTENCY << "\n";
//	cout << "CONSISTENT TAILS: " << CONSISTENT_TAILS << " INCONSISTENT_TAILS: " << INCONSISTENT_TAILS << "\n";
//	cout << "SPLIT_CHANCE: " << SPLIT_CHANCE << "\n";

 	/*cerr << "\nOPTIONS:\n";
	int i = 0;
	for(refset::iterator it = refinements->begin(); it != refinements->end(); ++it){
		cerr << i++ << ": ";
		(*it)->print();
	}
	cerr << i << " options." << endl;*/
	//cout << "choose a number." << endl;
	//int number;
	//cin >> number;

	//i = 0;
	double dev_score = (*refinements->begin())->score;
	dev_score = dev_score - dev_score * dev;
	int min_size = -1;
	refinement* best_ref = 0;
	for(refset::iterator it = refinements->begin(); it != refinements->end(); ++it){
		//i++;
		//if(i++ != number)
			//continue;
		//if((*it)->score > dev_score){
			//cerr << "trying " << i << " : ";
			//(*it)->print();
			(*it)->refine();
			int end_size = backtrack();
			//cerr << "end size: " << end_size << endl;
			//cout << "before undo:\n" << TA->to_str_long() << "\n\n" << endl;
			(*it)->undo_refine();
			//cout << "after undo:\n" << TA->to_str_long() << "\n\n" << endl;
			//cerr << end_size << endl;
			if(best_ref == 0){
				best_ref = *it;
				min_size = end_size;
			} else if(end_size != 0 && min_size > end_size){
				delete best_ref;
				best_ref = *it;
				min_size = end_size;
			} else {
				delete *it;
			}
			//cout << "after refining:\n" << TA->to_str_long() << "\n\n" << endl;
		//} else {
			//delete *it;
		//}
	}
	if(best_ref != 0){
		best_ref->refine();
		backtrack_range(dev);
		best_ref->undo_refine();
		delete best_ref;
	}
	
	delete refinements;
	return 0;
}

void add_refinements_to_q(refset* refinements){
	refset::iterator it = refinements->begin();
	while(it != refinements->end()){
		if((*it)->score == 0){
			++it;
			continue;
		}
		
		reflist::iterator itt = current_refinements->insert(current_refinements->end(), *it);
		for(reflist::iterator rit = current_refinements->begin(); rit != current_refinements->end(); ++rit){
			++(*rit)->ref_count;
		}
		
		Q.push(new reflist(*current_refinements));
		current_refinements->erase(itt);
		++it;
	}
	delete refinements;
}

void change_reflist(reflist *new_list){
	reflist::iterator old_it = current_refinements->begin();
	reflist::iterator new_it = new_list->begin();
	
	//cout << "current: " << endl;
	/*while(old_it != current_refinements->end()){
		(*old_it)->print();
		++old_it;
	}*/
	
	/*
	 * cout << "\nnew: " << endl;
	while(new_it != new_list->end()){
		(*new_it)->print();
		++new_it;
	}*/

	old_it = current_refinements->begin();
	new_it = new_list->begin();

	while(old_it != current_refinements->end() && new_it != new_list->end() && *old_it == *new_it){
		//cout << "equals: " << endl;
		//(*old_it)->print();
		//(*new_it)->print();
		--(*old_it)->ref_count;
		++old_it; ++new_it;
	}
	
	if(new_it == new_list->begin()){
		reflist::iterator it = current_refinements->end();
		if(it != current_refinements->begin()){
			do{
				--it;
				//cout << "undoing: ";
				//(*it)->print();
				--(*it)->ref_count;
				(*it)->undo_refine();
				if((*it)->ref_count == 0)
					delete (*it);
			} while(it != current_refinements->begin());
		}
	} else {
		--new_it;
		reflist::iterator it = current_refinements->end();
		--it;
		while(*it != *new_it){
			//cout << "undoing: ";
			//(*it)->print();
			(*it)->undo_refine();
			--it;
		}
		++new_it;
	}
	
	while(new_it != new_list->end()){
		refinement* new_ref = *new_it;
		assert(TA->num_states() >= new_ref->state);
		if(new_ref->target != 0)
			assert(TA->num_states() >= new_ref->target);
		
		//cout << "doing: ";
		//(*new_it)->print();
		(*new_it)->refine();
		++new_it;
	}
	
	delete current_refinements;
	current_refinements = new_list;	
	
/*	maybe try this some day...
 * 
 *  set<timed_state*> new_states;
    for(reflist::iterator it = new_list.begin();it != new_list.end(); ++it){
        refinement* ref = *it;
        if(ref->time == NEVER){
            if(ref->target != 0){
PROBLEM : intervals can be deleted
                interval* in = ref->state->get_interval(ref->symbol, ref->time);
                if(in->begin != ref->in->begin || in->end != ref->end){
                } else if(in->get_target() != ref->target){
                    if(in->get_target() != 0)
                    	in->undo_point(in->get_target());
                    in->point(ref->target);
                }
            } else {
                interval* in = ref->state->get_interval(ref->symbol, ref->time);
                if(in->begin != ref->in->begin || in->end != ref->end){
                } else if(in->get_target() != ref->target){
                    if(new_states.find(in->get_target()) != new_states.end()){
                        if(in->get_target() != 0)
                        	in->undo_point(in->get_target());
                        new_state = new timed_state();
                        TA->add_state(new_state);
                        in->point(new_state);
                        ref->target = new_state;
                    }
                }
            }
        } else {
            interval* in = ref->state->get_interval(ref->symbol, ref->time);
            if(in->begin != ref->in->begin || in->end != ref->end){
        }
    } */
};


void bestfirst(){
	add_refinements_to_q(get_best_polynomial_refinements());
	while(!Q.empty()){
		//cout << "-----\ntop of queue : score: " << (*Q.top()->rbegin())->score << " size: " << (*Q.top()->rbegin())->size << endl;
		//cout << "best result: " << best_result << " queue size: " << Q.size() << endl;
		change_reflist(Q.top());
		//cout << "refinements:\n";
		//for(reflist::iterator it = current_refinements->begin(); it != current_refinements->end(); ++it){
		//	(*it)->print();
		//}
		Q.pop();
	
		NODES++;
		
		if(get_num_transitions() >= best_result && best_result != -1){
			//cout << "too large..." << endl;
			continue;
		}

		refset* refinements = get_best_polynomial_refinements();
	
		if(refinements == 0){
			if(get_num_transitions() < best_result || best_result == -1){
				cout << "SOLUTION:\n";
				TA->print();
				//cerr << "nodes visited: " << NODES << "\n";
				cout << /*"END SOLUTION*/ "\n";
				cout.flush();
				//TA->print();
			
				best_result = get_num_transitions();
			} else {
				//cout << "no refinements left..." << endl;
			}
		} else {
			if(refinements->size() == 0){
				//cout << "only inconsistent refinements possible..." << endl;
			}
		for(refset::iterator it = refinements->begin(); it != refinements->end(); ++it){
					(*it)->refine();
					int backtrack_result = backtrack();
					//cerr << "current: " << get_num_transitions() << " score: " << (*it)->get_score() << " result: " << backtrack_result << endl;
					if(backtrack_result != 0 && (backtrack_result < 2 * best_result || best_result == -1))
						(*it)->score = backtrack_result;
					else
						(*it)->score = 0;
					(*it)->undo_refine();
			}
			add_refinements_to_q(refinements);
		}
	}
}

void run_bestfirst_search(timed_input *in){
	TA = new timed_automaton(in);
	current_refinements = new reflist();
	bestfirst();
	cerr << "total nodes visited: " << NODES << endl;
}

bool run_polynomial_search(timed_input *in)
{
	TA = new timed_automaton(in);
/*	cerr << "---------------------" << endl;
	cerr << "Beginning backtrack:" << endl;
	cout << TA->to_str_long();
	cout << "total positive = " << TOTAL_POSITIVE << " total negative = " << TOTAL_NEGATIVE << "\n";
	cout << "CONSISTENT FINALS: " << CONSISTENT_FINALS << " TIMED_INCONSISTENCY: " << TIMED_INCONSISTENCY << " TIMED_CONSISTENCY: " << TIMED_CONSISTENCY << "\n";
	cout << "CONSISTENT TAILS: " << CONSISTENT_TAILS << " INCONSISTENT_TAILS: " << INCONSISTENT_TAILS << "\n";
	cout.flush();
	cin.get();
*/
	backtrack_range(0.05);
	cerr << "total nodes visited: " << NODES << endl;
	return 1;
}

bool run_backtrack(timed_input *in)
{
	TA = new timed_automaton(in);
/*	cerr << "---------------------" << endl;
	cerr << "Beginning backtrack:" << endl;
	cout << TA->to_str_long();
	cout << "total positive = " << TOTAL_POSITIVE << " total negative = " << TOTAL_NEGATIVE << "\n";
	cout << "CONSISTENT FINALS: " << CONSISTENT_FINALS << " TIMED_INCONSISTENCY: " << TIMED_INCONSISTENCY << " TIMED_CONSISTENCY: " << TIMED_CONSISTENCY << "\n";
	cout << "CONSISTENT TAILS: " << CONSISTENT_TAILS << " INCONSISTENT_TAILS: " << INCONSISTENT_TAILS << "\n";
	cout.flush();
	cin.get();
*/
	backtrack();
	return 1;
}

int main(int argc, const char *argv[]){
	if(argc != 3){
		cerr << "Required input: datafile method" << endl;
		cerr << "where method is: p for polynomial search, g for greedy, and b for bestfirst search" << endl;
		return 0;
	}
	
	ifstream test_file(argv[1]);
	if(!test_file.is_open())
		return 0;
	
	timed_input *in = new timed_input(test_file);
	test_file.close();
	
	if(*argv[2] == 'p')
		run_polynomial_search(in);
	else if(*argv[2] == 'g')
		run_backtrack(in);
	else if(*argv[2] == 'b')
		run_bestfirst_search(in);
	else{
		cerr << "Required input: datafile method" << endl;
		cerr << "where method is: p for polynomial search, g for greedy, and b for bestfirst search" << endl;
		return 0;
	}

	return 1;
}

