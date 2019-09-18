
#include "timed_automaton.h"

interval::interval(double b, double e){
	begin = b;
	end = e;
	pos_tails = new_tail_list();
	neg_tails = new_tail_list();
	to = 0;
	map = new tail_map();
};

interval::~interval(){
	delete_tail_list(pos_tails);
	delete_tail_list(neg_tails);

	for(tail_map::iterator it = map->begin(); it != map->end(); ++it){
		delete_tail_list((*it).second.first);
		delete_tail_list((*it).second.second);
	}
	map->clear();
	delete map;
};
	

void interval::point(timed_state* target){
	to = target;
	assert(pos_tails->get_head() == 0 && neg_tails->get_head() == 0);
	
	for(tail_map::iterator it = map->begin(); it != map->end() /*&& CONSISTENT*/; ++it){
		del_identical_tail_score((*it).second.first, (*it).second.second);

	  	for(const tail_node* node = (*it).second.first->get_head(); node != 0 && (CONSISTENT || node->get_tail()->next_tail() == 0); node = node->next()){
			//(*it).second.first->del_tail(node->get_tail());
			if(node->get_tail()->next_tail() != 0) to->add_pos_tail_recursive(node->get_tail()->next_tail());
			else to->add_final_pos(1);
		}
		for(const tail_node* node = (*it).second.second->get_head(); node != 0 && (CONSISTENT || node->get_tail()->next_tail() == 0); node = node->next()){
			//(*it).second.second->del_tail(node->get_tail());
			if(node->get_tail()->next_tail() != 0) to->add_neg_tail_recursive(node->get_tail()->next_tail());
			else to->add_final_neg(1);
		}
	}
	
/*
  	for(const tail_node* node = pos_tails->get_head(); node != 0; node = node->next()){
		del_pos_tail_from_map(node->get_tail());
		if(node->get_tail()->next_tail() != 0) to->add_pos_tail_recursive(node->get_tail()->next_tail());
		else to->add_final_pos(1);
	}
	for(const tail_node* node = neg_tails->get_head(); node != 0; node = node->next()){
		del_neg_tail_from_map(node->get_tail());
		if(node->get_tail()->next_tail() != 0) to->add_neg_tail_recursive(node->get_tail()->next_tail());
		else to->add_final_neg(1);
	}
 */
};

void interval::undo_point(timed_state* target){
	assert(to == target);
	for(tail_map::iterator it = map->begin(); it != map->end(); ++it){
	  	for(const tail_node* node = (*it).second.first->get_head(); node != 0; node = node->next()){
			//(*it).second.first->del_tail(node->get_tail());
			if(node->get_tail()->next_tail() != 0) to->del_pos_tail_recursive(node->get_tail()->next_tail());
			else to->del_final_pos(1);
		}
		for(const tail_node* node = (*it).second.second->get_head(); node != 0; node = node->next()){
			//(*it).second.second->del_tail(node->get_tail());
			if(node->get_tail()->next_tail() != 0) to->del_neg_tail_recursive(node->get_tail()->next_tail());
			else to->del_final_neg(1);
		}
		add_identical_tail_score((*it).second.first, (*it).second.second);
	}
	for(const tail_node* node = pos_tails->get_head(); node != 0; node = node->next()){
		if(node->get_tail()->next_tail() != 0) del_pos_tail_recursive(node->get_tail()->next_tail());
		else to->del_final_pos(1);
	}
	for(const tail_node* node = neg_tails->get_head(); node != 0; node = node->next()){
		if(node->get_tail()->next_tail() != 0) del_neg_tail_recursive(node->get_tail()->next_tail());
		else to->del_final_neg(1);
		// ? del_neg_tail_recursive(node->get_tail());
	}
	for(const tail_node* node = pos_tails->get_head(); node != 0; node = pos_tails->get_head()){
		del_pos_tail(node->get_tail());
		add_pos_tail_to_map(node->get_tail());
	}
	for(const tail_node* node = neg_tails->get_head(); node != 0; node = neg_tails->get_head()){
		del_neg_tail(node->get_tail());
		add_neg_tail_to_map(node->get_tail());
	}
	to = 0;
};

void interval::add_pos_tail_recursive(timed_tail* tail){
	add_pos_tail(tail);
	if(to != 0){
		if(tail->next_tail() != 0) to->add_pos_tail_recursive(tail->next_tail());
		else to->add_final_pos(1);
	}
};

void interval::add_neg_tail_recursive(timed_tail* tail){
	add_neg_tail(tail);
	if(to != 0){
	 	if(tail->next_tail() != 0) to->add_neg_tail_recursive(tail->next_tail());
		else to->add_final_neg(1);
	}
};

void interval::del_pos_tail_recursive(timed_tail* tail){
	if(!tail->is_contained())
		return;
		
	if(to != 0){
		if(tail->next_tail() != 0) to->del_pos_tail_recursive(tail->next_tail());
		else to->del_final_pos(1);
	}
	del_pos_tail(tail);
};

void interval::del_neg_tail_recursive(timed_tail* tail){
	if(!tail->is_contained())
		return;

	if(get_target() != 0){
		if(tail->next_tail() != 0) get_target()->del_neg_tail_recursive(tail->next_tail());
		else get_target()->del_final_neg(1);
	}
	del_neg_tail(tail);
};

timed_automaton::timed_automaton(){
	root = new timed_state();
	states.push_back(root);
	input = 0;
};

timed_automaton::timed_automaton(timed_input* in){
	root = new timed_state();
	states.push_back(root);
	input = in;
	
	for(int i = 0; i < NUM_WORDS; ++i){
		timed_tail* tail = in->get_word(i)->get_first_tail();
		assert(tail != 0);
		if(tail->is_positive()) root->add_pos_tail_recursive(tail);
		else root->add_neg_tail_recursive(tail);
	}
};

timed_automaton::timed_automaton(istream& str){
	input = 0;
	int size, alph_size;
  str >> size >> alph_size;

  for(int i = 0; i < size; i++){
    states.push_back(new timed_state());
  }
	root = states[0];

  int number, to, symbol;
  bool pos;
	double time;
  for(int i = 0; i < size; i++){
    str >> number >> pos;
    timed_state* state = states[number];
    state->final = pos;

    for(int j = 0; j < alph_size; j++){
      str >> symbol;
      char t;
      str >> t; // read [
      while(t != ']'){
        str >> time;
        str >> to;
				if(time > 10) time = 10;
        if(time < 10) state->split(symbol, time);
        if(to != -1) state->point(symbol, time, states[to]);
        str >> t; // red , or ]
      }
    }
  }
};

timed_automaton::~timed_automaton(){
	for(state_list::iterator it = states.begin(); it != states.end(); ++it)
		delete *it;
};

int timed_automaton::get_number(timed_state* state){
		int i = 0;
		for(state_list::const_iterator it = states.begin(); it != states.end();
		++it){
			if(*it == state) return i;
			i++;
		}
		return -1;
};

void timed_automaton::print(){
		cout << states.size() << " " << MAX_SYMBOL << "\n";
		for(state_list::const_iterator it = states.begin(); it != states.end();
		++it){
		  cout << get_number(*it) << " ";
			if((*it)->final_pos != 0 || (*it)->final != 0) cout << "1 ";
			else cout << "0 ";
			
			for(int i = 0; i < MAX_SYMBOL; ++i){
				const interval_node* node = (*it)->get_intervals(i)->get_head();
				cout << i << " [ ";
				while(node != 0){
					if(node->next() == 0 ||
					get_number(node->next()->get_interval()->get_target()) !=
					get_number(node->get_interval()->get_target())){
						cout << node->get_interval()->get_end() + MIN_TIME_STEP
							 	 << " " << get_number(node->get_interval()->get_target());
						if(node->next() != 0)
							cout << " , ";
					}
					node = node->next();
				}
				cout << " ] ";
			}
			cout << "\n";
		}
		cout << endl;
};

ostream& timed_automaton::print(ostream& str){
		str << states.size() << " " << MAX_SYMBOL << "\n";
		for(state_list::const_iterator it = states.begin(); it != states.end(); ++it){
		  str << get_number(*it) << " ";
			if((*it)->final_pos != 0 || (*it)->final != 0) str << "1 ";
			else str << "0 ";
			
			for(int i = 0; i < MAX_SYMBOL; ++i){
				const interval_node* node = (*it)->get_intervals(i)->get_head();
				str << i << " [ ";
				while(node != 0){
					if(node->next() == 0 ||
					get_number(node->next()->get_interval()->get_target()) !=
					get_number(node->get_interval()->get_target())){
						str << node->get_interval()->get_end() + MIN_TIME_STEP
							 	 << " " << get_number(node->get_interval()->get_target());
						if(node->next() != 0)
							str << " , ";
					}
					node = node->next();
				}
				str << " ] ";
			}
			str << "\n";
		}
		str << endl;
		return str;
};

const string timed_automaton::to_str_short() const{
	ostringstream ostr;
	for(state_list::const_iterator it = states.begin(); it != states.end(); ++it)
		ostr << (*it)->to_str_short();
	return ostr.str();
};

const string timed_automaton::to_str_long() const{
	ostringstream ostr;
	for(state_list::const_iterator it = states.begin(); it != states.end(); ++it)
		ostr << (*it)->to_str_long();
	return ostr.str();
};

const string timed_state::to_str_short() const{
	ostringstream ostr;
	ostr << this << " pos:" << final_pos << " neg:" << final_neg << "\n";
	for(int i = 0; i < MAX_SYMBOL; ++i){
		const interval_node* node = targets[i]->get_head();
		bool together = false;
		while(node != 0){
			if(!together){
				ostr << "\t"  << i
			 	     << " [" << node->get_interval()->get_begin()
			    	 << ", ";
			}
			if(node->next() != 0 && node->next()->get_interval()->get_target() == node->get_interval()->get_target()){
				together = true;
			} else {
				 ostr << node->get_interval()->get_end()
			     	  << "]->" << node->get_interval()->get_target()
			     	  << "\n";
			     together = false;
			}			
			node = node->next();
		}
	}
	return ostr.str();
};

const string timed_state::to_str_long() const{
	ostringstream ostr;
	ostr << this << " pos:" << final_pos << " neg:" << final_neg << "\n";
	for(int i = 0; i < MAX_SYMBOL; ++i){
		const interval_node* node = targets[i]->get_head();
		while(node != 0){
			ostr << "\t"  << i
			     << " [" << node->get_interval()->get_begin()
			     << ", "  << node->get_interval()->get_end()
			     << "]->" << node->get_interval()->get_target()
			     << "\n";
			
			ostr << "\tpos: { ";
			const tail_node* tn = node->get_interval()->get_pos_tails()->get_head();
			while(tn != 0 && tn != node->get_interval()->get_pos_tails()->get_tail()){
				ostr << tn->get_tail()->get_time_value() << "-" << tn->get_tail()->get_char_symbols() << ", ";
				tn = tn->next();
			}
			if(node->get_interval()->get_pos_tails()->get_tail() != 0)
				ostr << node->get_interval()->get_pos_tails()->get_tail()->get_tail()->get_time_value() << "-"
				     << node->get_interval()->get_pos_tails()->get_tail()->get_tail()->get_char_symbols();
			ostr << " }\n";
			ostr << "\tneg: { ";
			tn = node->get_interval()->get_neg_tails()->get_head();
			while(tn != 0 && tn != node->get_interval()->get_neg_tails()->get_tail()){
				ostr << tn->get_tail()->get_time_value() << "-" << tn->get_tail()->get_char_symbols() << ", ";
				tn = tn->next();
			}
			if(node->get_interval()->get_neg_tails()->get_tail() != 0)
				ostr << node->get_interval()->get_neg_tails()->get_tail()->get_tail()->get_time_value() << "-"
				     << node->get_interval()->get_neg_tails()->get_tail()->get_tail()->get_char_symbols();
			ostr << " }\n";
			
			ostr << "\tmap: ";
			const tail_map* map = node->get_interval()->get_map();
			for(tail_map::const_iterator it = map->begin(); it != map->end(); ++it){
				ostr << "(" << (*it).first->get_char_symbols() << ",[" << (*it).second.first->get_size() << "," << (*it).second.second->get_size() << "])";
			}
			ostr << "\n";
			node = node->next();
		}
	}
	return ostr.str();
};


timed_state::timed_state(){
	final_pos = 0;
	final_neg = 0;
	final = false;
	targets = new interval_list*[MAX_SYMBOL];
	for(int i = 0; i < MAX_SYMBOL; ++i)
		targets[i] = new interval_list();
}

timed_state::~timed_state(){
	for(int i = 0; i < MAX_SYMBOL; ++i)
		delete targets[i];
	delete[] targets;
}

void timed_state::split(int symbol, double time){
	interval* old_in = targets[symbol]->get_interval(time);
	assert(old_in->get_target() == 0);
	interval* new_in = targets[symbol]->split(time);
	
	tail_map::iterator point = new_in->get_map()->begin();
	tail_map::iterator next, it;
	for(it = old_in->get_map()->begin(); it != old_in->get_map()->end(); it = next){
		next = it;
		++next;
		del_identical_tail_score((*it).second.first, (*it).second.second);
	  	const tail_node* next_node = 0;
	  	const tail_node* node = 0;
	  	for(node = (*it).second.first->get_head(); node != 0; node = next_node){
	  		next_node = node->next();
			if(node->get_tail()->get_time_value() <= time){
				if(point == new_in->get_map()->end() || (*point).first != (*it).first)
					point = new_in->get_map()->insert(point, map_pair((*it).first, list_pair(new_tail_list(), new_tail_list())));
				(*it).second.first->del_tail(node->get_tail());
				(*point).second.first->add_tail(node->get_tail());
			}
		}
	  	for(node = (*it).second.second->get_head(); node != 0; node = next_node){
	  		next_node = node->next();
			if(node->get_tail()->get_time_value() <= time){
				if(point == new_in->get_map()->end() || (*point).first != (*it).first)
					point = new_in->get_map()->insert(point, map_pair((*it).first, list_pair(new_tail_list(), new_tail_list())));
				(*it).second.second->del_tail(node->get_tail());
				(*point).second.second->add_tail(node->get_tail());
			}
		}
		if((*it).first == (*point).first)
			add_identical_tail_score((*point).second.first, (*point).second.second);
		add_identical_tail_score((*it).second.first, (*it).second.second);

		if((*it).second.first->get_size() == 0 && (*it).second.second->get_size() == 0){
			delete_tail_list((*it).second.first);
			delete_tail_list((*it).second.second);
			old_in->get_map()->erase(it);
		}
	}

/*
 * 	node = old_in->get_neg_tails()->get_head();
	if(node != 0){
		const tail_node* next = node->next();
		while(node != 0){
			if(node->get_tail()->get_time_value() <= time){
				old_in->del_neg_tail_from_map(node->get_tail());
				new_in->add_neg_tail_to_map(node->get_tail());
			}
			node = next;
			if(node != 0) next = node->next();
		}
	}
*/
};

void timed_state::undo_split(int symbol, double time){
	const interval_node* node =  targets[symbol]->get_node(time);
	interval* new_in = node->get_interval();
	if(node->next() == 0){
	  cerr << new_in->get_begin() << ", " << new_in->get_end() << endl;
	}
	assert(node->next() != 0);
	interval* old_in = node->next()->get_interval();
	assert(new_in->get_target() == 0);
	assert(old_in->get_target() == 0);
	
	tail_map::iterator point = old_in->get_map()->begin();
	tail_map::iterator next, it;
	for(it = new_in->get_map()->begin(); it != new_in->get_map()->end(); it = next){
		next = it;
		++next;
		
		point = old_in->get_map()->insert(point, *it);
		if((*point).second.first != (*it).second.first){
			del_identical_tail_score((*point).second.first, (*point).second.second);
			del_identical_tail_score((*it).second.first, (*it).second.second);
			(*point).second.first->merge((*it).second.first);
			(*point).second.second->merge((*it).second.second);
			delete_tail_list((*it).second.first);
			delete_tail_list((*it).second.second);
			add_identical_tail_score((*point).second.first, (*point).second.second);
		}
		new_in->get_map()->erase(it);
	}
	targets[symbol]->undo_split(time);
};
