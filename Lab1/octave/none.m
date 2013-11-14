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
%	Function none
%
%	Returns the first value of the vector given in input. This function is
%	recommended when reducing columns that were considered when forming groups
%	using groupby(). Other functions normally equally suitable (such as @mean)
%	may generate a strange behavior due to rounding errors, invisible to debug
%	and thus hard to detect and fix.
%	
%	
%	Parameters:
%	vector:	The column to be reduced to one value (vector)
%	out:	The first element of the input vector (scalar)
%
%	Example:
%	a = [1; 2; 3; 4]
%	a = [
%		1 ;
%		2 ;
%		3 ;
%		4;
%	]
%	b = none(a)
%	b = 1

function y = none(vector)
	y = vector(1,1);
end
