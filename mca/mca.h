// Valerio Morelli, August 2016

#ifndef MCA_H
#define MCA_H

#include <stdbool.h>

// Functions exposed to the main executables

void mca_standalone();
void mca_part();
void mca_find_EP();
void clean_up_standalone();
void clean_up_find_EP();

// GLOBAL VARIABLES COMPRISE PARAMETERS AND DERIVED VALUES
double r;																							// Risk-free rate
double lambda;																						// Opportunity cost for cash
double sigma;																						// Volatility for the loan portfolio
double delta;																						// Depreciation for the loan portfolio
double psi;																							// Investment adjustment cost

double taxe;																						// Tax on equity income
double taxi;																						// Tax on interest income
double taxc;																						// Tax on corporate income

double P;																							// Principal of external bank debt
double P_min;																						// Only used for find_EP
double P_max;																						// Only used for find_EP
int P_grid_size;																					// Only used for find_EP
int dP;																								// Only used for find_EP
double equity_cost;																					// Only used for find_EP
double premium;																						// Premium for coupon on external bank debt over r
double coupon;																						// Coupon on external bank debt = (r + premium) * P;
double rhohat;																						// Discount rate = (1 - taxi) * r
double theta;																						// Haircut on loan portfolio when bank has negative cash position with CB

double W_min;																						// W refers to the cash holdings of the bank with the CB
double W_max;
int W_grid_size;
double dW;
double dL;																							// L refers to the loan portfolio of the bank
double L_min;
double L_max;
int L_grid_size;

double T;																							// T is the terminal time
int T_grid_size;
double dT;

int iteration_max;																					// Maximum number of iterations to perform in one time step
double iteration_tol;																				// When equity value changes less than the iteration tolerance, complete the time step
double trigger_equity_derivative_tol;																// Used in update_default_investment funciton in mca.c

// Cash and Loan grids
double *W_grid, *L_grid;

// Principal grid (for find_EP)
double *P_grid;

// Result arrays for mca

// These double pointers store the results
double **equity, **investment, **equity_W, **equity_L;
bool **defaulting;

// We need to keep track of the equity values computed in the the preceding time step as well.
double **new_equity;
// We also need to keep track of the equity value of the preceding value iteration step
double **iteration_equity;

// Result arrays for mca_find_EP //
double **optimal_equity, **optimal_cash, **optimal_investment, **optimal_defaulting, **optimal_equity_W, **optimal_equity_L;

#endif
