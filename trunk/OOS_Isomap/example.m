%This code uses dijkstra's algorithm for the geodesic distance calculation.
%dijsktra.mexglx is compiled on Linux
%If you delete it, dijkstra.m will be used instead, but is much slower
%to re-compile for your system:
%mex -O dijkstra/dijkstra.cpp

%You may need to apply the patch:
%patch dijkstra/dijkstra.cpp dijkstra/dijkstra.patch

%Make a swiss roll data set
[d3] = genRippleSwissRoll([5 15],1.7,50,[0 0],0,20000);
%Look at it
figure(1)
plot3(d3(1,:),d3(2,:),d3(3,:),'.')

%we use a subset ~ 10% as the landmarks
ri = randperm(length(d3));
landmarks = d3(:,ri(1:2500));

%Do OOSIsomap
Coords2 = OOS_Isomap(d3,10,landmarks,2);

%View the embedding.
figure(2)
plot(Coords2(1,:),Coords2(2,:),'.')
