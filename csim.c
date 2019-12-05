#include "cachelab.h"

typedef struct _line{
	int valid;
	unsigned long int tag;
	int lru;
} line;

extern char *optarg;
extern int optind;

int main(int argc, char** argv) {
	int hits = 0;
	int misses = 0;
	int evicts = 0;

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
			case 't': t = optarg; break;

			case 'h':
			default:
				  // print out how to use this
					// print stuff here
		}
	}
