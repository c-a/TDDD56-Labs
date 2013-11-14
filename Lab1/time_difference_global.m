function y = time_difference_global(col, line)
	NSEC_IN_SEC = 1000000000;
	MSEC_IN_SEC = 1000;
	NSEC_IN_MSEC = NSEC_IN_SEC / MSEC_IN_SEC;
	y = (((line(5) - line(3)) * NSEC_IN_SEC + line(6) - line(4))) / NSEC_IN_MSEC;
end
