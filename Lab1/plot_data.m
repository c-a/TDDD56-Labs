% maxiter width height lower_r upper_r lower_i upper_i nb_thread loadbalance try thread thread_start_time_sec thread_start_time_nsec thread_stop_time_sec thread_stop_time_nsec global_start_time_sec global_start_time_nsec global_stop_time_sec global_stop_time_nsec

globalperf = select(data, [8 9 16 17 18 19]); % keep nb_threads and global timings
globalperf = duplicate(globalperf, [1 1 1 1 1 2]); % create 1 more columns to calculate timings
globalperf = apply(globalperf, 7, @time_difference_global); % compute the time delta from data gathered
globalperf = select(globalperf, [1 2 7]); % keep nb threads and timing
globalperf = duplicate(globalperf, [1 1 2]); % create 1 more columns to calculate global timing std dev

globalperf_1 = where(globalperf, [2], {[1]});
globalperf_2 = where(globalperf, [2], {[2]});
globalperf = where(globalperf, [2], {[0]});

globalperf = select(globalperf, [1 3 4]);
globalperf = groupby(globalperf, [1]); % group stats by skip and size
globalperf = reduce(globalperf, {@mean, @mean, @std}); % compute means on these ones

globalperf_1 = select(globalperf_1, [1 3 4]);
globalperf_1 = groupby(globalperf_1, [1]); % group stats by skip and size
globalperf_1 = reduce(globalperf_1, {@mean, @mean, @std}); % compute means on these ones

globalperf_2 = select(globalperf_2, [1 3 4]);
globalperf_2 = groupby(globalperf_2, [1]); % group stats by skip and size
globalperf_2 = reduce(globalperf_2, {@mean, @mean, @std}); % compute means on these ones

threadperf = select(data, [8 9 11 12 13 14 15]); % keep nbthreads, loadbalance, thread index and thread timings
threadperf = duplicate(threadperf, [1 1 1 1 1 1 2]);
threadperf = apply(threadperf, 8, @time_difference_thread);
threadperf = select(threadperf, [1 2 3 8]);
threadperf = groupby(threadperf, [1 2 3]);
threadperf = reduce(threadperf, {@mean, @mean, @mean, @mean});

threadperf_1 = select(where(threadperf, [2], {[1]}), [1 3 4]);
threadperf_2 = select(where(threadperf, [2], {[2]}), [1 3 4]);
threadperf = select(where(threadperf, [2], {[0]}), [1 3 4]);

threadperf_1 = extend(threadperf_1, [1], [2], 0);
threadperf_2 = extend(threadperf_2, [1], [2], 0);
threadperf = extend(threadperf, [1], [2], 0);

globalperf_legend = {'Unbalanced' 'Load-balanced 1' 'Load-balanced 2'};
sizeof=size(reduce(groupby(select(threadperf, [1]), [1]), {@mean}));
for i = 1:sizeof(1)
	threadperf_legend{i} = ['Thread ' int2str(i)];
end

quickerrorbar(1, ...
	{globalperf, globalperf_1, globalperf_2}, ... % data to be plotted
	1, 2, 3, ... % column for x values then for y values and error
	{[1 0 0] [1 0 1] [0 0 1] [0 0 0] [0 0.5 0.5]}, ... % colors
	{'o' '^' '.' 'x' '>' '<'}, ... % markers
	2, 15, 'MgOpenModernaBold.ttf', 8, 800, 400, ... %lines thickness, markers sizes, legend font and size, output bitmap size along x and y
	'Number of threads employed', 'Time in milliseconds', 'Global computation time for unbalanced and load-balanced threads', ...
	{'Unbalanced' 'Load-balanced (1)' 'Load-balanced (2)'}, ... % Curves names
	'northeast', 'global_timing_all.eps', 'epsc'); % location of legend, plot output filename

quickerrorbar(2, ...
	{globalperf}, ... % data to be plotted
	1, 2, 3, ... % column for x values then for y values and error
	{[1 0 0] [1 0 1] [0 0 1] [0 0 0] [0 0.5 0.5]}, ... % colors
	{'o' '^' '.' 'x' '>' '<'}, ... % markers
	2, 15, 'MgOpenModernaBold.ttf', 8, 800, 400, ... %lines thickness, markers sizes, legend font and size, output bitmap size along x and y
	'Number of threads employed', 'Time in milliseconds', 'Global computation time (unbalanced)', ...
	globalperf_legend, ... % Curves names
	'northeast', 'global_thread_timing_unbalanced.eps', 'epsc'); % location of legend, plot output filename

quickbar(2, ...
	threadperf, ... % data to be plotted
	1, 3, -1, ... % column for x values then for y values and base value
	'grouped', 0.5, ...
	'MgOpenModernaBold.ttf', 8, 800, 400, ... %lines thickness, markers sizes, legend font and size, output bitmap size along x and y
	'Number of threads employed', 'Time in milliseconds', 'Global and per thread computation time (unbalanced)', ...
	[{globalperf_legend{1}} threadperf_legend], ...
	'northeast', 'global_thread_timing_unbalanced.eps', 'epsc'); % location of legend, plot output filename

quickerrorbar(3, ...
	{globalperf, globalperf_2}, ... % data to be plotted
	1, 2, 3, ... % column for x values then for y values and error
	{[1 0 0] [1 0 1] [0 0 1] [0 0 0] [0 0.5 0.5]}, ... % colors
	{'o' '^' '.' 'x' '>' '<'}, ... % markers
	2, 15, 'MgOpenModernaBold.ttf', 8, 800, 400, ... %lines thickness, markers sizes, legend font and size, output bitmap size along x and y
	'Number of threads employed', 'Time in milliseconds', 'Global computation time (load-balancing 1)', ...
	globalperf_legend, ... % Curves names
	'northeast', 'global_thread_timing_balanced_1.eps', 'epsc'); % location of legend, plot output filename

quickbar(3, ...
	threadperf_1, ... % data to be plotted
	1, 3, -1, ... % column for x values then for y values and base value
	'grouped', 0.5, ...
	'MgOpenModernaBold.ttf', 8, 800, 400, ... %lines thickness, markers sizes, legend font and size, output bitmap size along x and y
	'Number of threads employed', 'Time in milliseconds', 'Global and per thread computation time (load-balancing 1)', ...
	[{globalperf_legend{1:2}} threadperf_legend], ...
	'northeast', 'global_thread_timing_balanced_1.eps', 'epsc'); % location of legend, plot output filename

quickerrorbar(4, ...
	{globalperf, globalperf_1, globalperf_2}, ... % data to be plotted
	1, 2, 3, ... % column for x values then for y values and error
	{[1 0 0] [1 0 1] [0 0 1] [0 0 0] [0 0.5 0.5]}, ... % colors
	{'o' '^' '.' 'x' '>' '<'}, ... % markers
	2, 15, 'MgOpenModernaBold.ttf', 8, 800, 400, ... %lines thickness, markers sizes, legend font and size, output bitmap size along x and y
	'Number of threads employed', 'Time in milliseconds', 'Global computation time (load-balancing 2)', ...
	globalperf_legend, ... % Curves names
	'northeast', 'global_thread_timing_balanced_2.eps', 'epsc'); % location of legend, plot output filename

quickbar(4, ...
	threadperf_2, ... % data to be plotted
	1, 3, -1, ... % column for x values then for y values and base value
	'grouped', 0.5, ...
	'MgOpenModernaBold.ttf', 8, 800, 400, ... %lines thickness, markers sizes, legend font and size, output bitmap size along x and y
	'Number of threads employed', 'Time in milliseconds', 'Global and per thread computation time (load-balancing 2)', ...
	[{globalperf_legend{1:3}} threadperf_legend], ...
	'northeast', 'global_thread_timing_balanced_2.eps', 'epsc'); % location of legend, plot output filename

quickbar(5, ...
	threadperf, ... % data to be plotted
	1, 3, -1, ... % column for x values then for y values and base value
	'grouped', 0.5, ...
	'MgOpenModernaBold.ttf', 8, 800, 400, ... %lines thickness, markers sizes, legend font and size, output bitmap size along x and y
	'Number of threads employed', 'Time in milliseconds', 'Computation time per thread (unbalanced)', ...
	threadperf_legend, ...
	'northeast', 'threads_timing_unbalanced.eps', 'epsc'); % location of legend, plot output filename

quickbar(6, ...
	threadperf_1, ... % data to be plotted
	1, 3, -1, ... % column for x values then for y values and base value
	'grouped', 0.5, ...
	'MgOpenModernaBold.ttf', 8, 800, 400, ... %lines thickness, markers sizes, legend font and size, output bitmap size along x and y
	'Number of threads employed', 'Time in milliseconds', 'Computation time per thread (load-balancing 1)', ...
	threadperf_legend, ...
	'northeast', 'threads_timing_1.eps', 'epsc'); % location of legend, plot output filename

quickbar(7, ...
	threadperf_2, ... % data to be plotted
	1, 3, -1, ... % column for x values then for y values and base value
	'grouped', 0.5, ...
	'MgOpenModernaBold.ttf', 8, 800, 400, ... %lines thickness, markers sizes, legend font and size, output bitmap size along x and y
	'Number of threads employed', 'Time in milliseconds', 'Computation time per thread (load-balancing 2)', ...
	threadperf_legend, ...
	'northeast', 'threads_timing_2.eps', 'epsc'); % location of legend, plot output filename
