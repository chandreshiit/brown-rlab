% compute distance matrix for an array of column vectors (a matrix)
%
% D = DistMat(A)

function D = DistMat(A)


[R C] = size(A);
D = zeros(C,C);

for c = 2:C
   for r = 1:c-1
      D(r,c) = norm( (A(:,c)-A(:,r)), 2 );
   end
end

D = D + D';

return