
#include "timed_data.h"
#include <math.h>

double MIN_DISTANCE = 0.0;
double MAX_DISTANCE = 10000.0;

/* 1.0 = overlapping, 0.0 = far apart */
double time_distance(const timed_tail* t1, const timed_tail* t2){
	assert(t1->get_length() == t2->get_length());
	//if(t1->get_length() <= 1)
		//return 1.0;
	
	double distance = 0.0;
	//double result = 0.0;
	double seperation_chance = 0.0;
	//const timed_tail* first = t1->next_tail();
	//const timed_tail* second = t2->next_tail();
	const timed_tail* first = t1;
	const timed_tail* second = t2;

	while(first != 0){
		double value = first->get_time_value() - second->get_time_value();
		if(value < 0) value = -value;
		
		assert(value <= MAX_DISTANCE);
		assert(value >= MIN_DISTANCE);
		
		seperation_chance += (1.0 - seperation_chance) * value / (MAX_DISTANCE - MIN_DISTANCE);

		if(value > distance) distance = value;
		
/*		
		if(result < value){
			if(value > MAX_DISTANCE)
				return 0.0;
			result = value;
		}
*/		
		//multiplier = multiplier * DISTANCE_MULTIPLIER;
		
		first = first->next_tail();
		second = second->next_tail();
	
	}

	//return 1.0 - (distance / (MAX_DISTANCE - MIN_DISTANCE));
	assert(seperation_chance >= 0.0);
	assert(seperation_chance <= 1.0);
	return 1.0 - seperation_chance;
/*	if(result < MIN_DISTANCE)
		return 1.0;
	//if(result > MAX_DISTANCE * t1->get_length())
	//	return 0.0;
	//assert(1.0 - (result - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE) >= 0);
	return (1.0 - (result - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE)); */
}

timed_input::timed_input(istream &str){
	str >> NUM_WORDS >> MAX_SYMBOL;
	alphabet = new char[MAX_SYMBOL];
	words = new timed_word*[NUM_WORDS];
	int current_alphabet_size = 0;
	for(int line = 0; line < NUM_WORDS; ++line)
	{
	 	timed_word* word = new timed_word();
	    str >> word->positive >> word->length;
	    if(word->positive) TOTAL_POSITIVE++;
	    else TOTAL_NEGATIVE++;
	    word->symbols       = (int*)malloc((word->length + 1)*sizeof(int));
	    word->char_symbols  = (char*)malloc((word->length + 1)*sizeof(char));
	    word->time_values   = (double*)malloc((word->length + 1)*sizeof(double));
	    int index;
	    double time_sum = 0.0;
	    for(index = 0; index < word->length; ++index)
	    {
	      char c;
	      str >> c;
	      word->char_symbols[index] = c;
	      str >> word->time_values[index];
	      time_sum += word->time_values[index];
	      bool found = 0;
	      for(unsigned i = 0; i < current_alphabet_size; ++i)
	      {
	        if(alphabet[i] == c)
	        {
	          word->symbols[index] = i;
	          found = 1;
	          break;
	        }
	      }
	      if(found == 0)
	      {
	        alphabet[current_alphabet_size] = c;
	        word->symbols[index] = current_alphabet_size;
	        current_alphabet_size++;
	      }
	    }
	    word->symbols[index] = MAX_SYMBOL;
	    word->char_symbols[index] = '\0';
	    word->time_values[index] = time_sum;
	    words[line] = word;
	 	word->first_tail = new timed_tail(word, 0, 0);
	 	timed_tail* prev_tail = word->first_tail;
	    for(index = 1; index < word->length; ++index)
	    	prev_tail = new timed_tail(word, index, prev_tail);
	  }
};

timed_input::~timed_input(){
	for(int line = 0; line < NUM_WORDS; ++line)
		delete words[line];
	delete[] words;
	delete[] alphabet;
};

timed_word::timed_word(){
	symbols = 0;
	time_values = 0;
	char_symbols = 0;
	length = 0;
	positive = 0;
	first_tail = 0;
};

timed_word::timed_word(int* s, double* t, int l, bool p, timed_tail* f){
	symbols = s;
	time_values = t;
	char_symbols = 0;
	length = l;
	positive = p;
	first_tail = f;
};

timed_word::~timed_word(){
	if(first_tail != 0)
		delete first_tail;
};

timed_tail::timed_tail(timed_word* w, int i, timed_tail* t){
	node = new tail_node(0, 0, this);
	word = w;
	index = i;
	length = w->get_length() - index;
	next = 0;
	prev = t;
	if(t != 0) t->next = this;
	inconsistency_pair = 0;
	inconsistency_value = 0.0;
	consistency_pair = 0;
	consistency_value = 0.0;
	num_inconsistency = 0;
	num_consistency = 0;
	sum_consistency = 0;
	sum_inconsistency = 0;
};

timed_tail::~timed_tail(){
	delete node;
	if(next != 0)
		delete next;
};

const string timed_tail::to_str_shot() const{
	ostringstream ostr;
	ostr << "(" << get_symbol() << "," << get_time_value() << ")";
	const timed_tail *nt = next;
	while(nt != 0){
		ostr << nt->get_symbol();
		nt = nt->next;
	}
	return ostr.str();
};

const string timed_tail::to_str_long() const{
	ostringstream ostr;
	ostr << "(" << get_symbol() << "," << get_time_value() << ")";
	const timed_tail *nt = next;
	while(nt != 0){
		ostr << "(" << nt->get_symbol() << "," << nt->get_time_value() << ")";
		nt = nt->next;
	}
	return ostr.str();
};

