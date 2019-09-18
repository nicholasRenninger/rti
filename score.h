#ifndef SCORE_H_
#define SCORE_H_

class tail_list;
class timed_tail;

extern int NUM_WORDS;
extern int TOTAL_POSITIVE;
extern int TOTAL_NEGATIVE;
extern int CONSISTENT_FINALS;
extern int INCONSISTENT_FINALS;
extern double CONSISTENT_TAILS;
extern double INCONSISTENT_TAILS;
extern double TIMED_CONSISTENCY;
extern double TIMED_INCONSISTENCY;
extern bool CONSISTENT;
extern double SPLIT_CHANCE;

extern void add_timed_inconsistencies(tail_list* list, tail_list* list2, timed_tail* tail);
extern void del_timed_inconsistencies(tail_list* list, tail_list* list2, timed_tail* tail);
extern void add_timed_consistencies(tail_list* list, tail_list* list2, timed_tail* tail);
extern void del_timed_consistencies(tail_list* list, tail_list* list2, timed_tail* tail);
extern void add_identical_tail_score(tail_list* list, tail_list* list2);
extern void del_identical_tail_score(tail_list* list, tail_list* list2);

#endif /*SCORE_H_*/
