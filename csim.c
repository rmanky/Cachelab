// Robear Mankaryous rmankaryous@wpi.edu

#include "cachelab.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include <getopt.h>
#include <math.h>

typedef struct _line{
	int valid;
	unsigned long int tag;
	int lru;
} line;

FILE *tracefile;

extern char *optarg;
extern int optind;

void ageUpdate(line **arrayCache, int sets, int lines, int setUsed, int lineUsed) {
	for (int i = 0; i < lines; i++) {
		if (i == lineUsed) {
			(arrayCache[setUsed][i]).lru = 0;
		}
		else {
			(arrayCache[setUsed][i]).lru += 1;
		}
	}
}

void clean(line** arrayCache, int sets, int lines) {
	for(int i = 0; i < sets; i++) {
		free(arrayCache[i]);
	}
	free(arrayCache);
}

int LRU(line **arrayCache, int sets, int lines, int setUsed) {
	int rLine, rAge = -1;

	for (int i = 0; i < lines; i++) {
		if ((arrayCache[setUsed][i]).lru > rAge) {
			rAge = (arrayCache[setUsed][i]).lru;
			rLine = i;
		}
	}

	return rLine;
}

int main(int argc, char** argv) {
	int hits, misses, evicts = 0;

	int opt;
	int VERBOSE = 0; //default off
	int E;
	int s;
	int b;
	char * trace = "default";

	while ((opt = getopt(argc, argv, "hvs:E:b:t")) != -1) {
		switch (opt) {
			case 'v': VERBOSE = 1; break;
			case 's': s = atoi(optarg); break;
			case 'E': E = atoi(optarg); break;
			case 'b': b = atoi(optarg); break;
			case 't': trace = optarg; break;

			case 'h':
			default:
				fprintf(stderr, "Usage: %s [-hvsEbt] [tracefile]\n", argv[0]);
				exit(EXIT_FAILURE);
			}

			tracefile = fopen(trace, "r");
			if (tracefile == NULL) {
				printf("%s\n","Tracefile could not be opened");
				exit(EXIT_FAILURE);
			}

			int numberOfSets = pow(2, s);

			struct _line **arrayCache = malloc(numberOfSets * sizeof(struct _line*));
			if (arrayCache == NULL) {
				printf("%s\n","Malloc error");
				exit(EXIT_FAILURE);
			}

			for (int i = 0; i < numberOfSets; i++) {
					arrayCache[i] = malloc(E * sizeof(struct _line));
					if (arrayCache[i] == NULL) {
						printf("%s\n","Malloc error");
						exit(EXIT_FAILURE);
					}
			}

			char line[32];
			int size, hitBool, evictBool, invalidFirst, LRULine;
			char *insToken, *addrToken, *sizeToken;
			const char delimit[] = ", ";
			unsigned long int addr, tag, set;

			while(fgets(line, 32, tracefile) != NULL) {
				hitBool = 0;
				evictBool = 0;
				invalidFirst = 0;

				if (line[0] == ' ') {
					insToken = strtok (line, delimit);
					addrToken = strtok (NULL, delimit);
					sizeToken = strtok (NULL, delimit);
					size = atoi(sizeToken);

					if (VERBOSE) {
						printf("%s %s, %d ", insToken, addrToken, size);
					}

					addr = strtol(addrToken, NULL, 16);
					tag = (addr >> s) >> b;
					set = ((addr << (64 - s - b)) >> (64 - s));

					for (int i = 0; i < E; i++) {
						if ((arrayCache[set][i]).valid == 1 && (arrayCache[set][i]).tag == tag) {
							hitBool = 1;
							ageUpdate(arrayCache, numberOfSets, E, set, i);
							break;
						}
						else if (!((arrayCache[set][i]).valid) && !invalidFirst) {
							invalidFirst = i + 1;
						}
					}

					if (!hitBool) {
						if (invalidFirst) {
							evictBool = 0;
							(arrayCache[set][invalidFirst - 1]).valid = 1;
							(arrayCache[set][invalidFirst - 1]).tag = tag;
							ageUpdate(arrayCache, numberOfSets, E, set, invalidFirst - 1);
						}
						else {
							evictBool = 1;
							LRULine = LRU(arrayCache, numberOfSets, E, set);
							(arrayCache[set][LRULine]).valid = 1;
							(arrayCache[set][LRULine]).tag = tag;
							ageUpdate(arrayCache, numberOfSets, E, set, LRULine);
						}
					}

					switch (*insToken) {
						case 'M':
							if (hitBool) {
								hits += 2;
								if (VERBOSE) {
									printf("%s\n", "hit hit");
								}
							}
							else if (evictBool) {
								misses += 1;
								evicts += 1;
								hits += 1;
								if (VERBOSE) {
									printf("%s\n", "miss eviction hit");
								}
							}
							else {
								misses += 1;
								hits += 1;
								if (VERBOSE) {
									printf("%s\n", "miss hit");
								}
							}
							break;

						case 'L':
						case 'S':
							if (hitBool) {
								hits += 1;
								if (VERBOSE) {
									printf("%s\n", "hit");
								}
							}
							else if (evictBool) {
								misses += 1;
								evicts += 1;
								if (VERBOSE) {
									printf("%s\n", "miss eviction");
								}
							}
							else {
								misses += 1;
								if (VERBOSE) {
									printf("%s\n", "miss");
								}
							}
							break;
					}
				}
			}
			printSummary(hits, misses, evicts);
			fclose(tracefile);
			clean(arrayCache, numberOfSets, E);

			return 0;
	}
}
