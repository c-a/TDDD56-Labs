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
%	Function extend
%
%	Creates dummy rows in order to make every groups (defined in the same way
%	as the groupby function) to have as many rows as the biggest group. The
%	columns of these dummy rows take the value of the same column in the 
%	original rows if the column is part of the group definition or the value of
%	parameter value if they don't. The values of columns defined by the
%	parametercopy are copied from the biggest group to all other extended
%	groups.
%	This function is usefull to present data at value zero when they were non
%	existant or relevant in the original experiment. For instance, to show the
%	work of threads 3 and 4 where experiments only involved 2 threads in some
%	cases and 4 threads in other cases.
%	
%	
%	Parameters:
%	matrix:	The matrix to be filtered (matrix)
%	group:	Indexes of columns to defining the group (vector).
%	copy:	Indexes of columns to be copied from the biggest group to all other
%		groups (vector). Be careful when choosing the columns to be copied,
%		not to make it to erase important data in smaller groups.
%	value:	The value to copy to all other columns (columns that do not not
%		define the groups or that are not copied from the biggest group
%		(scalar).
%	out:	The output matrix with all dummy rows added.
%
%	Example:
%	a = [1 1 3 4; 1 2 7 8; 1 1 5 6; 2 3 3 9; 2 3 5 5 ; 1 2 7 7]
%	a = [
%		1 1 3 4 ;
%		1 2 7 8 ;
%		1 1 5 6 ;
%		2 3 3 9 ;
%		2 3 5 5 ;
%		1 2 7 7 ;
%	]
%	b = extend(a, [1], [2], 0)
%	b = [
%		1 1 3 4 ;
%		1 2 7 8 ;
%		1 1 5 6 ;
%		1 2 7 7 ;
%		2 1 3 9 ;
%		2 2 5 5 ;
%		2 1 0 0 ;
%		2 2 0 0 ;
%	]

function out = extend(matrix, group, copy, value)
sep = groupby(matrix, group);
max_size=0;
max_index=0;

maxi = size(sep);
maxi = maxi(2);

for i = 1:maxi
    size_sep = size(sep{i});
	if size_sep(1) > max_size
		max_size = size_sep(1);
		max_index = i;
	end
end

out=[];

for i = 1:maxi
    size_sep = size(sep{i});
	height = max_size - size_sep(1);
	width = size_sep(2);
	append = [sep{i}; zeros(height, width)];

    maxj = size(copy);
    maxj = maxj(2);
	for j = 1:maxj
		append(:, copy(j)) = sep{max_index}(1:max_size, copy(j));		
    end

    maxj = size(group);
    maxj = maxj(2);
	for j = 1:maxj
		append(:, group(j)) = sep{i}(1, group(j));
	end

	out = [out; append];
end

end
