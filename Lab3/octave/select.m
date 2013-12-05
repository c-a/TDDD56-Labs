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
%	Function select
%
%	Filters the input matrix to keep only the columns given as parameter.
%	
%	
%	Parameters:
%	matrix:	The matrix to be filtered (matrix)
%	cols:	Indexes of columns to be kept (vector).
%	out:	The input matrix without any column not listed in cols (matrix)
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
%	b = select(a, [2 4])
%	b = [
%		1 4 ;
%		2 8 ;
%		1 6 ;
%		3 9 ;
%		3 5 ;
%		2 7 ;
%	]

function out = select(matrix, cols)
stuff=size(cols);
stuff=stuff(2);
for i = 1:stuff
	out(:, i) = matrix(:, cols(1, i));
end
end
