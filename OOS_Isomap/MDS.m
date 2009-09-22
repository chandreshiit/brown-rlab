function [coords, R, vec, val] = MDS(D, d, as_max)
% [C, R, vec, val] = MDS(D, d, as_max)
% Multi-Dimensionally-Transform the points (all points distances in D) into 1:d dimensions
% as_max will calculate all the residuals
% C = coordinates in d-space
% R = residuals

if nargin<3
    as_max=0;
end

N = length(D(:,1));
opt.disp = 0; 
[vec, val] = eigs(-.5*(D.^2 - sum(D.^2)'*ones(1,N)/N - ones(N,1)*sum(D.^2)/N + sum(sum(D.^2))/(N^2)), d, 'LR', opt); 
h = real(diag(val)); 
[foo,sorth] = sort(h);  sorth = sorth(end:-1:1); 
val = real(diag(val(sorth,sorth))); 
vec = vec(:,sorth); 

D = reshape(D,N*N,1); 
coords = real(vec(:,1:d).*(ones(N,1)*sqrt(val(1:d))'))'; 

%residuals for different dimensions
if as_max==1
    start=1;
    R = zeros(d,1);
else
    start=d;
    R=0;
end

for i=start:d
    CC = coords(1:i,:);
    r2 = 1-corrcoef(reshape(real(L2_distance(CC, CC)),N*N,1),D).^2; 
    R(i) = r2(2,1); 
end