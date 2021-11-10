function pos_free = network_loc(N, E, pos_anchor, rho)
L = length(E);
K = length(pos_anchor);
M = N-K;

% Generate points
x_k = rand(M, 2);
p = [x_k; pos_anchor]; 

% STEP 1
f_k = zeros(L, 1);
for k = 1:L
    ind_i = E(k,1); 
    ind_j = E(k,2);
    f_k(k) = sqrt( ( p(ind_i, 1) - p(ind_j, 1) ).^2 + ( p(ind_i, 2) - p(ind_j, 2) ).^2 ) - rho(k);
end

A = zeros(L, 2 * (N - K));
for k = 1:L
    ind_i = E(k,1); 
    ind_j = E(k,2);
    denom = sqrt( ( p(ind_i, 1) - p(ind_j, 1) ).^2 + ( p(ind_i, 2) - p(ind_j, 2) ).^2 );
    if (E(k, 1) < M+1)
        A(k,1) = ( p(ind_i, 1) - p(ind_j, 1) ) / denom;
        A(k,2) = ( p(ind_i, 2) - p(ind_j, 2) ) / denom;
    end
    if (E(k, 2) < M+1)
        A(k,3) = -1 * ( p(ind_i, 1) - p(ind_j, 1) ) / denom;
        A(k,4) = -1 * ( p(ind_i, 2) - p(ind_j, 2) ) / denom;
    end
end

% STEP 2
lambda = 0.25;
del_x = [A;sqrt(lambda).*eye(2*(N-K), 2*(N-K))] \ [f_k;zeros(2*(N-K),1)];
x = [x_k;x_k] - del_x;
x_hat = x(1:N-K, :);

% STEP 3
loop = true;
Beta1 = 0.8; Beta2 = 2.0; 
tracker = [];
while loop        
    % Calculate cost functions
    p_hat = [x_hat(1:N-K, :); pos_anchor];
    p_k = [x_k; pos_anchor];
    f_hat = zeros(L, 1);
    f_k = zeros(L, 1);
    for k = 1:L
        ind_i = E(k,1);
        ind_j = E(k,2);
        f_hat(k) = sqrt( ( p_hat(ind_i, 1) - p_hat(ind_j, 1) ).^2 + ( p_hat(ind_i, 2) - p_hat(ind_j, 2) ).^2 ) - rho(k);
    end
    for k = 1:L
        ind_i = E(k,1);
        ind_j = E(k,2);
        f_k(k) = sqrt( ( p_k(ind_i, 1) - p_k(ind_j, 1) ).^2 + ( p_k(ind_i, 2) - p_k(ind_j, 2) ).^2 ) - rho(k);
    end
    f_1 = norm(f_hat).^2;
    f_2 = norm(f_k).^2;
    
    % Calculate Df
    A = zeros(L, 2 * (N - K));
    for k = 1:L
        ind_i = E(k,1);
        ind_j = E(k,2);
        denom = sqrt( ( p_k(ind_i, 1) - p_k(ind_j, 1) ).^2 + ( p_k(ind_i, 2) - p_k(ind_j, 2) ).^2 );
        if (E(k, 1) < M+1)
            A(k,1) = ( p_k(ind_i, 1) - p_k(ind_j, 1) ) / denom;
            A(k,2) = ( p_k(ind_i, 2) - p_k(ind_j, 2) ) / denom;
        end
        if (E(k, 2) < M+1)
            A(k,3) = -1 * ( p_k(ind_i, 1) - p_k(ind_j, 1) ) / denom;
            A(k,4) = -1 * ( p_k(ind_i, 2) - p_k(ind_j, 2) ) / denom;
        end
    end
    
    % Update x and lambda
    if (f_1 < f_2)
        x_k = x_hat(1:N-K, :);
        lambda = Beta1 * lambda;
    else
        lambda = Beta2 * lambda;
    end
    
    % Compute next iteration
    del_x = [A;sqrt(lambda).*eye(2*(N-K), 2*(N-K))] \ [f_k(1:L);zeros(2*(N-K),1)];
    x_hat = [x_k;x_k] - del_x;
    
    % Optimal condition
    norm_del_g = norm(2.*A.'*f_k(1:L));
    tracker = [tracker; norm_del_g];
    if (norm_del_g < 10e-5)
        loop = false;
    end
end

pos_free = x_k;
end