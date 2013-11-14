function y = time_difference_global(col, line)
	NSEC_IN_SEC = 1000000000;
	MSEC_IN_SEC = 1000;
	NSEC_IN_MSEC = NSEC_IN_SEC / MSEC_IN_SEC;
	y = (((line(8) - line(6)) * NSEC_IN_SEC + line(9) - line(7))) / NSEC_IN_MSEC;
end
