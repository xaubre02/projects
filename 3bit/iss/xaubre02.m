% Priklad 1
% ###################################
[x, Fs] = audioread ('xaubre02.wav');
t = length(x) / Fs;
N = Fs * t;

% Fs = 16000
%  N = 16000
%  t = 1s

% Priklad 2
% ###################################
X = fft(x);
k = 1:(N/2);
Fn= k/N;
f = Fn * Fs;
plot (f, abs(X(1:N/2)));

% Priklad 3
% ###################################
max_X = max(abs(X));
% Maximum je 1697.5 Hz na 340. indexu

% Priklad 4
% ###################################
b = [0.2324 -0.4112 0.2324];
a = [1 0.2289 0.4662];
ukazmito(b, a, Fs);
%stabilni

% Priklad 5
% ###################################
% horní porpusť

% Priklad 6
% ###################################
y = filter(b,a,x);
Y = fft(y);
plot (f, abs(Y(1:N/2)));

% Priklad 7
% ###################################
max_Y = max(abs(Y));
% Maximum je 773.4216 Hz na 4473. indexu

% Priklad 8
% ###################################

% Priklad 9
% ###################################
l=length(x);
Rv=xcorr(x,'biased'); % vychýlený odhad
k=-50:50;
plot(k,Rv(l-50:l+50));

% Priklad 10
% ###################################
Rv(l+10);
% Hodnota je 0.0093
