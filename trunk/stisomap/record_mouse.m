figure;
plot([1 1 1 1],[1 2 3 4],'r.','MarkerSize',20);
hold on;
plot([-2 4],[0.5 4.5],'w.');
%[gx,gy] = ginput;
%gc = [gx gy];
gc = [1 1];
maxp = 400;
for i = 2:maxp
   [gx,gy] = ginput(1);
   gc = [gc ; [gx gy]];
   plot(gc(i-1:i,1),gc(i-1:i,2),'b.');
   title(sprintf('%d / %d',i,maxp));
end
gc = gc(2:end,:);


