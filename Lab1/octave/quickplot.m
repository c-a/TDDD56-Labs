%    Copyright 2011 Nicolas Melot
%
%    Nicolas Melot (nicolas.melot@liu.se)
%
%
%    This program is free software: you can redistribute it and/or modify
%    it under the terms of the GNU General Public License as published by
%    the Free Software Foundation, either version 3 of the License, or
%    (at your option) any later version.
%
%    This program is distributed in the hope that it will be useful,
%    but WITHOUT ANY WARRANTY; without even the implied warranty of
%    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%    GNU General Public License for more details.
%
%    You should have received a copy of the GNU General Public License
%    along with this program.  If not, see <http://www.gnu.org/licenses/>.
%
% =========================================================================
%
%	Function quickplot
%
%	Plots a curves along x and y axis. Similar to quickerrorbar but without
%	error bars. Output the graphic to a file in eps or png format.
%	
%	Parameters:
%	fignum:	Figure number. A figure of the same number as one or several
%		previously draw ones is draw on the same canvas and produces
%		an output that integrate these previous figures (scalar).
%	data:	Matrix containing x and y values to be plotted (matrix).
%	data_x:	Index in the input matrix (data) where the x axis values are 
%		recorded (scalar).
%	data_y:	Index in the input matrix (data) where the x axis values are 
%		recorded (scalar).
%	error:	Base value from which the bar of the histogram start. This
%		shifts up or down the y value to be plotted. When y values are
%		high or zeros, a base of -1 makes possible to see bars
%		representing a zero (scalar).
%	colors:	Colors of the curves to plot. Each color is represented by either
%		a string ('red', 'blue') or an RGB vector (cell of string and/or
%		vectors).
%	marks:	Marks to draw at each point of curves in the graph (cell of
%		strings).
%	markss:	Size of the marks (scalar).
%	curvew:	Thickness of the curves on the graph (scalar)
%	fontn:	Name of the font to use when writing legend, title and labelling
%		axis. (string)
%	fonts:	Font size of legend, title and axes label (scalar)
%	x_size:	Length of the canvas in pixels, along the x axis (scalar).
%	y_size:	Height of the canvas in pixels, along the y axis (scalar).
%	x_axis:	Label for x axis (string).
%	y_axis:	Label for y axis (string).
%	grapht:	Title of the graph (string).
%	graphl: Label of all curves or bars in the graph (cell of string).
%	legloc: Legend location, for example :'northeast'; see help legend (string).
%	outf:	Filename to output the graph (string).
%	format:	Descriptor of the output format. Example: 'epsc2'; see help print
%		(string).

function quickplot(fignum, data, data_x, data_y, colors, marks, curvew, markss, fontn, fonts, x_size, y_size, x_axis, y_axis, grapht, graphl, legloc, outf, format)

% Disable window popups when generating a new graph
set (0, 'defaultfigurevisible', 'off') 

figure(fignum);

plotting(1) = plot(data{1,1}(:,data_x), data{1,1}(:, data_y));
set(plotting(1), 'marker', marks{1, 1});
set(plotting(1), 'markersize', markss);
set(plotting(1), 'linewidth', curvew);
set(plotting(1), 'color', colors{1, 1});
hold on;

maxi = size(data);
maxi = maxi(2);
for i = 2:maxi
	plotting(i) = plot(data{1, i}(:, data_x), data{1, i}(:, data_y));

	set(plotting(i), 'marker', marks{1, i});
	set(plotting(i), 'markersize', markss);
	set(plotting(i), 'linewidth', curvew);
	set(plotting(i), 'color', colors{1, i});
end

% Here come the general graph settings
g_title = title(grapht);
x_label = xlabel(x_axis);
y_label = ylabel(y_axis);
legend(graphl, 'location', legloc);
legend('boxon');

set(x_label, 'fontname', fontn);
set(x_label, 'fontsize', fonts);
set(y_label, 'fontname', fontn);
set(y_label, 'fontsize', fonts);
set(g_title, 'fontname', fontn);
set(g_title, 'fontsize', fonts);

set (findobj (gcf, '-property', 'fontname'), 'fontname', fontn);
set (findobj (gcf, '-property', 'fontsize'), 'fontsize', fonts);

print(outf, ['-d' format], ['-F:' num2str(fonts)], ['-S' num2str(x_size) ',' num2str(y_size)]);

hold off;
set (0, 'defaultfigurevisible', 'on') 
	
end
