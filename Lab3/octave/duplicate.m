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
%	Function duplicate
%
%	Duplicate once or more one or several columns of a given matrix
%	
%	
%	Parameters:
%	matrix:	The matrix which some columns are duplicated (matrix)
%	times:	Number of times each column of the matrix must be represented
%		in the output (vector, as many elements as matrix has columns,
%		every elements striclty greater than zero)
%	out:	The matrix with duplicated columns (matrix)
%
%	Example:
%	a = [1 2 3 4; 5 6 7 8]
%	a = [
%		1 2 3 4 ;
%		5 6 7 8 ;
%	]
%	b = duplicate(a, [1 1 2 3])
%	b = [
%		1 2 3 3 4 4 4 ;
%		5 6 7 7 8 8 8 ;
%	] 

function out = duplicate(matrix, times)
column_count = 1;
maxi = size(matrix);
maxi = maxi(2);

for i = 1:maxi
	for j = 1:times(1, i)
		out(:, column_count) = matrix(:, i);
		column_count = column_count + 1;
	end
end
end
