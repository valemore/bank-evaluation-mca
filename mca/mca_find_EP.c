// Valerio Morelli, August 2016

// Usage:
// mca_find_EP.exe params.csv optimal_E.csv optimal_W.csv optimal_I.csv optimal_D.csv optimal_equity_W.csv optimal_equity_L.csv
//
// params.csv				-- Parameters to use
// optimal_E.csv			-- Output files for equity, cash, investment, defaulting flag, derivative of equity with respect to W and L
// optimal_W.csv
// optimal_I.csv
// optimal_D.csv
// optimal_equity_W.csv
// optimal_equity_L.csv


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
	if(argc < 8) {
		printf("Not enough arguments, expected seven.\n");
		return 1;
	}
	char *para_file = argv[1];
	char *optimal_equity_file = argv[2];
	char *optimal_cash_file = argv[3];
	char *optimal_investment_file = argv[4];
	char *optimal_defaulting_file = argv[5];
	char *optimal_equity_W_file = argv[6];
	char *optimal_equity_L_file = argv[7];	
	if(read_args_find_EP(para_file)) {
		return 2;
	}

	#ifdef TIMING
	time_t start, end;
	time(&start);
	#endif

	mca_find_EP();

	#ifdef TIMING
	time(&end);
	printf("Time: %.2lf seconds to run.\n", difftime(end, start));
	#endif

	if(write_array(optimal_equity_file, optimal_equity, P_grid_size, L_grid_size)) {
		clean_up_find_EP();
		return 3;
	}
	if(write_array(optimal_cash_file, optimal_cash, P_grid_size, L_grid_size)) {
		clean_up_find_EP();
		return 3;
	}
	if(write_array(optimal_investment_file, optimal_investment, P_grid_size, L_grid_size)) {
		clean_up_find_EP();
		return 3;
	}
	if(write_array(optimal_defaulting_file, optimal_defaulting, P_grid_size, L_grid_size)) {
		clean_up_find_EP();
		return 3;
	}
	if(write_array(optimal_equity_W_file, optimal_equity_W, P_grid_size, L_grid_size)) {
		clean_up_find_EP();
		return 3;
	}
	if(write_array(optimal_equity_L_file, optimal_equity_L, P_grid_size, L_grid_size)) {
		clean_up_find_EP();
		return 3;
	}

	clean_up_find_EP();
	
	return 0;
}
