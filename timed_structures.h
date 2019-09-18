
#ifndef TIMED_STRUCTURES_H_
#define TIMED_STRUCTURES_H_

#include <assert.h>

class timed_tail;
class tail_list;
class tail_node;
class interval;
class interval_list;
class interval_node;

extern tail_list* new_tail_list();
extern void delete_tail_list(tail_list* list);

class tail_node{
private:
	timed_tail *tail;
	tail_node  *next_node;
	tail_node  *prev_node;
	
	bool contained;

	friend class tail_list;
	friend tail_list* new_tail_list();
	friend void delete_tail_list(tail_list*);

public:
	tail_node(tail_node *from, tail_node *to, timed_tail *t);
	
	inline timed_tail *get_tail() const{
		return tail;
	};
	
	inline const tail_node	*next() const{
		return next_node;
	};
	
	inline const tail_node	*prev() const{
		return prev_node;
	};
	
	inline bool is_contained() const{
		return contained;
	};
};

class tail_list{
private:
	tail_node *head;
	tail_node *tail;
	int size;
	
	tail_list* next;
	friend tail_list* new_tail_list();
	friend void delete_tail_list(tail_list*);
	
public:
	double inconsistency_value;
  double consistency_value;
  int pairs;
  int i_pairs;

	tail_list();
	~tail_list();
	
	inline const tail_node *get_head() const{
		return head;
	};
	
	inline const tail_node *get_tail() const{
		return tail;
	};

	inline int get_size() const{
		return size;
	};
	
	inline bool contains(const timed_tail* t) const{
		const tail_node* node = head;
		while(node != 0){
			if(node->get_tail() == t)
				return true;
			node = node->next();
		}
		return false;
	};
	
	inline void add_tail_node(tail_node* node){
		assert(!node->contained);
		
		if(head == 0)
			tail = node;
		else 
			head->prev_node = node;
		node->next_node = head;
		node->prev_node = 0;
		head = node;
		if(tail != 0) assert(head != 0);
		size++;
	};

	inline void del_tail_node(tail_node* node){
		assert(node->contained);

		if(node->prev_node != 0) node->prev_node->next_node = node->next_node;
		if(node->next_node != 0) node->next_node->prev_node = node->prev_node;
		if(head == node) head = node->next_node;
		if(tail == node) tail = node->prev_node;
		if(tail != 0) assert(head != 0);
		node->next_node = 0;
		node->prev_node = 0;
		size--;
	};

	void add_tail(const timed_tail *t);
	void del_tail(const timed_tail *t);
	void merge(tail_list* other);
};

class interval_node{
private:
	interval* in;
	interval_node* next_node;
	interval_node* prev_node;

	friend class interval_list;
	  
public:
	interval_node(interval* i);
	~interval_node();
	
	inline interval* get_interval() const{
		return in;
	};
	
	inline const interval_node* next() const{
		return next_node;
	};
	
	inline const interval_node* prev() const{
		return prev_node;
	};
};

class interval_list{
private:
	interval_node* head;
	interval_node* tail;
	int size;

public:
	interval_list();
	~interval_list();
  
	inline const interval_node* get_head() const{
		return head;
	};
	
	inline const interval_node* get_tail() const{
		return tail;
	};
	
	inline int get_size() const{
		return size;
	};
	
	interval* get_interval(const double time) const;
	interval_node* get_node(const double time) const;

	interval* split(const double);
	interval* undo_split(const double);
};

#endif /*TIMED_STRUCTURES_H_*/
