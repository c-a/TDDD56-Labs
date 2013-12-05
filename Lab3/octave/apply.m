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
%	Function apply
%
%	Applies a function to a matrix row by row, and writes its scalar
%	output in the approriate column of this matrix in the same row.
%	
%	
%	Parameters:
%	matrix:	The matrix to apply the function to (matrix)
%	column:	Column index of the matrix where the outputs are written (scalar)
%	func:	The function to apply to the matrix (pointer to function).
%		Usually a user-defined function denoted by @<function_name>.
%		The function must take the followin signature:
%		function y = <function_name(column, row)
%		where:
%			column in the index of the column where output is written (scalar)
%			row is a vector where is copied the row to which the function
%				is currently applied (vector)
%			y is the output value (scalar)
%	out:	The matrix after computation

function out = apply(matrix, column, func)
% matrix: manipulated matrix
% culumn: column to be transformed
% func_ptr: function to be called. function pointed must be of the form (<line number being processed>(scalar), <line data>(vector))
size_matrix = size(matrix);
size_x = size_matrix(2);
size_y = size_matrix(1);

for i = 1:size_y
	for j = 1:size_x
		if j == column
			out(i, j) = func(column, matrix(i, :));
		else
			out(i, j) = matrix(i, j);
		end
	end
end
end
