% compute a windowed distance (squared) matrix 
% for an array of column vectors (a matrix)
% temporal window is of length n
%
% D = DistMatWin(A,n)
%
% Written by Alan Peters with modification by Chad Jenkins

function D = DistMatWin(A,n,p)


% compute the distance matrix
T = DistMat(A);

if n == 1 % then there is no window
   D = T;
   return
end

[R C] = size(T);

% create an accummulator array large enough
% to hold n copies of the matrix, each shift
% diagonally by 1 row and column
%(ocj) D = zeros(R+n-1,C+n-1,n);
D = zeros(R+n-1,C+n-1);

% create an array for the averging quotient 
% for each element of the windowed distance 
% matrix.
W = zeros(R+n-1,C+n-1);

% create an array of ones to accumulate into
% the averaging quotient array 
O = ones(R,C);

% (ocj) create averaging array for partitions
%a = n*ones(1,C);
%for k = p
%   for j = 0:4
%      a(1,k-j) = j+1;
%   end
%end


Tp = T;
Op = O;
% for each element of the window
for k = 1:n
   fprintf('accumulating window %d\n',k);
   % copy the dist mat into the correct position
   %(ocj) D(k:R+k-1,k:C+k-1,k) = T;  
   %(ocj1)D(k:R+k-1,k:C+k-1) = D(k:R+k-1,k:C+k-1) + T;  
   D(k:R+k-1,k:C+k-1) = D(k:R+k-1,k:C+k-1) + Tp;  
   % accumulate the averaging quotient
   %(ocj1)W(k:R+k-1,k:C+k-1) = W(k:R+k-1,k:C+k-1) + O;  
   W(k:R+k-1,k:C+k-1) = W(k:R+k-1,k:C+k-1) + Op;  
   Tp(:,p-(k-1)) = 0;
   Tp(p-(k-1),:) = 0;
   %[size(T) size(Tp)]
   %p
   %p-(k-1)
   %size(Tp(:,p-(k-1)))
   Op(:,p-(k-1)) = 0;
   Op(p-(k-1),:) = 0;
end

%figure; 
%imagesc(Op);
%imagesc(W);

% accumulate the windowed by summing along the 3rd axis
%(ocj) D = sum(D,3);

% complete the average by dividing by the averaging
% quotients and cutting the middle out of the result
m = ceil(n/2);
%(ocj)D = D(m:R+m-1,m:C+m-1) ./ W(m:R+m-1,m:C+m-1);
D = D(1:R,1:C) ./ W(1:R,1:C);
return
