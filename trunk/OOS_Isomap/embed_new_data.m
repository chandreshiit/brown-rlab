function Coords=embed_new_data(Data,landmarks,GD,neighborhood_size,vec,val)
%function Coords=embed_new_data(Data,landmarks,GD,neighborhood_size,vec,val)

sumd=sum(GD.^2);
%sum2 = sum(sum(GD.^2));
sum2=sum(sumd);
num_pts = length(Data(1,:));
dimensions=length(vec(1,:));
Coords = zeros(dimensions,num_pts);
per=ceil(.1*num_pts);
disp([' Embed New Data on ' num2str(num_pts) ' points'])
D=L2_distance(Data,landmarks);
tic;
for i=1:num_pts
    %Dnew = geo_dist_to_landmarks(Data(:,i),landmarks,GD,neighborhood_size);
    Dnew = geo_dist_to_landmarks(D(i,:),GD,neighborhood_size);
    Coords(:,i) = MDS_Transform_Landmark(Dnew,sum2,sumd,vec,val);
    if rem(i,per) == 1
        disp(['  ' num2str(i) ' done, ' num2str((toc/i)*(num_pts-i)/60) ' minutes left']);
    end
end
disp([' EnD total time = ' num2str(toc/60) ' minutes']);