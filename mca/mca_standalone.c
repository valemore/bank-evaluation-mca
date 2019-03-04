// Valerio Morelli, August 2016

// Usage:
// mca_standalone.exe params.csv Equity.csv Investment.csv Defaulting.csv
//
// params.csv		-- Parameters to use
// Equity.csv
// Investment.csv	-- Output files
// Defaulting.csv

// Any debug flags have to be specfied in mca.c.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// FLAG TO SPECIFIY WHETHER WE SHOULD TIME THE EXECUTION
#define TIMING
#ifdef TIMING
#include <time.h>
#endif

#include "mca_io.h"
#include "mca.h"

int main(int argc, char* argv[]) {
	if(argc < 7) {
		printf("Not enough arguments, expected six.\n");
		return 1;
	}
	char *para_file = argv[1];
	char *equity_file = argv[2];
	char *investing_file = argv[3];
	char *defaulting_file = argv[4];
	char *equity_W_file = argv[5];
	char *equity_L_file = argv[6];
	if(read_args(para_file)) {
		return 2;
	}

	#ifdef TIMING
	time_t start, end;
	time(&start);
	#endif

	mca_standalone();

	#ifdef TIMING
	time(&end);
	printf("Time: %.2lf seconds to run.\n", difftime(end, start));
	#endif

	if(write_array(equity_file, equity, W_grid_size, L_grid_size)) {
		clean_up_standalone();
		return 3;
	}
	if(write_array(investing_file, investment, W_grid_size, L_grid_size)) {
		clean_up_standalone();
		return 3;
	}
	if(write_bool_array(defaulting_file, defaulting, W_grid_size, L_grid_size)) {
		clean_up_standalone();
		return 3;
	}
	if(write_array(equity_W_file, equity_W, W_grid_size, L_grid_size)) {
		clean_up_standalone();
		return 3;
	}
	if(write_array(equity_L_file, equity_L, W_grid_size, L_grid_size)) {
		clean_up_standalone();
		return 3;
	}

	clean_up_standalone();
	
	return 0;
}
