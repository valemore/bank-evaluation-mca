mca_standalone

Use

	mca_standalone.exe params.csv equity.csv investment.csv defaulting.csv equity_W.csv equity_L.csv
	
to calculate initial values of equity, investment, default choice, as well as derivative of equity with respect to W and L using the MCA method with parameters given by params.csv.

The results are saved into the file names given as remaining arguments.


mca_find_EPq

Use

	mca_find_EP.exe params.csv optimal_E.csv optimal_W.csv optimal_I.csv optimal_D.csv optimal_equity_W.csv optimal_equity_L.csv
	
to calculate initial values of equity, cash, investment, default choice, as well as derivative of equity with respect to W and L using the MCA method with parameters given by params.csv.

The results are saved into the file names given as remaining arguments.

The outputs are two-dimensional grids of values, differing in the principal of the external debt P and the size of the loan portfolio L. For each point in the grid, the same procedure as for the above mca_standalone program is performed, and the size of the cash position is chosen such that it maximizes Equity - max(Cash, 0).
