% David Pipes
% L5 Task 1 Matlab Code

% Your program will generate a PWM voltage which is applied as the filter 
% input voltage vi, and it will also determine the filter output voltage vo 
% using the forward Euler numerical integration method.

clc, clear, close all
%%                              Set-up

% Parameters
fpwm = 50000;   % fpwm is 50,000 Hz
T = 1/fpwm;     % PWM has a 20 microsecond period, 500 cycles in our t

h = 1e-7;       % time increment, 0.1 microseconds
t = 0:h:1e-2;   % time setup 100,001 values, 0 to 10,000.0 microseconds
taxis = 0:100000; % for plotting

% Initializing some variables
i = 0;
sawtooth = 1:100001;    
vi = 1:100001;          
vik = 1:100001;
vActual = 1:100001;

% State Space Forward Euler Setup
A = -10000; % -1/RC
B = 10000;  %  1/RC
C = 1;      % y = x (capacitor voltage)
x = NaN*ones(1,length(t));
x(1) = 1;

%%                              Functions

% Sawtooth Function
for j = 1:length(sawtooth)
    sawtooth(j) = i;
    i = i + 0.005;
    if (mod(j,200) == 0)
        i = 0;
    end
end

% Continuous Time vi
for j = 1:length(vi)
    vi(j) = 1.5 + sin(2*pi*500*t(j));
end

% Discrete Time vi
for j = 1:length(vi)
    if (mod(j-1,200) ==0)
        vik(j) = vi(j);
    else
        vik(j) = vik(j-1);
    end
end

% Duty Cycle Input d
d = vik/3.3;

% PWM Signal, v(t) actual
for j = 1:length(vActual)
    if (d(j) > sawtooth(j))
vActual(j) = 3.3;
    else
        vActual(j) = 0;
    end
end

% vo (filtered)
for n = 1:length(t)-1
    x(n+1) = x(n)+h*(A*x(n)+B*vActual(n));  % fwd euler, discrete vi input
end
vo = C*x;

%%                               FIGURE 1 PLOT
% plot p and d versus t
figure
subplot(2,1,1);
hold on
plot(taxis,sawtooth)
plot(taxis,d)
title('Sawtooth and d[k]')
xlabel('Time (0.1 microseconds)')
ylabel('Internal Logic')
xlim([0 600])
ylim([0 1])
%legend('Sawtooth','Duty Cycle Input')
hold off

% plot vi versus t
subplot(2,1,2); 
plot(taxis,vActual)
title('PWM Signal')
xlabel('Time (0.1 microseconds)')
ylabel('Volts (V)')
xlim([0 600])
ylim([0 3.5])


%%                               FIGURE 2 PLOT
% plot vi versus t
figure
subplot(2,1,1); 
plot(taxis,vik)
title('vi[k]')
xlabel('Time (0.1 microseconds)')
ylabel('Volts (V)')
xlim([0 100000])
ylim([0 3])

% plot vo versus t
subplot(2,1,2); 
plot(taxis,vo)
title('vo')
xlabel('Time (0.1 microseconds)')
ylabel('Volts (V)')
xlim([0 100000])
ylim([0 3])

