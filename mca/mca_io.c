// Valerio Morelli, August 2016

// This file defines three functions:
// int read_args(char *filename);									-- parse parameters from file
// int write_array(char *filename, double **a, int x, int y);		-- write double array with dimensions x and y to file
// int write_bool_array(char *filename, bool **a, int x, int y);	-- write bool array with deimsnions x and y to file

// NOTE: We want to use maximum precision for writing the floating point number array.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "mca.h"

// The following macros are defined in float.h
// All three should be available with a C11 compiler, C99 provides only the third.
// DBL_DECIMAL_DIG		-- number of signficant digits for a double
// LDBL_DECIMAL_DIG		-- number of signficant digits for a long double
// DECIMAL_DIG			-- number of signficant digits for the widest supported floating type

// This has to be changed if we change the data type, e.g. change to LDBL_DECIMAL_DIG if we were to use long double
#ifdef DBL_DECIMAL_DIG
const int precision = DBL_DECIMAL_DIG;
#else
const int precision = DECIMAL_DIG;
#endif

const int MAX_LINE_LENGTH = 256;																	// Maximal line length of parameter file

// Do you we need to check for ferror as well?

// The read_args function sets the following parameters:

// Example paramter file for mca_standalone
//r,0.05
//lambda,0.01
//sigma,0.119
//delta,0.025
//psi,1.5
//taxe,0
//taxi,0
//taxc,0
//P,50
//theta,0.08
//W_min,0
//W_max,75
//W_grid_size,76
//L_min,0
//L_max,300
//L_grid_size,201
//T,40
//T_grid_size,80001
//iteration_max,100
//iteration_tol,0.1
//trigger_equity_derivative_tol,0.01

// READ_ARGS ASSIGNS VALUES READ FROM FILE TO THE ABOVE PARAMETERS
// Expected layout: csv file like this
//					r,0.01
//					lambda,0.01
// The parameters must follow exactly this format exactly in the correct order as given above, with the only flexibility that additonal columns will be ignored.
// (These additional columns can be used for comments.)
//
// Returns 0 if successful, 1 for any error reading the file, 2 for any error closing the file
int read_args(char *filename) {
	FILE *fp;
	fp = fopen(filename, "r");
	char buf[MAX_LINE_LENGTH];
	if(fp != NULL) {
		char *para;
		char *value;
		int linenum = 1;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "r")) {
			printf("Expected reading r, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		r = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "lambda")) {
			printf("Expected reading lambda, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		lambda = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "sigma")) {
			printf("Expected reading sigma, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		sigma = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "delta")) {
			printf("Expected reading delta, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		delta = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "psi")) {
			printf("Expected reading psi, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		psi = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "taxe")) {
			printf("Expected reading taxe, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		taxe = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "taxi")) {
			printf("Expected reading taxi, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		taxi = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "taxc")) {
			printf("Expected reading taxc, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		taxc = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "P")) {
			printf("Expected reading P, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		P = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "theta")) {
			printf("Expected reading theta, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		theta = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "W_min")) {
			printf("Expected reading W_min, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		W_min = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "W_max")) {
			printf("Expected reading W_max, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		W_max = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "W_grid_size")) {
			printf("Expected reading W_grid_size, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		W_grid_size = atoi(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "L_min")) {
			printf("Expected reading L_min, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		L_min = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "L_max")) {
			printf("Expected reading L_max, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		L_max = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "L_grid_size")) {
			printf("Expected reading L_grid_size, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		L_grid_size = atoi(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "T")) {
			printf("Expected reading T, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		T = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "T_grid_size")) {
			printf("Expected reading T_grid_size, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		T_grid_size = atoi(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "iteration_max")) {
			printf("Expected reading iteration_max, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		iteration_max = atoi(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "iteration_tol")) {
			printf("Expected reading iteration_tol, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		iteration_tol = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "trigger_equity_derivative_tol")) {
			printf("Expected reading trigger_equity_derivative_tol, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		trigger_equity_derivative_tol = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "premium")) {
			printf("Expected reading premium, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		premium = atof(value);

		if(fclose(fp)) {
			printf("I/O error when closing file %s\n", filename);
			return 2;
		}
		return 0;
	} else {
		printf("Error, could not open file %s for reading.\n", filename);
		return 1;
	}
	close_after_error :
		if(fclose(fp))
			printf("I/O error when closing file %s\n", filename);
		return 1;
}

// Example parameter file for mca_find_EP

//r,0.05
//lambda,0.01
//sigma,0.119
//delta,0.025
//psi,1.5
//taxe,0
//taxi,0
//taxc,0
//P_min,0
//P_max,50
//theta,0.08
//W_min,-25
//W_max,75
//W_grid_size,76
//L_min,0
//L_max,300
//L_grid_size,201
//T,40
//T_grid_size,80001
//iteration_max,100
//iteration_tol,0.01
//trigger_equity_derivative_tol,0.01
//premium,0.01

// Read parameters for mca_findEP
int read_args_find_EP(char *filename) {
	FILE *fp;
	fp = fopen(filename, "r");
	char buf[MAX_LINE_LENGTH];
	if(fp != NULL) {
		char *para;
		char *value;
		int linenum = 1;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "r")) {
			printf("Expected reading r, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		r = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "lambda")) {
			printf("Expected reading lambda, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		lambda = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "sigma")) {
			printf("Expected reading sigma, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		sigma = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "delta")) {
			printf("Expected reading delta, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		delta = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "psi")) {
			printf("Expected reading psi, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		psi = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "taxe")) {
			printf("Expected reading taxe, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		taxe = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "taxi")) {
			printf("Expected reading taxi, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		taxi = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "taxc")) {
			printf("Expected reading taxc, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		taxc = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "P_min")) {
			printf("Expected reading P_min, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		P_min = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "P_max")) {
			printf("Expected reading P_max, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		P_max = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "P_grid_size")) {
			printf("Expected reading P_grid_size, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		P_grid_size = atoi(value);
		++linenum;


		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "theta")) {
			printf("Expected reading theta, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		theta = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "W_min")) {
			printf("Expected reading W_min, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		W_min = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "W_max")) {
			printf("Expected reading W_max, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		W_max = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "W_grid_size")) {
			printf("Expected reading W_grid_size, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		W_grid_size = atoi(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "L_min")) {
			printf("Expected reading L_min, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		L_min = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "L_max")) {
			printf("Expected reading L_max, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		L_max = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "L_grid_size")) {
			printf("Expected reading L_grid_size, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		L_grid_size = atoi(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "T")) {
			printf("Expected reading T, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		T = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "T_grid_size")) {
			printf("Expected reading T_grid_size, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		T_grid_size = atoi(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "iteration_max")) {
			printf("Expected reading iteration_max, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		iteration_max = atoi(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "iteration_tol")) {
			printf("Expected reading iteration_tol, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		iteration_tol = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "trigger_equity_derivative_tol")) {
			printf("Expected reading trigger_equity_derivative_tol, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		trigger_equity_derivative_tol = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "equity_cost")) {
			printf("Expected reading equity_cost, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		equity_cost = atof(value);
		++linenum;

		fgets(buf, sizeof(buf), fp);
		para = strtok(buf, ",");
		value = strtok(NULL, ",");
		if(para == NULL) {
			printf("Malformatted parameter name on line %i in %s\n", linenum, filename);
			goto close_after_error;
		}
		if(strcmp(para, "premium")) {
			printf("Expected reading premium, instead read %s\n on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}
		if(value == NULL) {
			printf("Malformatted parameter value for parameter %s on line %i in %s\n", para, linenum, filename);
			goto close_after_error;
		}		
		premium = atof(value);
		
		if(fclose(fp)) {
			printf("I/O error when closing file %s\n", filename);
			return 2;
		}
		return 0;
	} else {
		printf("Error, could not open file %s for reading.\n", filename);
		return 1;
	}
	close_after_error :
		if(fclose(fp))
			printf("I/O error when closing file %s\n", filename);
		return 1;
}

// writes floating point array a with dimensions x and y to filename, taking care to write with maximum precision.
// Returns 0 if successful, 1 in case of write error, 2 in case of error closing the file.
int write_array(char *filename, double **a, int x, int y) {
	FILE *fp;
	int i, j;
	fp = fopen(filename, "w");
	if(fp != NULL) {
		for(i = 0; i < x; ++i) {
			for(j = 0; j < y - 1; ++j) {
				fprintf(fp, "%.*e,", precision, a[i][j]);
			}
			fprintf(fp, "%.*e\n", precision, a[i][j]);
		}
		if(fclose(fp)) {
			printf("I/O error when closing file %s\n", filename);
			return 2;
		}
		return 0;
	} else {
		printf("Error writing file %s\n", filename);
		return 1;
	}
}

// writes bool array a with dimensions x and y to filename
// Returns 0 if successful, 1 in case of write error, 2 in case of error closing the file.
int write_bool_array(char *filename, bool **a, int x, int y) {
	FILE *fp;
	int i, j;
	fp = fopen(filename, "w");
	if(fp != NULL) {
		for(i = 0; i < x; ++i) {
			for(j = 0; j < y - 1; ++j) {
				fprintf(fp, "%i,", a[i][j]);
			}
			fprintf(fp, "%i\n", a[i][j]);
		}
		if(fclose(fp)) {
			printf("I/O error when closing file %s\n", filename);
			return 2;
		}
		return 0;
	} else {
		printf("Error writing file %s\n", filename);
		return 1;
	}
}
