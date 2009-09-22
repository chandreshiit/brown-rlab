function [Di Dj Ds Comp_Sizes] = nearest_neighbors(Data, max_neighborhood_size, as_max)
% [Di Dj Ds Comp_Sizes]=nearest_neighbors(data, neighborhood_size, as_max =0)
% if as_max=1, treat the passed size as an upper limit and ask the user to
% choose the neighborhood size based on a graph.
% Di Dj Ds make up the maximum nearest neighbor matrix


if nargin<3
    as_max=0;
end

N = length(Data(1,:));
Di = zeros(max_neighborhood_size,N);
Dj = zeros(max_neighborhood_size,N);       
Ds = zeros(max_neighborhood_size,N);
%disp('alloced')
sqa=sum(Data.^2)';
%disp('squared');
sqb=2*Data';
%disp('2trans');
%Could parallelize this for speedup
per=round(N*.1);
disp([' Nearest Neighbors on ' num2str(N) ' points']);
tic;
for i=1:N
    b=Data(:,i);
    bb=sqa(i); 
    sqc=sqb*b; 
    d=sqrt((sqa+bb)-sqc);
    [vals,indicies] = sort(d);
    Di(:,i)=i;
    Dj(:,i) = indicies(2:max_neighborhood_size+1);%The first is always self=0
    Ds(:,i) = vals(2:max_neighborhood_size+1);
    if (rem(i,per) == 1)
        disp(['  ' num2str(i) ' done, ' num2str((toc/i)*(N-i)/60) ' minutes left']);
    end
end
disp([' NN total time = ' num2str(toc/60) ' minutes']);

%Find how many neighbors we actually need
if as_max==1
    Comp_Sizes=zeros(max_neighborhood_size,1);
    for n_s=max_neighborhood_size:-1:2
        disp(['trying ' num2str(n_s)]);
        [Dnn I]=make_dnn(Di,Dj,Ds,n_s);
        Comp_Sizes(n_s)=length(I);
    end
else
    Comp_Sizes=[];
end