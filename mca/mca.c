// Valerio Morelli, August 2016

// This file contains the bulk of the MCA program.

// GENERAL REMARKS

// CONCERNING CHOICE OF COMPILER
// I chose the most recent gcc version 5.4.0, because:
// - Excellent optimization.
// - Support of all modern C features and standards.
// - Support of newest OpenMP specification.
// - Possibility to use DrMemory for memory debugging.
// 	 Compile with the following flags (32-bit): -m32 -g -fno-inline -fno-omit-frame-pointer
//	 and run it with drmemory -- executable and arguments
// - Easily installable locally.

// CONCERNING PARALLELISM WITH OPENMP
// gcc supports the newest OpenMP specification.
// Questions (Left open, because performance is good enough):
// - Is the parallel for the best way? Does this repeat the step of creating and terminating thread? Or is it smart enough to understand that it has to create
// 	 the threads once and keeps reusing them everytime the parallel for is entered?
// - Is it better to set the environment variable OMP_NUM_THREADS manually ?
// - Static scheduling appropriate? Other pitfalls?
// - In the section that needs parallelism, consider creating one big enclosing parallel section, with critical sections / synchronizations in between.

// CONCERNING INTEGRATION WITH MATLAB
// At first I built the program as a DLL such that it can be called via calllib from MATLAB.
// Two problems emerged:
// - OpenMP is badly supported, see answer to my question http://de.mathworks.com/matlabcentral/answers/298300-calllib-and-parallelization-using-openmp#answer_230689
// - There are small numerical differences depending on whether output is printed to the screen or written to a csv file, or whether I call the program in
//   MATLAB via calllib. I was still investigating this problem and did not find a satisfactory answer.
// Given that even if I re-wrote everything using MEX (calllib is the legacy method, but quicker to implement) there is no official support for OpenMP. Maybe it's
// possible to get everything working by linking against the OpenMP library used by MATLAB, and possibly even using the same compiler version used for building MATLAB on
// Windows, but that would be a lot of effort and could be complicated by the fact that it could be difficult to get ECB IT to install the correct compiler versions etc.
//
// In light of this, the "integration" with MATLAB is limited to standard IO.
// We are taking care to write the csv files with maximum precision.
// The output files can then be read in Matlab with csvread.

// PERFORMANCE IMPROVEMENTS
// - We are compiling using the -O3 flag and gcc is known to have very aggressive optimization. Micro-optimizing the by e.g. changing order of operations did
//   not provide any additional benefit.
// - We are using pointers to pointers for storing all our values. I expected the extra layer of indirection and the "jagged" layout in memory to hurt
//   performance. However, when I replaced the jagged arrays with one-dimensional ones, performance worsened drastically, either because some optimizations
//   with adjacent rows and columns become too difficult for the compiler, or because the large array sizes make the indexing too costly and the looking up and
//   caching for large arrays is actually more efficient with jagged arrays.
// - The best bet could be this: Use the C99 features: http://stackoverflow.com/questions/917783/how-do-i-work-with-dynamic-multi-dimensional-arrays-in-c
//   VLAs cannot have file scope, but can the syntactic sugar of VLAs be used to adress the one-dimensional arrays in a fast and elegant way?

// NUMERICAL STABILITY
// The only problem I see is in the following line in the update_defaulting_investment function:
// investment[i][j] = ( equity_L[i][j] - (1 - taxc) * equity_W[i][j] ) / myabs(( (1 - taxc) * psi * equity_W[i][j]));
// There is a high likelihood of cancellation in the numerator that is then amplified by the denominator.
// Looking at the results, the problem seems to be negligible if we ignore the rightmost 10% of the loan grid.

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "mca.h"

// DEBUG FLAGS
// Setting a debug flag leads to printing some information at strategic sections of the code.
// Search the debug flag name to see what what and where messages would be printed to STDOUT.
// When this creates big outputs, redirect STDOUT to a file e.g. by appending "> log.txt" to a command.
//#define DEBUG_PRINT_PARAMS
//#define DEBUG_PRINT_TIME
//#define DEBUG_PRINT_TERMINAL_VALUES
//#define DEBUG_PRINT_TIME_INTERMEDIATE_RESULT
//#define DEBUG_PRINT_FINAL_TWO_EQUITY_ITERATIONS
//#define DEBUG_PRINT_ITERATION
//#define DEBUG_PRINT_ITERATION_INNER
//#define DEBUG_PRINT_EQUITY_UPDATE
//#define DEBUG_PRINT_DEFAULTING_UPDATE
//#define DEBUG_PRINT_INVESTMENT_UPDATE

//#define DEBUG_FIND_EP_PRINT_PARAMS
#define DEBUG_FIND_EP_PRINT_P_LOOP

// The following debug flags print debug information for the equity update of a specific (i,j) position in the cash-loan grid (zero-indexed), iteration (not
// zero-indexed), and time step (not zero-indexed).
//#define DEBUG_EQUITY_i 7
//#define DEBUG_EQUITY_j 39
//#define DEBUG_EQUITY_iteration 1
//#define DEBUG_EQUITY_time 1
// Same as above for the investment and defaulting update
//#define DEBUG_DEFAULTING_INVESTMENT_i 7
//#define DEBUG_DEFAULTING_INVESTMENT_j 39
//#define DEBUG_DEFAULTING_INVESTMENT_iteration 1
//#define DEBUG_DEFAULTING_INVESTMENT_time 1

// The following flag writes the Equity, Investment, and Default for a specifc time step and iteration step (both not zero-indexed) to files
//#define DEBUG_WRITE_time 2
//#define DEBUG_WRITE_iteration 3

// The following function can be used to set breakpoint for a debugger (e.g. gdb)
//#define DEBUG_GDB
#ifdef DEBUG_GDB
int debug_gdb_dummy = 1;
void debug() {
	debug_gdb_dummy *= -1;																			// We actually do something such that the whole function does not get optimized away.
	//fprintf(stderr, "debug()\n");
}
#endif

#if defined(DEBUG_WRITE_time) && defined(DEBUG_WRITE_iteration)
#include "mca_io.h"
#endif

// We need to include this if we use OMP functions in addition to pragmas
//#include <omp.h>

// Simple helper functions square, myabs, max. myabs named thusly to avoid name collision with C library function.

double square(double x) {
	return x*x;
}

double myabs(double x) {
	if(x > 0)
		return x;
	else
		return -x;
}

double max(double a, double b) {
	if(a > b)
		return a;
	return b;
}

// create_grid returns a pointer to an initialized array of grid values, used later for the grid of values for the cash or the loan portfolio. The array has
// length grid_size and is evenly spaced between min and max.
double* create_grid(double min, double max, int grid_size) {
	double *grid = malloc(grid_size * sizeof(double));
	double step = ( max - min ) / ( grid_size -	1);			/* ! */
	for(int i = 0; i < grid_size; ++i) {
		grid[i] = min + i * step;
	}
	return grid;
}

// The following three functions create the result arrays.
// The equity and defaulting arrays are initialized in the terminal_equity_default() function.
// The investment array is initialized in the create_investment_grid() function itself.
// All arrays have to be destroyed with destroy_WL_grid()

// Create result grid for equity values
double** create_equity_WL_grid() {
	double **grid = malloc(W_grid_size * sizeof(double*));
	for(int i = 0; i < W_grid_size; ++i) {
		grid[i] = malloc(L_grid_size * sizeof(double));
	}
	return grid;
}

// Create result grid for investment values. This also initializes the investment values to a guess for the first time step.
// Needs L_grid to be already initialized!
double** create_investment_WL_grid() {
	double **grid = malloc(W_grid_size * sizeof(double*));
	for(int i = 0; i < W_grid_size; ++i) {
		grid[i] = malloc(L_grid_size * sizeof(double));
		for(int j = 0; j < L_grid_size; ++ j) {
			grid[i][j] = delta * L_grid[j];
		}
	}
	return grid;
}

// Create result grid for defaulting flags
bool** create_defaulting_WL_grid() {
	bool **grid = malloc(W_grid_size * sizeof(bool*));
	for(int i = 0; i < W_grid_size; ++i) {
		grid[i] = malloc(L_grid_size * sizeof(bool));
	}
	return grid;
}

// Destroy any of the 2D grids created by the above three functions.
void destroy_WL_grid(void **grid) {
	for(int i = 0; i < W_grid_size; ++i) {
		free(grid[i]);
	}
	free(grid);
}

// Used for destroying the PL grids created in mca_find_EP for iterating over different principals P
void destroy_PL_grid(void **grid) {
	for(int i = 0; i < P_grid_size; ++i) {
		free(grid[i]);
	}
	free(grid);
}

// The following two are used when printing information for debugging
void print_WL_grid(double **grid) {
	for(int i = 0; i < W_grid_size; ++i) {
		for(int j = 0; j < L_grid_size; ++j) {
			printf("%f\t", grid[i][j]);
		}
		printf("\n");
	}
}

void print_bool_WL_grid(bool **grid) {
	for(int i = 0; i < W_grid_size; ++i) {
		for(int j = 0; j < L_grid_size; ++j) {
			printf("%i\t", grid[i][j]);
		}
		printf("\n");
	}
}

// Compute terminal boundary values
void terminal_equity_default(double *W_grid, double *L_grid, double **equity_T, bool **defaulting_T) {
	for(int i = 0; i < W_grid_size; ++i) {
		for(int j = 0; j < L_grid_size; ++j) {
			if(W_grid[i] >= 0)
				equity_T[i][j] = max(W_grid[i] * (1 - taxe) - P + L_grid[j], 0);
			else
				equity_T[i][j] = max(W_grid[i] - P + (1 - theta) * L_grid[j], 0);
			
			if(equity_T[i][j] == 0)
				defaulting_T[i][j] = true;
			else
				defaulting_T[i][j] = false;
		}
	}
}

// Updates the values new_equity points to.
// Requires that: equity points to the values computed in the preceding time step,
// 				  iteration_equity points to the values for equity computed in the preceding iteration step,
// 				  investment points to the the values for investment computed in the preceding iteration step.
#if defined(DEBUG_EQUITY_time) || defined(DEBUG_WRITE_time) || defined(DEBUG_GDB)
void update_new_equity(double **new_equity, double **equity, double **iteration_equity, double **investment, int i, int j, int t, int iteration) {
#else
void update_new_equity(double **new_equity, double **equity, double **iteration_equity, double **investment, int i, int j) {
#endif
	double b100p, b100n, b010p, b010n, b200, b020, b110;
	double Qf, disc;
	// Numerator probabilties
	// npx{p|nh}y{p|ng} : numerator probability for a {positive | negative} step {h | g} on the xy axis. h indicates a step on the x axis and g indicates a step on the y axis.
	double npxnhy, npxphy, npxyng, npxypg;
	// ptau is the probability for a time step.
	double uc, ptau;

	// Probability of a move on the grid, positive/negative hg (step on x axis) or positive/negative g (step on y axis)
	double pxypg, pxphy, pxnhy, pxyng, pxy;

	#ifdef DEBUG_EQUITY_time
	b100p = -7777;
	b100n = -7777;
	b010p = -7777;
	b010n = -7777;
	b200 = -7777;
	b020 = -7777;
	b110 = -7777;
	Qf = -7777;
	disc = -7777;
	npxnhy = -7777;
	npxphy = -7777;
	npxyng = -7777;
	npxypg = -7777;
	uc = -7777;
	ptau = -7777;
	pxypg = -7777;
	pxphy = -7777;
	pxnhy = -7777;
	pxyng = -7777;
	pxy = -7777;
	#endif
	
	if(investment[i][j] > 0 && W_grid[i] >= 0) {
		b100p = (1 / dW) * (1 - taxc) * (1 - taxe) * ( delta * L_grid[j] + W_grid[i] * (r - lambda) );
		b100n = (1 / dW) * (1 - taxc) * (1 - taxe) * ( coupon + myabs(investment[i][j]) + 0.5 * square(investment[i][j]) * psi );
		b010p = (1 / dL) * (investment[i][j]);
		b010n = (1 / dL) * delta * L_grid[j];
	}
	else if(investment[i][j] <= 0 && W_grid[i] >= 0) {
		b100p = (1 / dW ) * (1 - taxc) * (1 - taxe) * ( delta * L_grid[j] + W_grid[i] * (r - lambda) + myabs(investment[i][j]) );

		b100n = (1 / dW ) * (1 - taxc) * (1 - taxe) * ( coupon + 0.5 * square(investment[i][j]) * psi );

		b010p = 0; 

		b010n = 1/dL * ( myabs(investment[i][j]) + delta * L_grid[j] );
	}
	else if(investment[i][j] > 0 && W_grid[i] < 0) {  
		b100p = (1 / dW )* (1 - taxc) * (1 - taxe) * (delta * L_grid[j] );

		b100n = (1 / dW ) * (1 - taxc) * (1 - taxe) * ( myabs(W_grid[i]) * r + coupon  + myabs(investment[i][j]) + 0.5 * square(investment[i][j]) * psi );

		b010p = 1/dL * (investment[i][j]); 

		b010n = 1/dL * (delta  * L_grid[j]);
	}
	else { //if(investment[i][j] <= 0 && W_grid[i] < 0) {
		b100p = (1 / dW )* (1 - taxc) * (1 - taxe) * ( delta * L_grid[j] + myabs(investment[i][j]) );

		b100n = (1 / dW ) * (1 - taxc) * (1 - taxe) * ( myabs(W_grid[i]) * r + coupon  + 0.5 * square(investment[i][j]) * psi );

		b010p = 0; 

		b010n = 1/dL * ( myabs(investment[i][j]) + delta*L_grid[j] );
	}

	b200 = 0;																						// Is this needed?
	b020 = square(1/dL) * (0.5 * square(sigma * L_grid[j]));    
	b110 = 0;																						// Is this needed?

	Qf = 1/dT + b100n + b010p + b010n + b100p + 2 * b200 + 2 * b020 - myabs(b110);
	disc = exp(-rhohat / Qf);

	// Numerator Probabilities 
	// p[x-h,y]
	npxnhy = b100n + b200 - 0.5 * myabs(b110);
	// p[x+h,y]
	npxphy = b100p + b200 - 0.5 * myabs(b110);
	// p[x,y-g]
	npxyng = b010n + b020 - 0.5 * myabs(b110);
	// p[x,y+g]
	npxypg = b010p + b020 - 0.5 * myabs(b110);    
        
	uc = 0;																							// Is this needed?
        
	ptau = 1 / (Qf * dT);

	if (i == 0 && j == 0) {
		pxy = 1  - ptau;
                
		new_equity[i][j] =  1/Qf * uc +
			disc * ptau * equity[i][j] +
			disc * pxy * iteration_equity[i][j];
	}
	else if (i > 0 && i < (W_grid_size - 1) && j == 0) {
		pxypg = 1/Qf * npxypg;
		pxphy = 1/Qf * npxphy;
		pxnhy = 1/Qf * npxnhy;
		pxy = 1 - pxypg - pxphy - pxnhy - ptau;
                    
		new_equity[i][j] =  1/Qf * uc +
			disc * ptau * equity[i][j] +
			disc * pxy * iteration_equity[i][j] +
			disc * pxphy * iteration_equity[i+1][j] +
			disc * pxnhy * iteration_equity[i-1][j] +
			disc * pxypg * iteration_equity[i][j+1];
	}
	else if (i == 0 && j > 0 && j < (L_grid_size - 1)) {
		pxypg = 1/Qf * npxypg;
		pxphy = 1/Qf * npxphy;
		pxnhy = 1/Qf * npxnhy;
		pxyng = 1/Qf * npxyng;
		pxy = 1 - pxypg - pxphy - pxnhy - pxyng - ptau;
                
		new_equity[i][j] =   1/Qf * uc +
			disc * ptau * equity[i][j] +
			disc * pxy * iteration_equity[i][j] +
			disc * pxphy * iteration_equity[i+1][j] +
			disc * pxypg * iteration_equity[i][j+1] +
			disc * pxyng * iteration_equity[i][j-1];
	}
	else if (i == 0 && j == (L_grid_size - 1)) {													// SAME AS BELOW
		pxy = 1  - ptau;
		
		new_equity[i][j] =  1/Qf * uc +
			disc * ptau * equity[i][j] +
			disc * pxy * iteration_equity[i][j];
	}
	else if (i > 0 && i < (W_grid_size - 1) && j == (L_grid_size - 1)) {							// SAME AS ABOVE
		pxy = 1  - ptau;
		
		new_equity[i][j] =  1/Qf * uc +
			disc * ptau * equity[i][j] +
			disc * pxy * iteration_equity[i][j];

	}
	else if (i == (W_grid_size - 1) && j > 0 && j < (L_grid_size - 1)) {
		pxypg = 1/Qf * npxypg;
		// pxphy(i,j) = 0;
		pxnhy = 1/Qf * npxnhy;
		pxyng = 1/Qf * npxyng;
		pxy = 1 - pxypg - pxnhy - pxyng - ptau;
                    
		new_equity[i][j] =  1/Qf * uc +
			disc * ptau * equity[i][j] +
			disc * pxy * iteration_equity[i][j] +
			disc * pxnhy * iteration_equity[i-1][j] +
			disc * pxypg * iteration_equity[i][j+1] +
			disc * pxyng * iteration_equity[i][j-1];

	}
	else if (i == (W_grid_size - 1) && j == 0) {
		pxypg = 1/Qf * npxypg;
		//pxphy(i,j) = 0;
		pxnhy = 1/Qf * npxnhy;
		//pxyng(i,j) = 0;
		pxy = 1 - pxypg - pxnhy - ptau;
                    
		new_equity[i][j] =  1/Qf * uc +
			disc * ptau * equity[i][j] +
			disc * pxy * iteration_equity[i][j] +
			disc * pxnhy * iteration_equity[i-1][j] +
			disc * pxypg * iteration_equity[i][j+1];
	}
	else if (i == (W_grid_size - 1) && j == (L_grid_size - 1)) {
		//pxypg(i,j) = 0;
		//pxphy(i,j) = 0;
		pxnhy = 1/Qf * npxnhy;
		pxyng = 1/Qf * npxyng;
		pxy = 1 - pxnhy - pxyng - ptau;
                    
		new_equity[i][j] =  1/Qf * uc +
			disc * ptau * equity[i][j] +
			disc * pxy * iteration_equity[i][j] +
			disc * pxnhy * iteration_equity[i-1][j] +
			disc * pxyng * iteration_equity[i][j-1];
	}
	else {
		pxypg = 1/Qf * npxypg;
		pxphy = 1/Qf * npxphy;
		pxnhy = 1/Qf * npxnhy;
		pxyng = 1/Qf * npxyng;
		pxy = 1 - pxypg - pxphy - pxnhy - pxyng - ptau;
                    

                
		new_equity[i][j] =  1/Qf * uc +
			disc * ptau   * equity[i][j] +														// Check this!
			disc * pxy 	  * iteration_equity[i][j]  + 
			disc * pxphy  * iteration_equity[i+1][j]  +
			disc * pxnhy  * iteration_equity[i-1][j]  + 
			disc * pxypg  * iteration_equity[i][j+1]  +
			disc * pxyng  * iteration_equity[i][j-1];

            
	}
	#ifdef DEBUG_EQUITY_time
	if(i == DEBUG_EQUITY_i && j == DEBUG_EQUITY_j && iteration == DEBUG_EQUITY_iteration && t == DEBUG_EQUITY_time) {
		printf("DEBUG_EQUITY for (i, j, iteration, t) = (%i, %i, %i, %i)\n", i, j, iteration, t);
		printf("%-12s%-12g\n", "New equity:", new_equity[i][j]);
		printf("%-12s%-12g\n", "b100p:", b100p);
		// printf("%-12s%-12g\n", "dW:", dW);
		// printf("%-12s%-12g\n", "taxc:", taxc);
		// printf("%-12s%-12g\n", "taxe:", taxe);
		// printf("%-12s%-12g\n", "delta:", delta);
		// printf("%-12s%-12g\n", "L_grid[j]:", L_grid[j]);
		// printf("%-12s%-12g\n", "W_grid[i]:", W_grid[i]);
		// printf("%-12s%-12g\n", "r:", r);
		// printf("%-12s%-12g\n", "lambda:", lambda);
		printf("%-12s%-12g\n", "b100n:", b100n);
		printf("%-12s%-12g\n", "b010p:", b010p);
		printf("%-12s%-12g\n", "b010n:", b010n);
		printf("%-12s%-12g\n", "b200:", b200);
		printf("%-12s%-12g\n", "b020:", b020);
		printf("%-12s%-12g\n", "b110:", b110);
		printf("%-12s%-12g\n", "Qf:", Qf);
		printf("%-12s%-12g\n", "disc:", disc);
		printf("%-12s%-12g\n", "npxnhy:", npxnhy);
		printf("%-12s%-12g\n", "npxphy:", npxphy);
		printf("%-12s%-12g\n", "npxyng:", npxyng);
		printf("%-12s%-12g\n", "npxypg:", npxypg);
		printf("%-12s%-12g\n", "uc:", uc);
		printf("%-12s%-12g\n", "ptau:", ptau);
		printf("%-12s%-12g\n", "pxypg:", pxypg);
		printf("%-12s%-12g\n", "pxphy:", pxphy);
		printf("%-12s%-12g\n", "pxnhy:", pxnhy);
		printf("%-12s%-12g\n", "pxyng:", pxyng);
		printf("%-12s%-12g\n", "pxy:", pxy);
	}
	#endif
}

// Updates the values pointed to by defaulting and investment, after the new_equity values have been updated.
#if defined(DEBUG_DEFAULTING_INVESTMENT_time) || defined(DEBUG_WRITE_time) || defined(DEBUG_GDB)
void update_defaulting_investment(int t, int iteration) {
#else
void update_defaulting_investment() {
#endif
	// DEFAULT CONDITION
	// derivative of equity wrt W == derivative of equity wrt L == 0
	for(int i = 0; i < W_grid_size; ++i) {
		for(int j = 0; j < L_grid_size; ++j) {
			// Check for extreme cases on the cash grid, use the appropriate finite difference
			if(i == 0)
				equity_W[i][j] = ( new_equity[i+1][j] - new_equity[i][j] ) / dW;
			else if(i == W_grid_size - 1)
				equity_W[i][j] = ( new_equity[i][j] - new_equity[i - 2][j] ) / (2 * dW);
			else
				equity_W[i][j] = ( new_equity[i+1][j] - new_equity[i-1][j] ) / (2 * dW);
			
			double trigger_equity_W = myabs(equity_W[i][j]);
			
			// Check for extreme cases on the loan grid, use the appropriate finite difference
			if(j == L_grid_size - 1)
				equity_L[i][j] = ( new_equity[i][j] - new_equity[i][j-2] ) / (2 * dL);
			else if(j == 0 || defaulting[i][j-1] == true)											// REPLACE ????
				equity_L[i][j] = ( new_equity[i][j+1] - new_equity[i][j] ) / dL;
			else
				equity_L[i][j] = ( new_equity[i][j+1] - new_equity[i][j-1] ) / (2 * dL);

			double trigger_equity_L = myabs(equity_L[i][j]);

			if(trigger_equity_W > trigger_equity_derivative_tol && trigger_equity_L > trigger_equity_derivative_tol) { // CHECK ????????
				defaulting[i][j] = false;
			}
			if(defaulting[i][j] == false)
				//investment[i][j] = ((1 - taxc) * equity_W[i][j] - equity_L[i][j]) / ( - (1 - taxc) * psi * equity_W[i][j]);
				investment[i][j] = ( equity_L[i][j] - (1 - taxc) * equity_W[i][j] ) / myabs(( (1 - taxc) * psi * equity_W[i][j]));
				//investment[i][j] = exp2(log2(equity_L[i][j]) - log2(psi * (1 - taxc) * equity_W[i][j])) - 1 / psi;
				//investment[i][j] = equity_L[i][j] / ( psi * (1 - taxc) * equity_W[i][j]) - 1 / psi;
			else
				investment[i][j] = 0;

			#ifdef DEBUG_GDB
			if(equity_W[i][j] < 0 || equity_L[i][j] < 0) {
				debug();
			}
			#endif

			#ifdef DEBUG_DEFAULTING_INVESTMENT_time
			if(i == DEBUG_DEFAULTING_INVESTMENT_i && j == DEBUG_DEFAULTING_INVESTMENT_j && iteration == DEBUG_DEFAULTING_INVESTMENT_iteration && t == DEBUG_DEFAULTING_INVESTMENT_time) {
				printf("DEBUG_DEFAULTING_INVESTMENT for (i, j, iteration, t) = (%i, %i, %i, %i)\n", i, j, iteration, t);
				printf("%-18s%-12g\n", "New investment:", investment[i][j]);
				printf("%-18s%-12i\n", "New defaulting:", defaulting[i][j]);
				printf("%-18s%-12g\n", "equity_W", equity_W[i][j]);
				printf("%-18s%-12g\n", "trigger_equity_W", trigger_equity_W[i][j]);
				printf("%-18s%-12g\n", "equity_L", equity_L[i][j]);
				printf("%-18s%-12g\n", "trigger_equity_L", trigger_equity_L[i][j]);
			}
			#endif
		}
	}
}

// Perform a time step
#if defined(DEBUG_EQUITY_time) || defined(DEBUG_DEFAULTING_INVESTMENT_time) || defined(DEBUG_WRITE_time) || defined(DEBUG_GDB)
void step(int t) {
#else
void step() {
#endif
	// The following three variables are declared outside the loop so that we print information about them after the loop if necessary
	int iteration = 1;
	double sum_squared_equity_change;																
	double **tmp;																					
	for(; iteration <= iteration_max; ++iteration) {
		// We try to find the optimal investment decision iteratively.
		// When the equity value does not change much from one iteration to the next, or we have reached iteration_max,
		// we stick with whatever values we have computed.

		#ifdef DEBUG_PRINT_ITERATION_INNER
		printf("Entering iteration %i\n", iteration);
		#endif
		
		sum_squared_equity_change = 0;

		// PARALLELIZED
		// update_new_equity() needs equity to point to equity of previous time step, iteration_equity to point to equity of previous iteration step, and
		// investment to point to investment of previous iteration step.  This means we can simply loop over the cash loan grid with parallel for loops.
		//
		// Explanation: Each thread has a private copy of sum_squared_equity_change that gets reduced to one in the end by adding, the collapse(2) allows for
		// parallelizing both loops in one go (it simply transforms the nested loop into one loop, and parallelizes that. schedule(static) divides the loop into
		// equal-sized chunks for each thread, which produces little overhead and works well if every loop iteration is similarly intensive.
		//
		// The schedule(static) in the following does not actually seem to have an effect!
		// I was only able to affect the value reported by get_omp_schedule() by setting it in a serial part of the code.
		// If you perform changes, be sure to inspect with the OMP runtime functions.
		// However, performance did not change at all between different scheduling settings or chunk sizes.
		
		# pragma omp parallel for reduction(+:sum_squared_equity_change) schedule(static) collapse(2)
		for(int i = 0; i < W_grid_size; ++i) {
			for(int j = 0; j < L_grid_size; ++j) {
				// Compute new equity values for each position (i, j) in the cash-loan grid
				// The equity value depends also on the optimal investment strategy.
				// In the first iteration this is the investment guess from the previously computed point in time.
				// In later iterations we use the investment determined in the preceding iteration.
				#if defined(DEBUG_EQUITY_time) || defined(DEBUG_WRITE_time) || defined(DEBUG_GDB)
				update_new_equity(new_equity, equity, iteration_equity, investment, i, j, t, iteration);
				#else
				update_new_equity(new_equity, equity, iteration_equity, investment, i, j);
				#endif

				//#ifdef DEBUG_PRINT_EQUITY_UPDATE
				//printf("Pos. (%i, %i) -- New:\t%f\tPrev:\t%f\n", i, j, new_equity[i][j], iteration_equity[i][j]);
				//#endif
				sum_squared_equity_change += square(new_equity[i][j] - iteration_equity[i][j]);
			}
		}
		
		// POSSIBLY PARALLELIZE THIS AS WELL, SAME AS ABOVE.
		// BE CAREFUL, the update_defaulting_investment() code also uses defaulting[i][j-1] !!!

		// In any case, we need these for the next iteration or the next time step.
		#if defined(DEBUG_DEFAULTING_INVESTMENT_time) || defined(DEBUG_WRITE_time) || defined(DEBUG_GDB)
		update_defaulting_investment(t, iteration);
		#else
		update_defaulting_investment();
		#endif

		// Update iteration_equity
		// A the end of each iteration in the outer loop, iteration_equity points to the most recently computed equity value.
		tmp = iteration_equity;
		iteration_equity = new_equity;
		new_equity = tmp;

		#ifdef DEBUG_PRINT_EQUITY_UPDATE
		printf("Equity after iteration %i:\n", iteration);
		print_WL_grid(iteration_equity);
		#endif

		#ifdef DEBUG_PRINT_INVESTMENT_UPDATE
		printf("Investment after iteration %i:\n", iteration);
		print_WL_grid(investment);
		#endif

		#ifdef DEBUG_PRINT_DEFAULTING_UPDATE
		printf("Defaulting after iteration %i:\n", iteration);
		print_bool_WL_grid(defaulting);
		#endif

		#ifdef DEBUG_PRINT_ITERATION_INNER
		printf("End iteration %i\t Change:%f\n", iteration, sum_squared_equity_change);
		#endif

		#if defined(DEBUG_WRITE_time) && defined(DEBUG_WRITE_iteration)
		if(t == DEBUG_WRITE_time && iteration == DEBUG_WRITE_iteration) {
			char file_name[256] = {0};
			snprintf(file_name, sizeof file_name, "DEBUG_Equity_t%i_it%i.csv", DEBUG_WRITE_time, DEBUG_WRITE_iteration);
			write_array(file_name, iteration_equity, W_grid_size, L_grid_size);
			snprintf(file_name, sizeof file_name, "DEBUG_Investment_t%i_it%i.csv", DEBUG_WRITE_time, DEBUG_WRITE_iteration);
			write_array(file_name, investment, W_grid_size, L_grid_size);
			snprintf(file_name, sizeof file_name, "DEBUG_Defaulting_t%i_it%i.csv", DEBUG_WRITE_time, DEBUG_WRITE_iteration);
			write_bool_array(file_name, defaulting, W_grid_size, L_grid_size);
		}
		#endif

		// If equity value does not change much, break.
		if(sum_squared_equity_change < iteration_tol)
			break;
	}
	#ifdef DEBUG_PRINT_ITERATION
	printf("-- Exited after iteration: %i\t-- Change:%f\n", iteration, sum_squared_equity_change);
	#endif

	#ifdef DEBUG_PRINT_FINAL_TWO_EQUITY_ITERATIONS
	printf("Equity value of last iteration:\n");
	print_WL_grid(iteration_equity);
	printf("Equity value of second-to-last iteration:\n");
	print_WL_grid(tmp);
	#endif

	// When exiting the iteration loop, iteration_equity contains the most recently computed equity value.
	// We also need iteration_equity to contain these values for the first iteration of the next time step.
	// Therefore we need to copy values.
	for(int i = 0; i < W_grid_size; ++i) {															
		for(int j = 0; j < L_grid_size; ++j) {
			equity[i][j] = iteration_equity[i][j];
		}
	}
}

// Used for printing information when debugging
void print_intermediate_result(double t) {
	printf("Equity value at time %f\n", t);
	for(int i = 0; i < W_grid_size; ++i) {
		for(int j = 0; j < L_grid_size; ++j) {
			printf("%f\t", equity[i][j]);
		}
		printf("\n");
	}
	printf("Investment value at time %f\n", t);
	for(int i = 0; i < W_grid_size; ++i) {
		for(int j = 0; j < L_grid_size; ++j) {
			printf("%f\t", investment[i][j]);
		}
		printf("\n");
	}
	printf("Defaulting flag at time %f\n", t);
	for(int i = 0; i < W_grid_size; ++i) {
		for(int j = 0; j < L_grid_size; ++j) {
			printf("%i\t", defaulting[i][j]);
		}
		printf("\n");
	}
}

// Functions that performs the time steps from T to T_min
void traverse_time() {
	#ifdef DEBUG_PRINT_TIME
	double t;
	#endif
	for(int i = 1; i < T_grid_size; ++i) {
		#ifdef DEBUG_PRINT_TIME
		t = T - i *dT;
		printf("---- Time step:%-16f to %-16f\n", t+dT, t);
		#endif
		#if defined(DEBUG_EQUITY_time) || defined(DEBUG_DEFAULTING_INVESTMENT_time) || defined(DEBUG_WRITE_time) || defined(DEBUG_GDB)
		step(i);
		#else
		step();
		#endif
		#ifdef DEBUG_PRINT_TIME_INTERMEDIATE_RESULT
		print_intermediate_result(t);
		#endif
	}
	#ifdef DEBUG_GDB
	printf("Debug dummy: %i\n", debug_gdb_dummy);
	#endif
}

// Set up global variables and data structures for mca, except those related to P -- only coupon at the moment
// This funciton can then be invoked both by mca_standalone and mca_find_EP to set up common global variables and data structures.
void mca_initial_setup() {
	// Set up global variables
	rhohat = (1 - taxi) * r;

	dW = ( W_max - W_min ) / ( W_grid_size - 1);
	dL = ( L_max - L_min ) / ( L_grid_size - 1);
	dT = T / ( T_grid_size - 1);

	// Set up data structures
	W_grid = create_grid(W_min, W_max, W_grid_size);
	L_grid = create_grid(L_min, L_max, L_grid_size);

	equity = create_equity_WL_grid();
	defaulting = create_defaulting_WL_grid();
	investment = create_investment_WL_grid();

	new_equity = create_equity_WL_grid();
	iteration_equity = create_equity_WL_grid();

	equity_W = create_equity_WL_grid();
	equity_L = create_equity_WL_grid();
}

// Funciton to set up coupon
void setup_coupon() {
	coupon = (r + premium) * P;
}

// Free memory after mca_standalone
void clean_up_standalone() {
	free(W_grid);
	free(L_grid);
		
	destroy_WL_grid((void**) equity);
	destroy_WL_grid((void**) defaulting);
	destroy_WL_grid((void**) investment);

	destroy_WL_grid((void**) new_equity);
	destroy_WL_grid((void**) iteration_equity);
	destroy_WL_grid((void**) equity_W);
	destroy_WL_grid((void**) equity_L);
}

// Function called by the main function of mca_standalone.exe
void mca_standalone() {
	#ifdef DEBUG_PRINT_PARAMS
	printf("%-32s%-12g\n", "r", r);
	printf("%-32s%-12g\n", "lambda", lambda);
	printf("%-32s%-12g\n", "sigma", sigma);
	printf("%-32s%-12g\n", "delta", delta);
	printf("%-32s%-12g\n", "psi", psi);
	printf("%-32s%-12g\n", "taxe", taxe);
	printf("%-32s%-12g\n", "taxi", taxi);
	printf("%-32s%-12g\n", "taxc", taxc);
	printf("%-32s%-12g\n", "P", P);
	printf("%-32s%-12g\n", "theta", theta);
	printf("%-32s%-12g\n", "W_min", W_min);
	printf("%-32s%-12g\n", "W_max", W_max);
	printf("%-32s%-12i\n", "W_grid_size", W_grid_size);
	printf("%-32s%-12g\n", "L_min", L_min);
	printf("%-32s%-12g\n", "L_max", L_max);
	printf("%-32s%-12i\n", "L_grid_size", L_grid_size);
	printf("%-32s%-12g\n", "T", T);
	printf("%-32s%-12i\n", "T_grid_size", T_grid_size);
	printf("%-32s%-12i\n", "iteration_max", iteration_max);
	printf("%-32s%-12g\n", "iteration_tol", iteration_tol);
	printf("%-32s%-12g\n", "trigger_equity_derivative_tol", trigger_equity_derivative_tol);
	#endif

	// Setup global variables and data structures, except the variables related to P (coupon and rhohat)
	mca_initial_setup();

	// Set up remaining variables
	setup_coupon();

	// Compute terminal equity and default flag
	terminal_equity_default(W_grid, L_grid, equity, defaulting);

	// iteration_equity has to initialized to the current equity value before every time step
	for(int i = 0; i < W_grid_size; ++i) {
		for(int j = 0; j < L_grid_size; ++j) {
			iteration_equity[i][j] = equity[i][j];
		}
	}

	#ifdef DEBUG_PRINT_TERMINAL_VALUES
	print_intermediate_result(T);
	#endif
	
	// Step throug it
	traverse_time();
}

// Function called by the main function of mca_part.exe
void mca_part(int time_steps) {
	#ifdef DEBUG_PRINT_PARAMS
	printf("%-32s%-12g\n", "r", r);
	printf("%-32s%-12g\n", "lambda", lambda);
	printf("%-32s%-12g\n", "sigma", sigma);
	printf("%-32s%-12g\n", "delta", delta);
	printf("%-32s%-12g\n", "psi", psi);
	printf("%-32s%-12g\n", "taxe", taxe);
	printf("%-32s%-12g\n", "taxi", taxi);
	printf("%-32s%-12g\n", "taxc", taxc);
	printf("%-32s%-12g\n", "P", P);
	printf("%-32s%-12g\n", "theta", theta);
	printf("%-32s%-12g\n", "W_min", W_min);
	printf("%-32s%-12g\n", "W_max", W_max);
	printf("%-32s%-12i\n", "W_grid_size", W_grid_size);
	printf("%-32s%-12g\n", "L_min", L_min);
	printf("%-32s%-12g\n", "L_max", L_max);
	printf("%-32s%-12i\n", "L_grid_size", L_grid_size);
	printf("%-32s%-12g\n", "T", T);
	printf("%-32s%-12i\n", "T_grid_size", T_grid_size);
	printf("%-32s%-12i\n", "iteration_max", iteration_max);
	printf("%-32s%-12g\n", "iteration_tol", iteration_tol);
	printf("%-32s%-12g\n", "trigger_equity_derivative_tol", trigger_equity_derivative_tol);
	#endif

	// Setup global variables and data structures, except the variables related to P (coupon and rhohat)
	mca_initial_setup();

	// Set up remaining variables
	setup_coupon();

	// Compute terminal equity and default flag
	terminal_equity_default(W_grid, L_grid, equity, defaulting);

	// iteration_equity has to initialized to the current equity value before every time step
	for(int i = 0; i < W_grid_size; ++i) {
		for(int j = 0; j < L_grid_size; ++j) {
			iteration_equity[i][j] = equity[i][j];
		}
	}

	#ifdef DEBUG_PRINT_TERMINAL_VALUES
	print_intermediate_result(T);
	#endif
	
	// Step throug it partly
	#ifdef DEBUG_PRINT_TIME
	double t;
	#endif
	for(int tau = 1; tau <= time_steps; ++tau) {
		#ifdef DEBUG_PRINT_TIME
		t = T - tau *dT;
		printf("---- Time step:%-16f to %-16f\n", t+dT, t);
		#endif
		#if defined(DEBUG_EQUITY_time) || defined(DEBUG_DEFAULTING_INVESTMENT_time) || defined(DEBUG_WRITE_time) || defined(DEBUG_GDB)
		step(tau);
		#else
		step();
		#endif
		#ifdef DEBUG_PRINT_TIME_INTERMEDIATE_RESULT
		print_intermediate_result(t);
		#endif
	}
	#ifdef DEBUG_GDB
	printf("Debug dummy: %i\n", debug_gdb_dummy);
	#endif
}

// Free memory after mca_find_EP
void clean_up_find_EP() {
	clean_up_standalone();
	free(P_grid);
	destroy_PL_grid((void **)optimal_equity);
	destroy_PL_grid((void **)optimal_cash);
	destroy_PL_grid((void **)optimal_investment);
	destroy_PL_grid((void **)optimal_defaulting);
	destroy_PL_grid((void **)optimal_equity_W);
	destroy_PL_grid((void **)optimal_equity_L);
}

// Set up global variables and data structures only relevant for mca_find_EP
void mca_find_EP_setup() {
	mca_initial_setup();
	dP = ( P_max - P_min ) / ( P_grid_size - 1);
	P_grid = create_grid(P_min, P_max, P_grid_size);

	optimal_equity = malloc(P_grid_size * sizeof(double*));
	optimal_cash = malloc(P_grid_size * sizeof(double*));
	optimal_investment = malloc(P_grid_size * sizeof(double*));
	optimal_defaulting = malloc(P_grid_size * sizeof(double*));
	optimal_equity_W = malloc(P_grid_size * sizeof(double*));
	optimal_equity_L = malloc(P_grid_size * sizeof(double*));

	for(int p = 0; p < P_grid_size; ++p) {
		optimal_equity[p] = malloc(L_grid_size * sizeof(double));
		optimal_cash[p] = malloc(L_grid_size * sizeof(double));
		optimal_investment[p] = malloc(L_grid_size * sizeof(double));
		optimal_defaulting[p] = malloc(L_grid_size * sizeof(double));
		optimal_equity_W[p] = malloc(L_grid_size * sizeof(double));
		optimal_equity_L[p] = malloc(L_grid_size * sizeof(double));
	}
}

// Perform one interation on the P grid for mca_find_EP
void mca_find_EP_iteration(int p) {
	P = P_grid[p];
	#ifdef DEBUG_PRINT_PARAMS_EP_ITERATION
	printf("Entering EP iteration with P = %-12g\n", P);
	#endif

	// Update the global variables that depend on P
	setup_coupon();
	
	// Compute terminal equity and default flag
	terminal_equity_default(W_grid, L_grid, equity, defaulting);

	// iteration_equity has to initialized to the current equity value before every time step
	for(int i = 0; i < W_grid_size; ++i) {
		for(int j = 0; j < L_grid_size; ++j) {
			iteration_equity[i][j] = equity[i][j];
		}
	}

	#ifdef DEBUG_PRINT_TERMINAL_VALUES_EP_ITERATION
	print_intermediate_result(T);
	#endif
	
	// Step throug it
	traverse_time();
}

// void find_optimal_equity_in_col(int column, int *maxi, double *maxx) {
// 	*maxi = 0;
// 	*maxx = equity[0][column] - W_grid[0];
// 	for(int i = 1; i < W_grid_size; ++i) {
// 		if(equity[i][column] > *maxx) {
// 			*maxi = i;
// 			*maxx = equity[i][column] - W_grid[i];
// 		}
// 	}
// }

// Find the optimal equity and the maximizing W (respectively its index) for a given value of L in the WL grid
int find_optimal_equity_in_col(int column) {
	int maxi = 0;
	double maxx = equity[0][column] - max(W_grid[0], 0);
	for(int i = 1; i < W_grid_size; ++i) {
		if(equity[i][column] - max(W_grid[i], 0) > maxx) {
			maxi = i;
			maxx = equity[i][column] - max(W_grid[i], 0);
		}
	}
	return maxi;
}
		
// Function called by the main function of mca_find_EP.exe
void mca_find_EP() {
	#ifdef DEBUG_FIND_EP_PRINT_PARAMS
	printf("%-32s%-12g\n", "r", r);
	printf("%-32s%-12g\n", "lambda", lambda);
	printf("%-32s%-12g\n", "sigma", sigma);
	printf("%-32s%-12g\n", "delta", delta);
	printf("%-32s%-12g\n", "psi", psi);
	printf("%-32s%-12g\n", "taxe", taxe);
	printf("%-32s%-12g\n", "taxi", taxi);
	printf("%-32s%-12g\n", "taxc", taxc);
	printf("%-32s%-12g\n", "P_min", P_min);
	printf("%-32s%-12g\n", "P_max", P_max);
	printf("%-32s%-12i\n", "P_grid_size", P_grid_size);
	printf("%-32s%-12g\n", "theta", theta);
	printf("%-32s%-12g\n", "W_min", W_min);
	printf("%-32s%-12g\n", "W_max", W_max);
	printf("%-32s%-12i\n", "W_grid_size", W_grid_size);
	printf("%-32s%-12g\n", "L_min", L_min);
	printf("%-32s%-12g\n", "L_max", L_max);
	printf("%-32s%-12i\n", "L_grid_size", L_grid_size);
	printf("%-32s%-12g\n", "T", T);
	printf("%-32s%-12i\n", "T_grid_size", T_grid_size);
	printf("%-32s%-12i\n", "iteration_max", iteration_max);
	printf("%-32s%-12g\n", "iteration_tol", iteration_tol);
	printf("%-32s%-12g\n", "trigger_equity_derivative_tol", trigger_equity_derivative_tol);
	printf("%-32s%-12g\n", "equity_cost", equity_cost);
	#endif
	
	//int maxi;
	//double maxx;
	int maxi;
	
	mca_find_EP_setup();
		
	for(int p = 0; p < P_grid_size; ++p) {
		#ifdef DEBUG_FIND_EP_PRINT_P_LOOP
		printf("Entering P iteration #%i with P = %-12g\n", p, P_min + p * dP);
		#endif
		mca_find_EP_iteration(p);
		for(int j = 0; j < L_grid_size; ++j) {
			maxi = find_optimal_equity_in_col(j);
			if(defaulting[maxi][j] == false) {
				optimal_equity[p][j] = equity[maxi][j];
				optimal_cash[p][j] = W_grid[maxi];
				optimal_investment[p][j] = investment[maxi][j];
				optimal_defaulting[p][j] = 0;
				optimal_equity_W[p][j] = equity_W[maxi][j];
				optimal_equity_L[p][j] = equity_L[maxi][j];
			} else {
				optimal_equity[p][j] = 0;
				optimal_cash[p][j] = 0;
				optimal_investment[p][j] = 0;
				optimal_defaulting[p][j] = 1;
				optimal_equity_W[p][j] = equity_W[maxi][j];
				optimal_equity_L[p][j] = equity_L[maxi][j];
			}
		}
	}
}
		
