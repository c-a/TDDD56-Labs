% 1-input_size 2-input_set 3-input_try 4-variant 5-nb_threads 6-global_start_sec 7-global_start_nsec 8-global_stop_sec 9-global_stop_nsec

function y = time_difference_global(col, line)
	NSEC_IN_SEC = 1000000000;
	MSEC_IN_SEC = 1000;
	NSEC_IN_MSEC = NSEC_IN_SEC / MSEC_IN_SEC;
	% Numbers in columns 5 6 7 8
	y = (((line(7) - line(5)) * NSEC_IN_SEC + line(8) - line(6))) / NSEC_IN_MSEC;
end
