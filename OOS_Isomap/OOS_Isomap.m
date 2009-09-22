function Coords = OOS_Isomap(Data,neighborhood_size,landmarks,dimensions)
%function Coords=OOS_Isomap(Data,neighborhood_size,landmarks,dimensions)
%landmarks are points, not indicies

%get geodesic distances between landmarks
[Di Dj Ds]=nearest_neighbors(landmarks,neighborhood_size,0);
[Dnn I]=make_dnn(Di,Dj,Ds,neighborhood_size);
landmarks=landmarks(:,I);
GD=dijkstra(Dnn,1:length(I));

%transform the landmarks
[land_trans,R,vec,val] = MDS(GD,dimensions,0);

%Run the data
Coords=embed_new_data(Data,landmarks,GD,neighborhood_size,vec,val);