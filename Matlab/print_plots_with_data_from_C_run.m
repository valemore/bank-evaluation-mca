clc
clear all
close all

wd_matlab = 'C:/Users/morellv/00J/bank-evaluation-mca/Matlab'

cd(wd_matlab)

% Parameters
r = 0.05;                   % risk free rate
lambda = 0.01;              % opportunity cost cash
sigma = 0.119;              % volatility loan portfolio
delta = 0.025;              % depreciation loan portfolio
psi = 1.5;                  % adjustment cost
taue = 0;                   % taxes equity income 
taui = 0;                   % taxes interest income 
tauc = 0;                   % taxes corporate income 
P = 50;                     % external Debt principal 
Coupon = r*P;               % coupon 
Rhohat = (1 - taui)*r;      % discount rate
Theta = 0.08 ;              % CB haircut

% L loan grid
NL = 199;
Lmin = 0;
Lmax = 298.5;
dLstep = (Lmax - Lmin)./NL;
LLmax = Lmax + dLstep;
L = [Lmin:dLstep:Lmax,LLmax]; 
dL = [dLstep.*ones(1,NL+1),LLmax - Lmax];
leL = length(L);

% W cash grid
minW = 0;
Wbar = 74;
dWstep = 1;
Wmax = Wbar + dWstep;

Wl = [minW:dWstep:Wbar,Wmax];
dW = [dWstep.*ones(1,length(Wl(1:end-1))),Wmax - Wbar];
leW = length(Wl);

Equityguess = zeros(leW,leL);
DefaultT = zeros(leW,leL);

% Terminal boundary
for i = 1:leW
    
    for j = 1:leL
        
        if Wl(i) >= 0
            
            Equityguess(i,j) = max( Wl(i)*(1 - taue) - P + L(j), 0);

            if Equityguess(i,j) == 0
            
                DefaultT(i,j) = 1;
           
            end
            
        else
            
            Equityguess(i,j) = max( Wl(i) - P + (1 - Theta)*L(j), 0);

            if Equityguess(i,j) == 0
            
                DefaultT(i,j) = 1;
           
            end
            
        end    
        
    end
    
end

% Time
T = 40;
tau = 0.0005;
stept = T/tau;
tle = stept;

%%% END PARAMETERS ETC

Default = csvread('../mca/Defaulting.csv');
Equitynew = csvread('../mca/Equity.csv');
Inv = csvread( '../mca/Investment.csv');
dEw = csvread('../mca/equity_W.csv');
dEl = csvread('../mca/equity_L.csv');

% maDefaultT = defaultarea(leL,leW,L,Wl,DefaultT);
% maDefault = defaultarea(leL,leW,L,Wl,Default);

%close all

% figure(1)
% plot(maDefaultT(:,1),maDefaultT(:,2),'-',...
%      maDefault(:,1),maDefault(:,2),'--','LineWidth',2)
% xlabel('L - loan stock', 'FontSize', 15)
% ylabel('W - Cash', 'FontSize', 15)

figure(2)
plot(L,Equitynew(21,:),'-',L,Equitynew(61,:),'-.','LineWidth',2)
xlabel('L - loan stock', 'FontSize', 15)
ylabel('Equity value', 'FontSize', 15)
legend('Cash = 20', 'Cash = 60','Location','Best')

figure(3)
plot(Wl,Equitynew(:,30),'-',Wl,Equitynew(:,130),'-.','LineWidth',2)
xlabel('W - cash', 'FontSize', 15)
ylabel('Equity value', 'FontSize', 15)
legend('Loan stock = 43.5', 'Loan stock = 193.5','Location','Best')

figure(4)
plot(L(1:100),Inv(21,1:100),'-',L(1:100),Inv(61,1:100),'-.','LineWidth',2)
xlabel('L - loan stock', 'FontSize', 15)
ylabel('Investment in loans', 'FontSize', 15)
legend('Cash = 20', 'Cash = 60','Location','Best')

figure(5)
plot(Wl,Inv(:,30),'-',Wl,Inv(:,80),'-.'....
    ,Wl,Inv(:,125),'--','LineWidth',2)
xlabel('W - cash', 'FontSize', 15)
ylabel('Investment in loans', 'FontSize', 15)
legend('Loan stock = 43.5', 'Loan stock = 118.5', 'Loan stock = 186','Location','Best')

figure(6)
plot(L,dEl(21,:),'-',L,dEl(61,:),'-.','LineWidth',2)
xlabel('L - loan stock', 'FontSize', 15)
ylabel('El', 'FontSize', 15)
legend('Cash = 20', 'Cash = 60','Location','Best')

figure(7)
plot(Wl,dEw(:,30),'-',Wl,dEw(:,130),'-.','LineWidth',2)
xlabel('W - cash', 'FontSize', 15)
ylabel('Ew', 'FontSize', 15)
legend('Loan stock = 43.5', 'Loan stock = 193.5','Location','Best')

%print(1,'Fig1.png','-dpng','-r600');
print(2,'Fig2.png','-dpng','-r600');
print(3,'Fig3.png','-dpng','-r600');
print(4,'Fig4.png','-dpng','-r600');
print(5,'Fig5.png','-dpng','-r600');
print(6,'Fig6.png','-dpng','-r600');
print(7,'Fig7.png','-dpng','-r600');


