# Solving the SDE in C
The mca program solves the SDE in about 50 seconds (compared to 7,5 hours for the pure Matlab code). Choosing a different tolerance value of 0.01, the program runs in about 70 seconds.

I am confident that the program implements the same logic as the pure Matlab code. Doing a test run with the same parameters yields extremely similar results, except for investment, where the difference is huge (mean square difference of 17 and maximum difference of 90000).

These differences are probably due to the inherent instability of the algorithm:

Inv(i,j) = ((1 - tauc).*dEw(i,j) - dEl(i,j))./( -(1 - tauc)*psi*dEw(i,j) )

where we have cancellation in the numerator which is amplified by the division.

I also get these big differences if I change slightly the parameters for my program or the order of operations inside the code, confirming the suspicion that the investment calculation is unstable.

Changing to using the absolute value of the denominator in the investment calculation should lessen the erratic behaviour at  the edges of the grid.

# Further comment on the instability and the grid size
The problem of instability seems to appear mostly at the edges of the grid. From my test runs, it appears a good idea to discard the rightmost 10 -15 % of the L grid and the lowest 5-10% of the W grid.

# Version that finds the optimal values for E, P, and W
The program mca_standalone.exe computes the W-L-grids of results for a given P.

The program mca_find_EP.exe repeatedly performs the above procedure for different values of P lying on a grid, and for every given L chooses the cash position W such that E(W, L) – max(W, 0) is maximized.

This yields results P-L-grids of results giving the optimal values for equity, investment, cash, defaulting flag, and derivatives of equity wrt W and L, for a given pair of P and L.

The final optimization choice would then be to pick the P with maximal equity value.

The results are a bit uninteresting in the sense that is optimal no debt, and no cash holdings or very low cash holdings.

# Speed
I chose big and fine grid sizes on purpose (see the parameter files in the results below). The program takes about 14 hours.

# Plots and results for the mca_find_EP program
Similar parameters as before:
mca\Result_160819
With more extreme parameters:
mca\Result_160823

