/*
 *  globals.cpp
 *  IICT
 *
 *  Created by Sicco Verwer on 4/6/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "globals.h"
#include <sys/time.h>
#include <sys/resource.h>

int MAX_SYMBOL = 2;
double MIN_TIME_STEP = 0.0001;

int total_consistent_merges = 0;
double MIN_DELAY = 0.0;
double MAX_DELAY = 10000.0;
double NEVER		 = -1.0;
double MIN_INTERVAL_SIZE = 0.5;
int MAX_INTERVAL_DEPTH = 4;
int MAX_GUARDS = 3;

int total_apta_size = 0;

//bool consistent = true;
int positive_merge_count = 0;
int negative_merge_count = 0;
int positive_final_tails = 0;
int positive_merges = 0;
int maxi_size = 0;
int maxi_score = 0;
int num_node = 0;

int max_size = 0;
int best_score = 0;

std::vector<char> alphabet;

int alphabet_size = 2;
int num_words = 0;

int max_positive_tails = 0;
int total_positive_tails = 0;
int total_positive_words = 0;
double average_length = 0.0;

int conflicting_merges = 0;
int consistent_merges = 0;
bool consistent = true;

double start_time = 0.0;

int total_positive = 0;

double get_time() 
{
	struct timeval result;
	struct rusage usage;
	getrusage( RUSAGE_SELF, &usage);
	result = usage.ru_utime;
	//cout << "maxrss: " << usage.ru_maxrss << " ixrss: " << usage.ru_ixrss << " idrss: " << usage.ru_idrss  << " isrss: " << usage.ru_isrss << " minflt: " << usage.ru_minflt << " majflt: " << usage.ru_majflt << endl;
	return (double)result.tv_sec + ((double)result.tv_usec/1000000.0);
}

int intcmp(int* i1, int*i2)
{
	if( *i1 < *i2 )
		return -1;
	if( *i1 > *i2 )
		return 1;
	if( *i1 == MAX_SYMBOL )
		return 0;
	else
		return intcmp( ++i1, ++i2 );
}

