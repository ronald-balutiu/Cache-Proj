#include "cachelab.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct
{
	int tag;
	int validb;
	unsigned long long int addr;
	int LRUholder;
} line;

typedef struct 
{
	int s;  // 2^s sets (set bits)
	int S;  // Number of sets
	int b;  // 2^b blocks (block bits)
	int B;  // Number of blocks
	int t;  // tag bits
	int E;  // Number of lines
	line** sets;
} cache;

typedef struct
{
	int sum_misses;
	int sum_hits;
	int sum_evictions;
} summary;

/* Finds and returns the least recently used line inside of a set. */
int LRUfinder(cache* lru_cache, int lru_set)
{
	int temp = -2;
	int line = 0;
	for (int i = 0; i < lru_cache -> E; ++i)
	{
		if (lru_cache -> sets[lru_set][i].LRUholder > temp)
		{
			temp = lru_cache -> sets[lru_set][i].LRUholder;
			line = i;
		}
	}
	return line;
}

summary* simulate(FILE* f, cache* sim_c, summary* sum)
{
	int misses = 0;             										// counter for total misses
	int hits = 0;              											// counter for total hits
	int evictions = 0;          										// counter for total evictions
	char* inst = malloc(sizeof(char)); 									// space for type of instruction
	unsigned long int* parse_addr = malloc(sizeof(unsigned long int));  // given address
	int* useless_size = malloc(sizeof(int));							// number of bytes accessed by operation

	while(fscanf(f, "%c %lx,%i", inst, parse_addr, useless_size) > 0) {
		if(*inst == 'I') {
		}
																		// if instruction load, do nothing
		else if(*inst == 'L' || *inst == 'S' || *inst == 'M') {
			unsigned long long int tag = (*parse_addr >> sim_c -> b) >> sim_c -> s;
			int set = ((*parse_addr << sim_c -> t) >> sim_c -> t)  >> sim_c -> b;
			int checked = 0;
			for (int i = 0; i < sim_c -> E; ++i)
			{
																		// logic for attempt leading to a hit
				if(sim_c -> sets[set][i].validb == 1 && sim_c -> sets[set][i].tag == tag && checked != 1) {
					sim_c -> sets[set][i].LRUholder = -1;
					hits++;
					checked = 1;
					for (int j = 0; j < sim_c -> E; ++j)
					{
						sim_c -> sets[set][j].LRUholder++;
					}
				}
																		// logic for attempt leading to a cold miss
				else if(sim_c -> sets[set][i].validb != 1 && checked != 1) {
					misses++;
					checked = 1;
					sim_c -> sets[set][i].validb = 1;
					sim_c -> sets[set][i].tag = tag;
					sim_c -> sets[set][i].addr = *parse_addr;
					sim_c -> sets[set][i].LRUholder = -1;
					for (int j = 0; j < sim_c -> E; ++j)
					{
						sim_c -> sets[set][j].LRUholder++;
					}
				}
																		// logic for attempt leading to an eviction
				else if(sim_c -> sets[set][i].validb == 1 && sim_c -> sets[set][i].tag != tag && checked != 1) {
					if(i == (sim_c -> E - 1)){
						misses++;
						evictions++;
						checked = 1;
						int replace_line = LRUfinder(sim_c, set);
						sim_c -> sets[set][replace_line].tag = tag;
						sim_c -> sets[set][replace_line].addr = *parse_addr;
						sim_c -> sets[set][replace_line].LRUholder = -1;
						for (int j = 0; j < sim_c -> E; ++j)
						{
							sim_c -> sets[set][j].LRUholder++;
						}
					}
				}
			}
			if(*inst == 'M'){
				hits++;
			}
		}
	}
	sum -> sum_misses = misses;
	sum -> sum_evictions = evictions;
	sum -> sum_hits = hits;
	return sum;
}

int main(int argc, char **argv)
{
	cache* cac = malloc(sizeof(cache));
	summary* return_sum = malloc(sizeof(summary));
	char* got_file;
	int Pacc = 1;
	if (strcmp(argv[Pacc], "-s") == 0) {
		cac -> s = atoi((argv[Pacc + 1]));
		Pacc += 2;
	}
	if (strcmp(argv[Pacc], "-E") == 0) {
		cac -> E = atoi((argv[Pacc + 1]));
		Pacc += 2;
	}
	if (strcmp(argv[Pacc], "-b") == 0) {
		cac -> b = atoi((argv[Pacc + 1]));
		Pacc += 2;
	}
	if (strcmp(argv[Pacc], "-t") == 0) {
		got_file = argv[Pacc + 1];
		Pacc += 2;
	}
	if (Pacc != 9) {
		fprintf(stdout, "Error in parsing!\n");
		exit(0);
	}

	cac -> S = pow(2, (cac -> s));
	cac -> B = pow(2, (cac -> b));
	cac -> t = 64 - (cac -> s) - (cac -> b);
	cac -> sets = (line **)malloc(sizeof(line *) * cac -> S);
	for (int i = 0; i < cac -> S; ++i)
	{
		cac -> sets[i] = (line *)malloc(sizeof(line) * cac-> E);
	}
	for (int i = 0; i < cac -> S; ++i)
	{
		for (int j = 0; j < cac -> E; ++j)
		{
			cac -> sets[i][j].validb = -1;
			cac -> sets[i][j].LRUholder = 0;
			cac -> sets[i][j].addr = 0;
			cac -> sets[i][j].tag = -1;
		}
	}

	FILE* opened_file = fopen(got_file, "r");

	return_sum = simulate(opened_file, cac, return_sum);

    printSummary(return_sum -> sum_hits, return_sum -> sum_misses, return_sum -> sum_evictions);
    return 0;
}
