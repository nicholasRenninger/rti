#ifndef TIMED_DATA_H_
#define TIMED_DATA_H_

using namespace std;

class timed_input;
class timed_word;
class timed_tail;

#include <istream>
#include <string>
#include "timed_structures.h"
#include "globals.h"
#include "score.h"

extern double time_distance(const timed_tail*, const timed_tail*);

class timed_input
{
	char* alphabet;
	timed_word** words;
	
public:
	timed_input(istream& str);
	~timed_input();

	/* get methods */
	inline char get_symbol(int i) const{
		return alphabet[i];
	};
	
	inline timed_word* get_word(int i) const{
		return words[i];
	};
};

class timed_word
{
	int*	symbols;
	double*	time_values;
	char*	char_symbols;
	int		length;
	bool	positive;
	
	timed_tail* first_tail;

	friend class timed_input;

public:
	timed_word();
	timed_word(int*, double*, int, bool, timed_tail*);
	~timed_word();

	inline const int* get_symbols() const{
		return symbols;
	};
	
	inline const char* get_char_symbols() const{
		return char_symbols;
	};

	inline const double* get_time_values() const{
		return time_values;
	};
	
	inline const int get_length() const{
		return length;
	};
	
	inline const bool is_positive() const{
		return positive;
	};
	
	inline timed_tail* get_first_tail() const{
		return first_tail;
	};
};

class timed_tail
{
	tail_node   *node;
	timed_word  *word;
	int          index;
	int 		 length;
	
	timed_tail *next;
	timed_tail *prev;
	
	friend class tail_list;
	friend class tail_node;
	friend class timed_input;
	
public:
	timed_tail* inconsistency_pair;
	double inconsistency_value;
	timed_tail* consistency_pair;
	double consistency_value;

	double sum_consistency;
	double sum_inconsistency;
	int num_consistency;
	int num_inconsistency;

	/* constructs a tail from the specified arguments */
	timed_tail(timed_word *, int, timed_tail *);
	~timed_tail();

	/* used for printing */
	const string to_str_shot() const;
	const string to_str_long() const;	
	
	/* get methods */
	inline const timed_word	*get_word() const{
		return word;
	};
	
	inline const bool is_positive() const{
		return word->is_positive();
	};

	inline const int get_index() const{
		return index;
	};

	inline const int get_length() const{
		return length;
	};

	inline const int get_symbol() const{
		return word->get_symbols()[index];
	};
	
	inline const char get_char_symbol() const{
		return word->get_char_symbols()[index];
	};

	inline const int* get_symbols() const{
		return &(word->get_symbols()[index]);
	};
	
	inline const char* get_char_symbols() const{
		return &(word->get_char_symbols()[index]);
	};

	inline const double get_time_value() const{
		return word->get_time_values()[index];
	};
	
	inline const double* get_time_values() const{
		return &(word->get_time_values()[index]);
	};
	
	inline bool is_contained() const{
		return node->is_contained();
	};
	
	inline timed_tail *next_tail() const{
		return next;
	};
	
	inline timed_tail *prev_tail() const{
		return prev;
	};
};

#endif /*TIMED_DATA_H_*/
