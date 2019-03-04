clc
clear all
close all

wd_matlab = 'C:/Users/morellv/00J/bank-evaluation-mca/Matlab'

cd(wd_matlab)

my_E = csvread('../mca/Equity.csv');

diff_E = (my_E - Equitynew) .^ 2;

mean(diff_E(:))
max(diff_E(:))


my_I = csvread('../mca/Investment.csv');

diff_I = (my_I - Inv) .^ 2;

mean(diff_I(:))
max(diff_I(:))


indices_W = 1:70;
indices_L = 1:180;

my_I2 = my_I(indices_W, indices_L);
Inv2 = Inv(indices_W, indices_L);

diff_I2 = (my_I2 - Inv2) .^ 2;

mean(diff_I2(:))
max(diff_I2(:))



my_D = csvread('../mca/Defaulting.csv');

diff_D = (my_D - Default) .^ 2;

mean(diff_D(:))
max(diff_D(:))


my_Ew = csvread('../mca/equity_W.csv');

diff_Ew = (my_Ew - dEw) .^ 2;

mean(diff_Ew(:))
max(diff_Ew(:))


my_El = csvread('../mca/equity_L.csv');

diff_El = (my_El - dEl) .^ 2;

mean(diff_El(:))
max(diff_El(:))