
#include "timed_structures.h"
#include "timed_data.h"
#include "timed_automaton.h"

tail_list* deleted_lists = 0;

tail_list* new_tail_list(){
	if(deleted_lists == 0)
		return new tail_list();
	tail_list* result = deleted_lists;
	deleted_lists = deleted_lists->next;
	return result;
}

void delete_tail_list(tail_list* list){
	list->next = deleted_lists;
	deleted_lists = list;
	
	tail_node* next = 0;
	for(tail_node* node = list->head; node != 0; node = next){
		next = node->next_node;
		node->next_node = 0;
		node->prev_node = 0;
		node->contained = false;
	}
	list->head = 0;
	list->tail = 0;
	list->size = 0;
	
	list->pairs = 0;
	list->i_pairs = 0;
	list->consistency_value = 0;
	list->inconsistency_value = 0;

	//delete list;
}

/* Tail list methods */
tail_list::tail_list(){
	head = 0;
	tail = 0;
	size = 0;

	pairs = 0;
	i_pairs = 0;
	consistency_value = 0;
	inconsistency_value = 0;
};

tail_list::~tail_list(){
	tail_node* node = head;
	if(head == 0)
		return;

	tail_node* next = head->next_node;
	while(node != 0){
		delete node;
		node = next;
		next = node->next_node;
	}
};

tail_node::tail_node(tail_node *from, tail_node *to, timed_tail *t){
	next_node = to;
	prev_node = from;
	tail = t;
	contained = false;
};

void tail_list::add_tail(const timed_tail *t){
	assert(!contains(t));
	assert(t->node != 0);
	add_tail_node(t->node);
	t->node->contained = true;
};

void tail_list::del_tail(const timed_tail *t){
	assert(contains(t));
	assert(t->node != 0);
	del_tail_node(t->node);
	t->node->contained = false;
};

void tail_list::merge(tail_list* other){
	if(tail != 0){
		if(other->head != 0){
			tail->next_node = other->head;
			other->head->prev_node = tail;
			tail = other->tail;
		}
	} else {
		head = other->head;
		tail = other->tail;
	}
	other->head = 0;
	other->tail = 0;
	size += other->size;
};

/* Interval list methods */
interval_list::interval_list(){
	interval *in = new interval(MIN_DELAY, MAX_DELAY);
  	head = new interval_node(in);
  	tail = head;
};

interval_list::~interval_list(){
	interval_node* node = head;
	while(node != 0){
		interval_node* next = node->next_node;
		delete node;
		node = next;
	}
};

interval_node::interval_node(interval *i){
	assert(i != 0);
	in = i;
	next_node = 0;
	prev_node = 0;
};

interval_node::~interval_node(){
  delete in;
};

interval* interval_list::get_interval(const double time) const{
	interval_node* node = get_node(time);
	if(node != 0)
		return node->in;
	return 0;
};

interval_node* interval_list::get_node(const double time) const{
	interval_node* node = head;
	while(node != 0){
		if(node->get_interval()->end >= time)
			return node;
		node = node->next_node;
	}
	return 0;
};

interval *interval_list::split(const double split_time){
	interval_node *right = get_node(split_time);
	interval* new_in = new interval(right->in->begin, split_time);
	right->in->begin = split_time + MIN_TIME_STEP;
	interval_node *left = new interval_node(new_in);
	
	if(head == right){
		head = left;
		left->next_node = right;
		right->prev_node = left;
	} else {
		interval_node* prev = right->prev_node;
		prev->next_node = left;
		left->prev_node = prev;
		left->next_node = right;
		right->prev_node = left;
	}
	size++;
	
	assert(get_node(split_time) != right);

	return new_in;
};

interval *interval_list::undo_split(const double split_time){
	interval_node *left  = get_node(split_time);
	interval_node *right = left->next_node;
	interval* old_in = right->in;
	interval* new_in = left->in;
	
	assert(split_time == new_in->end);
	
	old_in->begin = new_in->begin;
	
	if(head == left){
		head = right;
		right->prev_node = 0;
	} else {
		interval_node* prev = left->prev_node;
		prev->next_node = right;
		right->prev_node = prev;
	}
	
	delete left;
	size--;
	
	return old_in;
};

