/*
 *  globals.h
 *  IICT
 *
 *  Created by Sicco Verwer on 4/6/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#define DEBUG

using namespace std;

#include <list>
#include <set>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <set>
#include <list>
#include <stdlib.h>
#include <queue>
#include <assert.h>
#include <math.h>


extern int MAX_SYMBOL;
extern double MIN_TIME_STEP;

extern double MAX_DELAY;
extern double MIN_DELAY;
extern double NEVER;
extern double MIN_TIME_STEP;
extern double MIN_INTERVAL_SIZE;
extern int MAX_INTERVAL_DEPTH;
extern int MAX_SYMBOL;
extern int MAX_GUARDS;

extern bool consistent;
extern int total_consistent_merges;
extern int alphabet_size;
extern int num_words;
extern int total_apta_size;

extern int positive_merge_count;
extern int negative_merge_count;
extern int positive_final_tails;
extern int positive_merges;

extern int max_positive_tails;
extern int total_positive_tails;
extern double average_length;
extern int conflicting_merges;
extern int consistent_merges;
extern int maxi_size;
extern int maxi_score;
extern int max_size;
extern int best_score;
extern int num_node;
extern int total_positive_words;
extern std::vector<char> alphabet;

extern double start_time;

extern int total_positive;

int intcmp(int* i1, int*i2);

#define VERSION 0.2
#define REVIEW 1

/**
 *  get_time
 *  get the current process usage time
 */
double get_time();

#endif

