input = readmatrix('mfcc.csv');
samples = 10;
a = ones(samples,1);
ground_truth = zeros(4*samples,4);
ground_truth(1:samples,1) = a;
ground_truth(samples+1:2*samples,2) = a;
ground_truth(2*samples+1:3*samples,3) = a;
ground_truth(3*samples+1:4*samples,4) = a;