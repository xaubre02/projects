function ukazmito(b,a,Fs)
% function ukazmito(b,a)
%
% - ukaze impulni odezvu 
% - ukaze kmit charku pro zadanou vzorkovaci frekvenci
% - ukaze poly a nuly a zjisti stabilitu 

N = 32; n = 0:N-1; imp = [1 zeros(1,N-1)]; 
h = filter(b,a,imp); 
subplot(222); stem(n,h); grid; xlabel('n'); ylabel('h[n]')
H = freqz(b,a,256); f=(0:255) / 256 * Fs / 2; 
subplot(221); plot (f,abs(H)); grid; xlabel('f'); ylabel('|H(f)|')
subplot(223); plot (f,angle(H)); grid; xlabel('f'); ylabel('arg H(f)')
subplot(224); zplane (b,a); 
p = roots(a); 
if (isempty(p) | abs(p) < 1) 
  disp('stabilni...')
else
  disp('NESTABILNI !!!')
end
