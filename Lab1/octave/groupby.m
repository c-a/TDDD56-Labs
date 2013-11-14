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
%	Function groupby
%
%	Separate rows of matrix into groups within which the set of values in
%	given columns are identical in every row, but bifferent between rows of
%	different groups. A group takes shape in a smaller matrix. Each row of
%	the original matrix is mapped to exactly one group. All groups can then
%	be reduced to one row then merged together into a single matrix thanks
%	to the function reduce(). The rows within a group keep the same order as
%	they had in the input matrix.
%	
%	
%	Parameters:
%	matrix:	The matrix to be separated separate into groups (matrix)
%	col:	The column indexes that are taken into account when forming
%		groups (vector).
%	out:	All groups of rows (cell array of matrices)
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
%	b = groupby(a, [1 2])
%	b = {
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

function out = groupby(matrix, col)

% returns several matrices separated into groups where the value of all given col are identical. The matrix is sorted in acending order in the priority given by col
%
% cell_array = groupby(matrix, vector);
%
% only one line of vector col is taken into account

nb_col = size(col);
nb_col = nb_col(2);

% Warm-up
old_size = 1;
old_recipient = {matrix};

for i = 1:nb_col
	new_recipient = {};
	new_size = 0;

	for j = 1:old_size
		% sort the matrix regarding the current column
		mat = sortrows(old_recipient{j}, col(1, i));

		% The matrix is copied from the old container to the new container
		% beginning with first line
		new_size = new_size + 1;
		new_recipient{new_size}(1, :) = mat(1, :);
		new_mat_size = 2;

		% Browse the rest of the matrix and copy its rows in the new recipient
		size_mat = size(mat);
        size_mat = size_mat(1);
		key = mat(1, col(1, i));
		for k = 2:size_mat
			% If a new key has been found
			if mat(k, col(1, i)) ~= key
				new_size = new_size + 1; % increment the size of new_recipient
				new_mat_size = 1; % reset the new matrix' size
				
				% Update the key
				key = mat(k, col(1, i));
			end

			% Copy another matrix row into the new matrix
			new_recipient{new_size}(new_mat_size, :) = mat(k, :);
			new_mat_size = new_mat_size + 1;
		end
	end

	% Get ready for a new recursion step
	old_recipient = new_recipient;
	old_size = new_size;
end

out = old_recipient;

end
