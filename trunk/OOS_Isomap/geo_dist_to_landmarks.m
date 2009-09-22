function Dists = geo_dist_to_landmarks(D, GD, n_size)
% D = geo_dist_to_landmarks(point, landmarks, GD, n_size)
% returns the geodesic distance between the new point and the landmarks
% landmarks are in raw space (n x d), and GD is the geodesic distance between them.

%Get the eucledian distance from the point to the landmarks
%D = L2_distance(point,landmarks);
N = length(D);
[vals,inds] = sort(D);

%get the n_size nearest neighbors
neighbori = zeros(n_size,1);
neighborv = zeros(n_size,1); 
for i=1:n_size
    neighbori(i) = inds(i);
    neighborv(i) = vals(i);
end

%extend into full distance matrix
%pull out the neighbors
GDland=GD(neighbori,:);
%add the distances
GDland=GDland+repmat(neighborv,1,N);
Dists=min(GDland);



%Dists = zeros(1,N);
%for i=1:N %for every landmark
%    pths = neighborv;
%    for j=1:length(pths) %the distance through the neighbors
%        pths(j) = pths(j) + GD(neighbori(j),i);
%    end
%    Dists(i) = min(pths); %shortest distance
%end