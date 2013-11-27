% 1-input_size 2-input_set 3-variant 4-nb_threads 5-try 6-global_start_sec 7-global_start_nsec 8-global_stop_sec 9-global_stop_nsec

globalperf = select(data, [1 2 3 4 6 7 8 9]); % 
globalperf = duplicate(globalperf, [1 1 1 1 1 1 1 2]); % 
globalperf = apply(globalperf, 9, @time_difference_global); % 
globalperf = select(globalperf, [1 2 3 4 9]); % 
globalperf = duplicate(globalperf, [1 1 1 1 2]); % 
globalperf = groupby(globalperf, [1 2 3 4]); % 
globalperf = reduce(globalperf, {@mean, @mean, @mean, @mean, @mean, @std}); %

% Keep only results produced by variant 1  
globalperf_v1 = select(where(globalperf, [3], {[1]}), [1 2 4 5 6]);

globalperf_v1_rand = where(globalperf_v1, [2], {[0]}); %
globalperf_v1_rand_size = select(where(globalperf_v1_rand, [3], {[max(globalperf_v1_rand(:, 3))]}), [1 2 3 4 5]);
globalperf_v1_rand_thread = where(globalperf_v1_rand, [1], {[max(globalperf_v1_rand(:, 1))]});
globalperf_v1_inc = where(globalperf_v1, [2], {[1]}); %
globalperf_v1_inc_size = select(where(globalperf_v1_inc, [3], {[max(globalperf_v1_rand(:, 3))]}), [1 2 3 4 5]);
globalperf_v1_inc_thread = where(globalperf_v1_inc, [1], {[max(globalperf_v1_rand(:, 1))]});
globalperf_v1_dec = where(globalperf_v1, [2], {[2]}); %
globalperf_v1_dec_size = select(where(globalperf_v1_dec, [3], {[max(globalperf_v1_rand(:, 3))]}), [1 2 3 4 5]);
globalperf_v1_dec_thread = where(globalperf_v1_dec, [1], {[max(globalperf_v1_rand(:, 1))]});
globalperf_v1_cons = where(globalperf_v1, [2], {[3]}); %
globalperf_v1_cons_size = select(where(globalperf_v1_cons, [3], {[max(globalperf_v1_rand(:, 3))]}), [1 2 3 4 5]);
globalperf_v1_cons_thread = where(globalperf_v1_cons, [1], {[max(globalperf_v1_rand(:, 1))]});

% TIP: copy-paste the previous block and filter to isolate other variants and generate a curve for them
%globalperf_v2 = select(where(globalperf, [3], {[2]}), [1 2 4 5 6]);

% Score calculation
% Mean calculation time using variant 1 at maximal number of thread for both big and small input size
score = mean(select(where(globalperf, [3 4], {[1] [max(globalperf(:, 4))]}), [5]));

quickerrorbar(1, ...
	{globalperf_v1_rand_size, globalperf_v1_inc_size, globalperf_v1_dec_size, globalperf_v1_cons_size}, ... % Plot global timings for 100 millions jumps only.
	1, 4, 5, ... % column for x values then for y values and standard deviation (error bars)
	{[1 0 0] [1 0 1] [0 0 1] [0 0 0] [0 0.5 0.5]}, ... % Colors to be applied to the curves, written in RGB vector format
	{'o' '^' '.' 'x' '>' '<'}, ... % Enough markers for 6 curves. Browse the web to find more.
	2, 15, 'MgOpenModernaBold.ttf', 25, 800, 400, ... % Curves' thickness, markers sizes, Font name and font size, canvas' width and height
	'Input size in 4 bytes integers', 'Time in milliseconds', 'Time to sort an integer set', ... % Title of the graph, label of y axis and label of x axis.
	{'Random input ', 'Decrementing input ', 'Incrementing input ', 'Constant input'}, ... % Labels for curves
	'northeast', 'size.eps', 'epsc'); % Layout of the legend, file to write the plot to and format of the output file

quickerrorbar(2, ...
	{globalperf_v1_rand_thread, globalperf_v1_inc_thread, globalperf_v1_dec_thread, globalperf_v1_cons_thread}, ... % Plot global timings for 100 millions jumps only.
	3, 4, 5, ... % column for x values then for y values and standard deviation (error bars)
	{[1 0 0] [1 0 1] [0 0 1] [0 0 0] [0 0.5 0.5]}, ... % Colors to be applied to the curves, written in RGB vector format
	{'o' '^' '.' 'x' '>' '<'}, ... % Enough markers for 6 curves. Browse the web to find more.
	2, 15, 'MgOpenModernaBold.ttf', 25, 800, 400, ... % Curves' thickness, markers sizes, Font name and font size, canvas' width and height
	'Number of threads', 'Time in milliseconds', 'Time to sort an integer set', ... % Title of the graph, label of y axis and label of x axis.
	{'Random input ', 'Decrementing input ', 'Incrementing input ', 'Constant input'}, ... % Labels for curves
	'northeast', 'thread.eps', 'epsc'); % Layout of the legend, file to write the plot to and format of the output file
