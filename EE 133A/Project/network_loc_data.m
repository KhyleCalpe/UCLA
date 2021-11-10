function [E, pos, K] = network_loc_data(N, R);

% [E, pos, K] = network_loc_data(N, R);

% Input arguments.
%
% N:     Number of nodes.  The free nodes 1,..., N-9 are randomly placed 
%        in the square [0,1] x [0,1].  The anchor nodes N-8, ..., N are at 
%        (0,0), (0.5,0), (1,0), (0,0.5), (0.5,0.5), (1,0.5), 
%        (0,1), (0.5,1), (1,1),
% R:     Radio range.  Two nodes are adjacent if their distance is less 
%        than or equal to R.
% 
% Output arguments.
%
% E:     Lx2 array.  Edge k is between nodes E(k,1) and E(k,2).
% pos:   Nx2 array.  Node k is at position (pos(k,1), pos(k,2)).
% K:     Number of anchor points (K=9).

K = 9;           
pos = [ rand(N-K, 2); 
        0.0, 0.0;
        0.0, 0.5;
        0.0, 1.0;
        0.5, 0.0;
        0.5, 0.5;
        0.5, 1.0;
        1.0, 0.0;
        1.0, 0.5;
        1.0, 1.0;
];

E = [];   
for k = 1:N-K
    for j = k+1:N
        if ( norm(pos(k,:) - pos(j,:)) <= R)
            E = [E; k, j];
        end;
    end
end;
