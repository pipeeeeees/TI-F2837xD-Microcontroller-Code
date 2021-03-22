% David Pipes
% H9

clc, clear, close all
%%

M = csvread('hello2 2.csv');

h = 0.01; 
t = 0:h:9.99;
tt = 0:length(M(:,2))-1;

A = [-0.833, 1, 0, 0;
     -4.584, 0, 1, 0;
       0, 0, 0, 1;
       0, 0, 0, 0];
B = [0;0;0.417;2.292];
C = [1,0,0,0];

u = ones(1,length(t));
for n = 1:length(t)-1
    if(n<628)
        u(n) = 3*sin(t(n)) - sin(3*t(n));
    else
        u(n) = 0;
    end
end

x = NaN*ones(4,length(t));
x(:,1) = 0;


for n = 1:length(t)-1
    x(:,n+1) = x(:,n)+h*(A*x(:,n)+B*u(:,n));
end

ymodeled = C*x;

ymeasured = (M(:,2))';

QOF = 100 * (1 - norm(ymeasured-ymodeled)/norm(ymeasured-mean(ymodeled)))

subplot(211), plot(t,ymodeled)
subplot(212), plot(tt,M(:,2))




