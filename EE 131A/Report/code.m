%% Read/store files
load data.txt;
load data_2.txt;
load data_3.txt;
f_path = 'D:\UCLA\Courses\EE 131A\Project\Plots';

%% Section 1

% Indices of missing and available data
K_miss     = isnan(data); 
K_avail    = ~isnan(data);

% Filtered data
data_miss  = data(K_miss);
data_avail = data(K_avail);

%% Section 1.b

% Estimate sample mean
N_cont = 10:10:60000;
N_disc = [10,20,50,100,200,300,500,1000,2000,10000,20000,30000,60000];
mu_N_c = sample_mean(data_avail, N_cont);
mu_N_d = sample_mean(data_avail, N_disc);

% mu_N vs N plot
hold on;
plot(N_cont, mu_N_c);
scatter(N_disc, mu_N_d);
title('Estimated Sample Mean ($\hat{\mu}_N$) vs Sample Count (N)', 'Interpreter', 'Latex');
xlabel('sample count (N)', 'Interpreter', 'Latex');
ylabel('sample mean ($\hat{\mu}_N$)', 'Interpreter', 'Latex');
ylim([19.8 20.4])
print(gcf, fullfile(f_path, '01_b'), '-dpng', '-r300');

%% Section 1.c

% Estimate sample mean accuracy
square_d   = (data_avail(:) - mu_N_d).^2;
square_c   = (data_avail(:) - mu_N_c).^2;
acc_d      = zeros(1, length(mu_N_d));
acc_c      = zeros(1, length(mu_N_c));

for mu_N_index = 1:length(mu_N_d)
    acc_d(mu_N_index) = sum(square_d(:,mu_N_index))/length(data_avail);
end

for mu_N_index = 1:length(mu_N_c)
    acc_c(mu_N_index) = sum(square_c(:,mu_N_index))/length(data_avail);
end

% A_N vs N plot
hold on;
plot(N_cont, acc_c);
scatter(N_disc, acc_d);
title('Estimated Sample Mean Accuracy ($\hat{A}_N$) vs Sample Count (N)', 'Interpreter', 'Latex');
xlabel('sample count (N)', 'Interpreter', 'Latex');
ylabel('sample mean accuracy ($\hat{A}_N$)', 'Interpreter', 'Latex');
xlim([0 60000]);
ylim([99.18 99.38]);
print(gcf, fullfile(f_path, '01_c'), '-dpng', '-r300');

%% Section 2.a

% Initializations
n        = [1,2,3,4,5,10,20,30];        % samples
samples  = 10000;
M_n      = zeros(samples, length(n));   % RVs
mean_sum = zeros([1 samples]);
min_val  = zeros([1 length(n)]);        % subplots
max_val  = zeros([1 length(n)]);
sample_x = zeros(100, length(n));
mean_n   = zeros([1 length(n)]);
sd_n     = zeros([1 length(n)]);

% Generate mean for n RVs
for mean_ind = 1:length(n)
    for sum_ind = 1:n(mean_ind)
        mean_sum = mean_sum + 4*rand([1 samples])+2;
    end
    M_n(:, mean_ind) = mean_sum./n(mean_ind);
    % Reset for next iteration
    mean_sum = zeros([1 samples]);
end

% Close all open figures
close all;

% PDF & CDF subplots
for plot_ind = 1:length(n)
    % Parameters
    min_val(plot_ind)     = min(M_n(:, plot_ind));
    max_val(plot_ind)     = max(M_n(:, plot_ind));
    sample_x(:, plot_ind) = linspace(min_val(plot_ind), max_val(plot_ind));
    mean_n(plot_ind)      = mean(M_n(:, plot_ind));
    sd_n(plot_ind)        = std(M_n(:, plot_ind));
    
    % Formatting (PDF)
    figure(1);
    subplot(4, 2, plot_ind);
    title_string = strcat({'$n$ = '}, num2str(n(plot_ind)));
    title(title_string, 'Interpreter', 'Latex');
    xlabel('sample value', 'Interpreter', 'Latex');
    ylabel('density', 'Interpreter', 'Latex');
    
    % Plot (PDF)
    hold on;
    histogram(M_n(:, plot_ind), 'BinWidth', 1/(n(plot_ind)+1), 'Normalization', 'pdf');
    y_pdf = (1/(sd_n(plot_ind).*sqrt(2*pi))).*exp(-(1/2)*((sample_x(:, plot_ind)-mean_n(plot_ind))./sd_n(plot_ind)).^2);
    plot(sample_x(:, plot_ind), y_pdf);
    
    % Formatting (CDF)
    figure(2);
    subplot(4, 2, plot_ind);
    title_string = strcat({'$n$ = '}, num2str(n(plot_ind)));
    title(title_string, 'Interpreter', 'Latex');
    xlabel('sample value', 'Interpreter', 'Latex');
    ylabel('density', 'Interpreter', 'Latex');
    
    % Plot (CDF)
    hold on;
    histogram(M_n(:, plot_ind), 'BinWidth', 1/(n(plot_ind)+1), 'Normalization', 'cdf');
    y_cdf = cdf('Normal', sample_x(:, plot_ind), mean_n(plot_ind), sd_n(plot_ind));
    plot(sample_x(:, plot_ind), y_cdf);
end

% Save subplots
figure(1);
sgtitle('Mean Distribution', 'Interpreter', 'Latex');
print(gcf, fullfile(f_path, '02_a_pdf'), '-dpng', '-r300');

figure(2);
sgtitle('Cumulative Mean Distribution', 'Interpreter', 'Latex');
print(gcf, fullfile(f_path, '02_a_cdf'), '-dpng', '-r300');

%% Section 2.c

% PDF & CDF subplots
for plot_ind = 1:length(n) 
    % Multivariate Gaussian
    mv_GRV = mvnrnd(mean_n(plot_ind), sd_n(plot_ind), length(sample_x(:, plot_ind)));
    
    % Formatting (PDF)
    figure(1);
    subplot(3, 3, plot_ind);
    title_string = strcat({'$n$ = '}, num2str(n(plot_ind)));
    title(title_string, 'Interpreter', 'Latex');
    xlabel('sample value', 'Interpreter', 'Latex');
    ylabel('density', 'Interpreter', 'Latex');
    
    % Plot (PDF)
    hold on;
    histogram(M_n(:, plot_ind), 'BinWidth', 1/(n(plot_ind)+1), 'Normalization', 'pdf');
    histogram(mv_GRV, 'BinWidth', 1/(n(plot_ind)+1), 'Normalization', 'pdf');
    
    % Formatting (CDF)
    figure(2);
    subplot(3, 3, plot_ind);
    title_string = strcat({'$n$ = '}, num2str(n(plot_ind)));
    title(title_string, 'Interpreter', 'Latex');
    xlabel('sample value', 'Interpreter', 'Latex');
    ylabel('density', 'Interpreter', 'Latex');
    
    % Plot (CDF)
    hold on;
    histogram(M_n(:, plot_ind), 'BinWidth', 1/(n(plot_ind)+1), 'Normalization', 'cdf');
    histogram(mv_GRV, 'BinWidth', 1/(n(plot_ind)+1), 'Normalization', 'cdf');
end

% Save subplots
figure(1);
sgtitle('Mean Distribution', 'Interpreter', 'Latex');
subplot(3,3,9);
plot(0,0,  0,0);
axis off;
legend('M_n', 'Gaussian');
print(gcf, fullfile(f_path, '02_c_pdf'), '-dpng', '-r300'); 

figure(2);
sgtitle('Cumulative Mean Distribution', 'Interpreter', 'Latex');
subplot(3,3,9);
plot(0,0,  0,0);
axis off;
legend('M_n', 'Gaussian');
print(gcf, fullfile(f_path, '02_c_cdf'), '-dpng', '-r300');

%% Section 2.d.a

% Initializations
n        = [1,2,3,4,5,10,20,30];        % samples
tosses   = [2 2 4 4 1 3 5];
samples  = 10000;
M_n      = zeros(samples, length(n));   % RVs
mean_sum = zeros([1 samples]);
min_val  = zeros([1 length(n)]);        % subplots
max_val  = zeros([1 length(n)]);
sample_x = zeros(100, length(n));
mean_n   = zeros([1 length(n)]);
sd_n     = zeros([1 length(n)]);

% Generate mean for n biased RVs
for mean_ind = 1:length(n)
    for sum_ind = 1:n(mean_ind)
        toss = tosses(randi(length(tosses), [1 samples]));
        mean_sum = mean_sum + toss;
    end
    M_n(:, mean_ind) = mean_sum./n(mean_ind);
    % Reset for next iteration
    mean_sum = zeros([1 samples]);
end

% Close all open figures
close all;

% PDF & CDF subplots
for plot_ind = 1:length(n)
    % Parameters
    min_val(plot_ind)     = min(M_n(:, plot_ind));
    max_val(plot_ind)     = max(M_n(:, plot_ind));
    sample_x(:, plot_ind) = linspace(min_val(plot_ind), max_val(plot_ind));
    mean_n(plot_ind)      = mean(M_n(:, plot_ind));
    sd_n(plot_ind)        = std(M_n(:, plot_ind));
    
    % Formatting (PDF)
    figure(1);
    subplot(4, 2, plot_ind);
    title_string = strcat({'$n$ = '}, num2str(n(plot_ind)));
    title(title_string, 'Interpreter', 'Latex');
    xlabel('sample value', 'Interpreter', 'Latex');
    ylabel('density', 'Interpreter', 'Latex');
    
    % Plot (PDF)
    hold on;
    histogram(M_n(:, plot_ind), 'BinWidth', 1/(n(plot_ind)+1), 'Normalization', 'pdf');
    y_pdf = (1/(sd_n(plot_ind).*sqrt(2*pi))).*exp(-(1/2)*((sample_x(:, plot_ind)-mean_n(plot_ind))./sd_n(plot_ind)).^2);
    plot(sample_x(:, plot_ind), y_pdf);
    
    % Formatting (CDF)
    figure(2);
    subplot(4, 2, plot_ind);
    title_string = strcat({'$n$ = '}, num2str(n(plot_ind)));
    title(title_string, 'Interpreter', 'Latex');
    xlabel('sample value', 'Interpreter', 'Latex');
    ylabel('density', 'Interpreter', 'Latex');
    
    % Plot (CDF)
    hold on;
    histogram(M_n(:, plot_ind), 'BinWidth', 1/(n(plot_ind)+1), 'Normalization', 'cdf');
    y_cdf = cdf('Normal', sample_x(:, plot_ind), mean_n(plot_ind), sd_n(plot_ind));
    plot(sample_x(:, plot_ind), y_cdf);
end

% Save subplots
figure(1);
sgtitle('Mean Distribution', 'Interpreter', 'Latex');
print(gcf, fullfile(f_path, '02_d_a_pdf'), '-dpng', '-r300');

figure(2);
sgtitle('Cumulative Mean Distribution', 'Interpreter', 'Latex');
print(gcf, fullfile(f_path, '02_d_a_cdf'), '-dpng', '-r300');

%% Section 2.d.c

% PDF & CDF subplots
for plot_ind = 1:length(n) 
    % Multivariate Gaussian
    mv_GRV = mvnrnd(mean_n(plot_ind), sd_n(plot_ind), length(sample_x(:, plot_ind)));
    
    % Formatting (PDF)
    figure(1);
    subplot(3, 3, plot_ind);
    title_string = strcat({'$n$ = '}, num2str(n(plot_ind)));
    title(title_string, 'Interpreter', 'Latex');
    xlabel('sample value', 'Interpreter', 'Latex');
    ylabel('density', 'Interpreter', 'Latex');
    
    % Plot (PDF)
    hold on;
    histogram(M_n(:, plot_ind), 'BinWidth', 1/(n(plot_ind)+1), 'Normalization', 'pdf');
    histogram(mv_GRV, 'BinWidth', 1/(n(plot_ind)+1), 'Normalization', 'pdf');
    
    % Formatting (CDF)
    figure(2);
    subplot(3, 3, plot_ind);
    title_string = strcat({'$n$ = '}, num2str(n(plot_ind)));
    title(title_string, 'Interpreter', 'Latex');
    xlabel('sample value', 'Interpreter', 'Latex');
    ylabel('density', 'Interpreter', 'Latex');
    
    % Plot (CDF)
    hold on;
    histogram(M_n(:, plot_ind), 'BinWidth', 1/(n(plot_ind)+1), 'Normalization', 'cdf');
    histogram(mv_GRV, 'BinWidth', 1/(n(plot_ind)+1), 'Normalization', 'cdf');
end

% Save subplots
figure(1);
sgtitle('Mean Distribution', 'Interpreter', 'Latex');
subplot(3,3,9);
plot(0,0,  0,0);
axis off;
legend('M_n', 'Gaussian');
print(gcf, fullfile(f_path, '02_d_c_pdf'), '-dpng', '-r300'); 

figure(2);
sgtitle('Cumulative Mean Distribution', 'Interpreter', 'Latex');
subplot(3,3,9);
plot(0,0,  0,0);
axis off;
legend('M_n', 'Gaussian');
print(gcf, fullfile(f_path, '02_d_c_cdf'), '-dpng', '-r300');

%% Section 3.b

% Parameters
bd_mu_0     = [9;10];
bd_mu_1     = [6;7];
bd_sigma    = [1.15,0.1;0.1,0.5];

% Classification inequality
b_class_vec   = 2*(bd_mu_1-bd_mu_0).'*bd_sigma^(-1)*data_2.';
b_class_const = bd_mu_0.'*bd_sigma^(-1)*bd_mu_0-bd_mu_1.'*bd_sigma^(-1)*bd_mu_1;
b_class_ans   = b_class_vec+b_class_const;

% Proportion of samples from class 0
mean(b_class_ans >= 0); % 0.5017

% Scatter plot data
b_class_0_ind = b_class_ans >= 0;
b_class_1_ind = b_class_ans <  0;
b_class_0_vec = data_2(b_class_0_ind, :);
b_class_1_vec = data_2(b_class_1_ind, :);

% Scatter plots
hold on;
scatter(b_class_0_vec(:, 1), b_class_0_vec(:, 2), 'r');
scatter(b_class_1_vec(:, 1), b_class_1_vec(:, 2), 'b');

% Contour
ineq_f_b = @(x,y) 2*(bd_mu_1-bd_mu_0).'*bd_sigma^(-1)*[x;y]+b_class_const;
fcontour(ineq_f_b, [0 14 4 13], 'LevelList', 0);

% Formatting
title('Petal Length vs Petal Width', 'Interpreter', 'Latex');
xlabel('Petal Length', 'Interpreter', 'Latex');
ylabel('Petal Width', 'Interpreter', 'Latex');
legend('class 0', 'class 1', '$\vec{b}^T\cdot\vec{x}+a=0$', 'Interpreter', 'Latex', 'Location', 'Best');
print(gcf, fullfile(f_path, '03_b_scatter'), '-dpng', '-r300');

%% Section 3.d

% Parameter
d_prob = 0.05;

% Classification inequality
d_class_vec   = 2*(bd_mu_1-bd_mu_0).'*bd_sigma^(-1)*data_2.';
d_class_const = (bd_mu_0.'*bd_sigma^(-1)*bd_mu_0-bd_mu_1.'*bd_sigma^(-1)*bd_mu_1)+log((1-d_prob)/d_prob);
d_class_ans   = d_class_vec+d_class_const;

% Proportion of samples from class 0
mean(d_class_ans >= 0); % 0.5085

% Scatter plot data
d_class_0_ind = d_class_ans >= 0;
d_class_1_ind = d_class_ans <  0;
d_class_0_vec = data_2(d_class_0_ind, :);
d_class_1_vec = data_2(d_class_1_ind, :);

% Scatter plots
hold on;
scatter(d_class_0_vec(:, 1), d_class_0_vec(:, 2), 'r');
scatter(d_class_1_vec(:, 1), d_class_1_vec(:, 2), 'b');

% Contour
ineq_f_d = @(x,y) 2*(bd_mu_1-bd_mu_0).'*bd_sigma^(-1)*[x;y]+d_class_const;
fcontour(ineq_f_d, [0 14 4 13], 'LevelList', 0);

% Formatting
title('Petal Length vs Petal Width', 'Interpreter', 'Latex');
xlabel('Petal Length', 'Interpreter', 'Latex');
ylabel('Petal Width', 'Interpreter', 'Latex');
legend('class 0', 'class 1', '$\vec{b}^T\cdot\vec{x}+a=0$', 'Interpreter', 'Latex', 'Location', 'Best');
print(gcf, fullfile(f_path, '03_d_scatter'), '-dpng', '-r300');

%% Section 3.f

% Parameter
f_mu_0    = [9;10]; 
f_mu_1    = [6;7];
f_sigma_0 = [1.15,0.1;0.1,0.5];
f_sigma_1 = [0.2,0.3;0.3,2];
f_prob    = 0.5;

% Initializations
f_class_ans = zeros(length(data_3), 1);

% Classification inequality
f_class_lin_vec  = 2*(f_mu_1.'*f_sigma_1^(-1)-f_mu_0.'*f_sigma_0^(-1))*data_3.';
f_class_det      = det(f_sigma_0)^(-1/2)*det(f_sigma_1)^(-1/2);
f_class_const    = (f_mu_0.'*f_sigma_0^(-1)*f_mu_0-f_mu_1.'*f_sigma_1^(-1)*f_mu_1)...
                    +log(((1-f_prob)/f_prob)*f_class_det);

% Inequality vector                
for sample_ind = 1:length(data_3)
    f_class_quad_vec = data_3(sample_ind,:)*(f_sigma_0^(-1)-f_sigma_1^(-1))*data_3(sample_ind,:).';
    f_class_ans(sample_ind) = f_class_quad_vec+f_class_lin_vec(sample_ind)+f_class_const;
end
               
% Proportion of samples from class 0
mean(f_class_ans >= 0); % 0.5051

% Scatter plot data
f_class_0_ind = f_class_ans >= 0;
f_class_1_ind = f_class_ans <  0;
f_class_0_vec = data_3(f_class_0_ind, :);
f_class_1_vec = data_3(f_class_1_ind, :);

% Scatter plots
hold on;
scatter(f_class_0_vec(:, 1), f_class_0_vec(:, 2), 'r');
scatter(f_class_1_vec(:, 1), f_class_1_vec(:, 2), 'b');

% Contour
ineq_f_d = @(x,y) [x,y]*(f_sigma_0^(-1)-f_sigma_1^(-1))*[x;y]...
    +2*(f_mu_1.'*f_sigma_1^(-1)-f_mu_0.'*f_sigma_0^(-1))*[x;y]+f_class_const;
fcontour(ineq_f_d, [0 14 4 13], 'LevelList', 0);

% Formatting
title('Petal Length vs Petal Width', 'Interpreter', 'Latex');
xlabel('Petal Length', 'Interpreter', 'Latex');
ylabel('Petal Width', 'Interpreter', 'Latex');
legend('class 0', 'class 1', '$\vec{x}^TC\vec{x}+\vec{b}^T\cdot\vec{x}+a=0$', 'Interpreter', 'Latex', 'Location', 'Best');
print(gcf, fullfile(f_path, '03_f_scatter'), '-dpng', '-r300');

%% Functions

% sample mean estimator
function mu_N = sample_mean(data, samples)
    mu_N = zeros(1, length(samples));
    for i = 1:length(samples)
        mu_N(i) = mean(data(1:samples(i)));
    end
end