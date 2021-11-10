% Read in file
fftFile = fopen('out.txt', 'r');
fftOut  = fscanf(fftFile, '%f', [64 64]);
fclose(fftFile);

% Mesh plot components
X = 48e3/64*(1:64); %Hz
Y = 48e3/64*(1:64); %Hz
Z = fftshift(fftOut);
mesh(X, Y, Z);