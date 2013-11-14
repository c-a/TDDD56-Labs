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
%	Function reduce
%
%	Reduce all groups taken from the groupby function output into a single
%	row per group applying one given function per column, and merge all 
%	obtained rows into a single matrix.
%	
%	
%	Parameters:
%	cell:	The cell containing the groups to be reduced and merged (cell
%		array of matrices).
%	funcs:	All functions to be applied. Functions are applied to the columns
%		as a whole, not row by row. This allow the computation of
%		statistics (mean, standard deviation). One per column in every
%		matrix of the input cell. The first function is applied to the
%		first column of each group, the second to the second column and so.
%		typical functions are @mean or @std (cell array of function
%		pointers).
%	out:	Matrix of all reduced groups merged together (matrix)
%
%	Example:
%	a = {
%		[1,1] = [
%			1 1 3 4 ;
%			1 1 5 6 ;
%		]
%		[1,2] = [
%			1 2 7 8 ;
%			1 2 7 7 ;
%		]
%		[1,3] = [
%			2 3 3 9 ;
%			2 3 5 5 ;
%		]
%	}
%	b = reduce(a, {@none, @none, @mean, @std})
%	b = [
%		1 1 4 1.41421 ;
%		1 2 7 0.70711 ;
%		2 3 4 2.82843 ;
%	]

function  out = reduce(cell, funcs)

maxi = size(cell);
maxi = maxi(2);
for i = 1:maxi
    maxj = size(funcs);
    maxj = maxj(2);
	for j = 1:maxj
		out(i,j) = funcs{1, j}(cell{1, i}(:, j));
	end
end
