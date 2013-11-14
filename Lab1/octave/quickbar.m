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
%	Function quickbar
%
%	Plots an histogram from the data given as input parameters, format
%	and decorate it (legend, title, axis, etc) and output it to a graphic
%	file eps or png. The histogram can feature several bars per step along
%	the x axis. In this case, several rows in input data are labelled to the
%	same x value and represent several entities shown for this x. The entities
%	are assumed to be always sorted on the same order for every x values.
%	
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
%	base:	Base value from which the bar of the histogram start. This
%		shifts up or down the y value to be plotted. When y values are
%		high or zeros, a base of -1 makes possible to see bars
%		representing a zero (scalar).
%	style:	Style of the bar groups at each x step. For instance 'grouped'
%		or 'stacked'; see help bar (string).
%	thickn:	Thickness of the bars on the graph (scalar)
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

function quickbar(fignum, data, data_x, data_y, base, style, thickn, fontn, fonts, x_size, y_size, x_axis, y_axis, grapht, graphl, legloc, outf, format)

y_marging = 10;
figure(fignum);

x = reduce(groupby(data(:, data_x), [1]), {@mean});
data = extend(data, [data_x], [data_x], base);
data = groupby(data, [data_x]);
y = data{1,1}(:, data_y);

maxi = size(data);
maxi = maxi(2);
for i = 2:maxi
	y = [y data{1, i}(:, data_y)];
end

hold on;
handle = bar(x, y', thickn, style);
%set(handle(1), 'basevalue', base);

max_value = max(max(y));
min_value = min([min(min(y)) base]);

ylim([min_value - (max_value - min_value) / y_marging, max_value + (max_value - min_value) / y_marging]);

% Here come the general graph settings
g_title = title(grapht);
x_label = xlabel(x_axis);
y_label = ylabel(y_axis);
legend(graphl, 'location', legloc);
legend('boxon');
grid off;

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
