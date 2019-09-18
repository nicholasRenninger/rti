#ifndef TIMED_AUTOMATON_H_
#define TIMED_AUTOMATON_H_

using namespace std;

class timed_automaton;
class timed_state;
class interval;
class timed_tail;

#include <string.h>
#include <list>
#include <map>
#include <vector>
#include "globals.h"
#include "timed_data.h"
#include "score.h"
#include "timed_structures.h"

struct less_tail{
	bool operator()(const timed_tail* t1, const timed_tail* t2) const{
		if(t1->get_length() < t2->get_length()) return 1;
		if(t1->get_length() > t2->get_length()) return 0;
		return strcmp(t1->get_char_symbols(), t2->get_char_symbols()) < 0;
	}
};

typedef vector<timed_state*> state_list;
typedef pair<tail_list*, tail_list*> list_pair;
typedef map<const timed_tail*, list_pair, less_tail> tail_map;
typedef pair<const timed_tail*, list_pair> map_pair;

class interval{
private:
	double begin; 		// inclusive
	double end;			// inclusive
	tail_list* pos_tails;		// positive tails
	tail_list* neg_tails;	// negative tails
	timed_state* to;		// target state

	tail_map* map;
	
	friend class interval_list;
	friend class interval_node;

public:
	/* constructor */
	interval(double b, double e);
	~interval();
	
	/* get methods */
	inline timed_state* get_target() const{
		return to;
	};
	
	inline double get_begin() const{
		return begin;
	};
	
	inline double get_end() const{
		return end;
	};
	
	inline void switch_map(interval* in){
		tail_map* m = in->map;
		in->map = map;
		map = m;
	};
	
	inline tail_map* get_map() const{
		return map;
	};
	
	inline void add_pos_tail(timed_tail* tail){
		if(to != 0) pos_tails->add_tail(tail);
		else add_pos_tail_to_map(tail);
	};
	
	inline void add_pos_tail_to_map(timed_tail* tail){
		tail_map::iterator it = map->find(tail);
		if(it == map->end())
			it = map->insert(map_pair(tail, list_pair(new_tail_list(), new_tail_list()))).first;

		add_timed_inconsistencies((*it).second.second, (*it).second.first, tail);
		add_timed_consistencies((*it).second.second, (*it).second.first, tail);
		(*it).second.first->add_tail(tail);

/*  if((*it).second.second->get_size() > 0){
			INCONSISTENT_TAILS += 1;
			if((*it).second.first->get_size() == 1)
				INCONSISTENT_TAILS += (*it).second.second->get_size();
		}
		if((*it).second.first->get_size() == 2) CONSISTENT_TAILS += 2;
		else if((*it).second.first->get_size() > 2) CONSISTENT_TAILS += 1;*/
	};
	
	inline void del_pos_tail(timed_tail* tail){
		if(to != 0) pos_tails->del_tail(tail);
		else del_pos_tail_from_map(tail);
	};
	
	inline void del_pos_tail_from_map(timed_tail* tail){
		tail_map::iterator it = map->find(tail);
		assert(it !=map->end());

/*	if((*it).second.second->get_size() > 0){
			INCONSISTENT_TAILS -= 1;
			if((*it).second.first->get_size() == 1)
				INCONSISTENT_TAILS -= (*it).second.second->get_size();
		}
		if((*it).second.first->get_size() == 2) CONSISTENT_TAILS -= 2;
		else if((*it).second.first->get_size() > 2) CONSISTENT_TAILS -= 1;*/

		assert((*it).second.first->contains(tail));
		(*it).second.first->del_tail(tail);
		del_timed_inconsistencies((*it).second.second, (*it).second.first, tail);
		del_timed_consistencies((*it).second.second, (*it).second.first, tail);
		
		if((*it).second.first->get_size() == 0 && (*it).second.second->get_size() == 0){
			delete_tail_list((*it).second.first);
			delete_tail_list((*it).second.second);
			map->erase(it);
		}
	};

	inline void add_neg_tail(timed_tail* tail){
		if(to != 0) neg_tails->add_tail(tail);
		else add_neg_tail_to_map(tail);
	};
	
	inline void add_neg_tail_to_map(timed_tail* tail){
		tail_map::iterator it = map->find(tail);
		if(it == map->end())
			it = map->insert(map_pair(tail, list_pair(new_tail_list(), new_tail_list()))).first;

		add_timed_inconsistencies((*it).second.first, (*it).second.second, tail);
		add_timed_consistencies((*it).second.first, (*it).second.second, tail);
		(*it).second.second->add_tail(tail);

/*  if((*it).second.first->get_size() > 0){
			INCONSISTENT_TAILS += 1;
			if((*it).second.second->get_size() == 1)
				INCONSISTENT_TAILS += (*it).second.first->get_size();
		}
		if((*it).second.second->get_size() == 2) CONSISTENT_TAILS += 2;
		else if((*it).second.second->get_size() > 2) CONSISTENT_TAILS += 1;*/
	};
	
	inline void del_neg_tail(timed_tail* tail){
		if(to != 0) neg_tails->del_tail(tail);
		else del_neg_tail_from_map(tail);
	};
	
	inline void del_neg_tail_from_map(timed_tail* tail){
		tail_map::iterator it = map->find(tail);
		assert(it !=map->end());

/*
		if((*it).second.first->get_size() > 0){
			INCONSISTENT_TAILS -= 1;
			if((*it).second.second->get_size() == 1)
				INCONSISTENT_TAILS -= (*it).second.first->get_size();
		}
		if((*it).second.second->get_size() == 2) CONSISTENT_TAILS -= 2;
		else if((*it).second.second->get_size() > 2) CONSISTENT_TAILS -= 1;*/

		assert((*it).second.second->contains(tail));
		(*it).second.second->del_tail(tail);
		
		del_timed_inconsistencies((*it).second.first, (*it).second.second, tail);
		del_timed_consistencies((*it).second.first, (*it).second.second, tail);

		if((*it).second.first->get_size() == 0 && (*it).second.second->get_size() == 0){
			delete_tail_list((*it).second.first);
			delete_tail_list((*it).second.second);
			map->erase(it);
		}
	};

	void add_pos_tail_recursive(timed_tail* tail);
	void add_neg_tail_recursive(timed_tail* tail);

	void del_pos_tail_recursive(timed_tail* tail);
	void del_neg_tail_recursive(timed_tail* tail);

	inline tail_list* get_pos_tails() const{
		return pos_tails;
	};
	
	inline tail_list* get_neg_tails() const{
		return neg_tails;
	};
	
	inline bool is_empty() const{
		return pos_tails->get_size() == 0 && neg_tails->get_size() == 0 && map->empty();
	};
	
	void point(timed_state* target);
	void undo_point(timed_state* target);
};

class timed_automaton{
private:
	state_list states;
	timed_state* root;
	timed_input* input;

	int get_number(timed_state*);
  
public:
    timed_automaton();
    timed_automaton(timed_input* in);
    timed_automaton(istream& str);
	~timed_automaton();
	
	const string to_str_short() const;
	const string to_str_long() const;

	void print();
	ostream& print(ostream&);
	
	inline timed_state* get_root() const{
		return root;
	};
	
	inline const state_list& get_states() const{
		return states;
	};
	
	inline void add_state(timed_state* s){
		states.push_back(s);
	};

	inline void del_state(timed_state* s){
		state_list::iterator it = states.end();
		while(it != states.begin()){
			--it;
			if((*it) == s){
				states.erase(it);
				break;
			}
		}
	};
	
	inline bool contains_state(timed_state* s){
		state_list::iterator it = states.begin();
		while(it != states.end()){
			if(*it == s) return true;
			++it;
		}
		return false;
	};
	
	inline timed_state* get_state(int number){
		if(number <= states.size())
			return states[number - 1];
		return 0;
	};
	
	inline int num_states(){
		return states.size();
	};
};

class timed_state{
private:
	interval_list** targets;
  
	int final_pos;
	int final_neg;
	
	bool final;
  
	friend class timed_automaton;
	friend class interval_list;
  
public:
    timed_state();
	~timed_state();

	const string to_str_short() const;
	const string to_str_long() const;

    inline timed_state* get_target(const int symbol, const double time) const{
    	return targets[symbol]->get_interval(time)->get_target();
    };
    
    inline const interval_list* get_intervals(const int symbol) const{
    	return targets[symbol];
    };
    
    inline interval* get_interval(const int symbol, const double time) const{
    	return targets[symbol]->get_interval(time);
    };

    inline void add_final_pos(const int i){
    	CONSISTENT_FINALS += 1;
    	final_pos += i;
    	if(final_neg != 0) CONSISTENT = false;
    };
    
    inline void add_final_neg(const int i){
    	CONSISTENT_FINALS += 1;
    	final_neg += i;
    	if(final_pos != 0) CONSISTENT = false;
    };

    inline void del_final_pos(const int i){
    	final_pos -= i;
    	CONSISTENT_FINALS -= 1;
    };

	inline void del_final_neg(const int i){
		final_neg -= i;
    	CONSISTENT_FINALS -= 1;
	};
	
	inline void add_pos_tail_recursive(timed_tail* tail) const{
		get_interval(tail->get_symbol(), tail->get_time_value())->add_pos_tail_recursive(tail);
	};
	
	inline void add_neg_tail_recursive(timed_tail* tail) const{
		get_interval(tail->get_symbol(), tail->get_time_value())->add_neg_tail_recursive(tail);
	};
	
	inline void del_pos_tail_recursive(timed_tail* tail) const{
		get_interval(tail->get_symbol(), tail->get_time_value())->del_pos_tail_recursive(tail);
	};
	
	inline void del_neg_tail_recursive(timed_tail* tail) const{
		get_interval(tail->get_symbol(), tail->get_time_value())->del_neg_tail_recursive(tail);
	};

    inline  void point(int symbol, double time, timed_state* target){
    	get_interval(symbol, time)->point(target);
    };
    
    inline void undo_point(int symbol, double time, timed_state* target){
    	get_interval(symbol, time)->undo_point(target);
    };
    
    inline void set_final(bool b){
    	final = b;
    };
    
    inline bool is_final(){
    	return final;
    };
    
    void split(int symbol, double time);
    void undo_split(int symbol, double time);
};

#endif /* TIMED_AUTOMATON_H_*/
