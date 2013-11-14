% measure max_push_pop nb_threads non_blocking try thread global_start_sec global_start_nsec global_stop_sec global_stop_nsec thread_start_sec thread_start_nsec thread_stop_sec thread_stop_nsec

globaltiming = select(data, [1 2 3 4 6 7 8 9 10 11 12 13 14]); % keep algorithm prevent_spawn trivial_size nb_threads loadbalance input_set input_size thread global_start_sec global_start_nsec global_stop_sec global_stop_nsec
globaltiming = duplicate(globaltiming, [1 1 1 1 1 1 1 1 1 2 1 1 2]); % create 1 more columns to calculate timings
globaltiming = apply(globaltiming, 10, @time_difference_global); % compute the time delta from data gathered
globaltiming = apply(globaltiming, 15, @time_difference_thread); % compute the time delta from data gathered
globaltiming = select(globaltiming, [1 2 3 4 5 10 15]); % keep features and associated previously calculated delta
globaltiming = duplicate(globaltiming, [1 1 1 1 1 2 2]); % create 1 more columns to calculate global timing std dev

thread_timing = groupby(globaltiming, [1 2 3 4 5]);
thread_timing = reduce(thread_timing, {@none, @none, @none, @none, @mean, @none, @std, @mean, @std});
thread_timing = extend(thread_timing, [1 2 3 4], [5], 0);
thread_timing_push = where(thread_timing, [1], {[1]});
thread_timing_push_standard = where(thread_timing_push, [4], {[0]});
thread_timing_push_soft = where(thread_timing_push, [4], {[1]}); 
thread_timing_push_hard = where(thread_timing_push, [4], {[2]});
thread_timing_pop = where(thread_timing, [1], {[2]});
thread_timing_pop_standard = where(thread_timing_pop, [4], {[0]});
thread_timing_pop_soft = where(thread_timing_pop, [4], {[1]});
thread_timing_pop_hard = where(thread_timing_pop, [4], {[2]});

global_timing = groupby(globaltiming, [1 2 3 4]);
global_timing = reduce(global_timing, {@none, @none, @none, @none, @none, @mean, @std, @mean, @std});
global_timing = where(global_timing, [2], {[5000000]});
global_timing_push = where(global_timing, [1], {[1]});
global_timing_push_standard = where(global_timing_push, [4], {[0]});
global_timing_push_soft = where(global_timing_push, [4], {[1]}); 
global_timing_push_hard = where(global_timing_push, [4], {[2]});
global_timing_pop = where(global_timing, [1], {[2]});
global_timing_pop_standard = where(global_timing_pop, [4], {[0]});
global_timing_pop_soft = where(global_timing_pop, [4], {[1]});
global_timing_pop_hard = where(global_timing_pop, [4], {[2]});

quickerrorbar(1, ...
	{global_timing_push_standard, global_timing_push_soft, global_timing_push_hard}, ... % data to be plotted
	3, 6, 7, ... % column for x values then for y values and error
	{[1 0 0] [1 0 1] [0 0 1] [0 0 0] [0 0.5 0.5] [0 0.5 0]}, ... % colors
	{'o' '^' '.' 'x' '>' '<'}, ... % markers
	2, 15, 'MgOpenModernaBold.ttf', 8, 800, 400, ... %lines thickness, markers sizes, legend font and size, output bitmap size along x and y
	'Number of threads employed', 'Time in milliseconds', 'Time evolution with the number of threads pushing to the same stack', ...
	{'Standard lock protection' 'Software, lock-based CAS' 'hardware CAS'}, ... % Curves names
	'northeast', 'global_timing_push.eps', 'epsc'); % location of legend, plot output filename

quickerrorbar(1, ...
	{global_timing_push_standard, global_timing_push_soft, global_timing_push_hard, global_timing_pop_standard, global_timing_pop_soft, global_timing_pop_hard}, ... % data to be plotted
	3, 6, 7, ... % column for x values then for y values and error
	{[1 0 0] [1 0 1] [0 0 1] [0 0 0] [0 0.5 0.5] [0 0.5 0]}, ... % colors
	{'o' '^' '.' 'x' '>' '<'}, ... % markers
	2, 15, 'MgOpenModernaBold.ttf', 8, 800, 400, ... %lines thickness, markers sizes, legend font and size, output bitmap size along x and y
	'Number of threads employed', 'Time in milliseconds', 'Time evolution with the number of threads pushing or popping to the same stack', ...
	{'Standard lock protection (push)' 'Software, lock-based CAS (push)' 'hardware CAS (push)' 'Standard lock protection (pop)' 'Software, lock-based CAS (pop)' 'hardware CAS (pop)'}, ... % Curves names
	'northeast', 'global_timing_both.eps', 'epsc'); % location of legend, plot output filename

quickerrorbar(2, ...
	{global_timing_pop_standard, global_timing_pop_soft, global_timing_pop_hard}, ... % data to be plotted
	3, 6, 7, ... % column for x values then for y values and error
	{[1 0 0] [1 0 1] [0 0 1] [0 0 0] [0 0.5 0.5] [0 0.5 0]}, ... % colors
	{'o' '^' '.' 'x' '>' '<'}, ... % markers
	2, 15, 'MgOpenModernaBold.ttf', 8, 800, 400, ... %lines thickness, markers sizes, legend font and size, output bitmap size along x and y
	'Number of threads employed', 'Time in milliseconds', 'Time evolution with the number of threads popping to the same stack', ...
	{'Standard lock protection' 'Software, lock-based CAS' 'hardware CAS'}, ... % Curves names
	'northeast', 'global_timing_pop.eps', 'epsc'); % location of legend, plot output filename
