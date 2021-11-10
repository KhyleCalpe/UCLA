clear;
clc;
N = 50; R = 0.4; s = 0.05;
[E, pos, K] = network_loc_data(N, R);
pos_anchor = pos(N-K+1:N, :);
L = size(E,1);
d = sqrt(sum( (pos(E(:,1),:) - pos(E(:,2),:)).^2, 2));
rho = (1 + s*randn(L,1)) .* d;
pos_free = network_loc(N, E, pos_anchor, rho); % your function