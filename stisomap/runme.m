center = 0; %2MOONS

display_figures = 1;

%include isomap routines in path
path(path,'isomap');

%(2MOONS data)
%gc = load('ginput_c2.mat'); %original
gc = load('gc_var2.mat'); %revised

datamatrix = gc.gc; % HACK
for i = 1:size(datamatrix,2)
   datamatrix(find(datamatrix(:,i)==0),i) = min(datamatrix(find(datamatrix(:,i)~=0),i))-5;
end

dm = datamatrix;
ndm = dm;

if (display_figures)
figure;
imagesc(ndm);
end

% isomap
global X;
X = ndm';
options.dims = [1:5];
[Y R] = IsomapII('dfun','k',7);
mY = Y.coords{3}';
figure;
plot3(mY(:,1),mY(:,2),mY(:,3),'b-');
hold on;
for iii = 1:size(mY,1)
   plot3(mY(iii,1),mY(iii,2),mY(iii,3),'.','Color',[iii/size(mY,1) 0 (size(mY,1)-iii)/size(mY,1)],'MarkerSize',20);
end
title('isomap embedding');
axis('equal');

%compute spatial distance between temporal windows at each point
D = DistMatWin(ndm',10,[size(ndm,1)]); 

% mds on distmat
[Y R] = emds(D,1:size(D,2),[3]);
mY = Y.coords{1}';
figure;
plot3(mY(:,1),mY(:,2),mY(:,3),'b-');
hold on;
for iii = 1:size(mY,1)
   plot3(mY(iii,1),mY(iii,2),mY(iii,3),'.','Color',[iii/size(mY,1) 0 (size(mY,1)-iii)/size(mY,1)],'MarkerSize',20);
end
title('MDS embedding (using temporal windowing)');
axis('equal');



%perform continuous spatio-temporal isomap on distance matrix
options.dims = [1:5];

for ct = [1 10 100 1000 10000000]
%options.landmarks = unique(ceil(size(D,1)*rand(1,1500)));
options.landmarks = 1:size(D,1);
options.cctn = ct;
options.tvwin = 20;
options.catn = 1;
[Y R E stcor] = IsomapIIst(D,'ntk',3,options);
%copy 3 dimensional embedding to separate variable
tY = Y.coords{3};

if (display_figures)
%(2MOONS) plot input data and spatio-temporal correspondences
figure;
if (size(dm,2) == 2)
   plot(dm(:,1),dm(:,2),'b.-');
   hold on;
   for i = 1:size(stcor,1)
      line(dm([stcor(i,1) stcor(i,2)],1),dm([stcor(i,1) stcor(i,2)],2),'Color',[1 0 0]);
   end
else
   if (size(dm,2) > 3)
      [ddm,dum1,dum2] = svd(dm,0);
   else
      ddm = dm;
   end
   plot3(ddm(:,1),ddm(:,2),ddm(:,3),'b.-');
   hold on;
   for i = 1:size(stcor,1)
      line(ddm([stcor(i,1) stcor(i,2)],1),ddm([stcor(i,1) stcor(i,2)],2),ddm([stcor(i,1) stcor(i,2)],3),'Color',[1 0 0]);
   end
end
title(sprintf('k-nearest nontrivial neighbors (with k = %d, tvwin = %f)',3,options.tvwin));
axis('equal');
end


if (display_figures)
%plot input data with color coded temporal ordering
figure;
if (size(dm,2) == 2)
   plot(dm(:,1),dm(:,2),'b-');
   hold on;
   for iii = 1:size(dm,1)
      plot(dm(iii,1),dm(iii,2),'.','Color',[iii/size(dm,1) 0 (size(dm,1)-iii)/size(dm,1)],'MarkerSize',20);
   end
else
   if (size(dm,2) > 3)
      [ddm,dum1,dum2] = svd(dm,0);
   else
      ddm = dm;
   end
   plot3(ddm(:,1),ddm(:,2),ddm(:,3),'b-');
   hold on;
   for iii = 1:size(dm,1)
      plot3(ddm(iii,1),ddm(iii,2),ddm(iii,3),'.','Color',[iii/size(ddm,1) 0 (size(ddm,1)-iii)/size(ddm,1)],'MarkerSize',20);
   end
end
title('input data');
axis('equal');
end

if (display_figures)
%plot embedding with color coded temporal ordering
dtY = tY';
if (size(dtY,2) == 2)
   plot(dtY(:,1),dtY(:,2),'b-');
   hold on;
   for iii = 1:size(dtY,1)
      plot(dtY(iii,1),dtY(iii,2),'.','Color',[iii/size(dtY,1) 0 (size(dtY,1)-iii)/size(dtY,1)],'MarkerSize',20);
   end
else
   if (size(dtY,2) > 3)
      [dtY,dum1,dum2] = svd(dtY,0);
      dtY = dtY';
   else
      dtY = tY;
   end
   size(dtY)
   figure;
   plot3(dtY(1,:),dtY(2,:),dtY(3,:),'b-');
   hold on;
   axis('equal');
   %show temporal ordering by labeling plot markers
   %text(tY(1,:),tY(2,:),tY(3,:),num2str([1:size(tY,2)]'));
   %show temporal ordering by coloring plot markers
   for iii = 1:size(dtY,2)
      plot3(dtY(1,iii),dtY(2,iii),dtY(3,iii),'.','Color',[iii/size(dtY,2) 0 (size(dtY,2)-iii)/size(dtY,2)],'MarkerSize',20);
   end
end
title(sprintf('st-isomap embedding (with cctn = %f)',options.cctn));
axis('equal');
end

end %ct
 

















