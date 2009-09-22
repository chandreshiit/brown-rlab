function E=MDS_Transform_Landmark(newdists,sum2,sumd,vec,val)
%Y=MDS_Transform_Landmark(newdists,sum2,sumd,vec,val)
%Compute the embedding of the point defined by newdists
%vec and val are the eigen vecs and vals computed by the original transform
%dists are the distances between landmarks
%sumd = sum(GD.^2)
%sum2 = sum(sumd)


%implemented from Bengio
tmp = length(newdists)+1;

fromtmp = (sum(newdists.^2));
%expected = (sum(sum(dists.^2)) + 2*fromtmp)/(tmp*tmp);
expected = (sum2 + 2*fromtmp)/(tmp*tmp);

fromtmp = fromtmp/tmp;
diff = expected - fromtmp;

%sumd=sum(dists.^2);

K = length(val);
Ktild = zeros(tmp,1);
for i=1:length(Ktild)-1
    %k between newpoint and point i
    %Ktild(i) = -0.5*(newdists(i).^2 -((sum(dists(:,i).^2)+newdists(i))/tmp) +diff);
    Ktild(i) =  -0.5*(newdists(i).^2 -((sumd(i)           +newdists(i))/tmp) +diff);
end
%what's this for?
%Ktild(tmp) = -0.5*(expected - fromtmp -((sum(newdists.^2))/tmp));
Ktild(tmp) = -0.5*(diff - fromtmp);

Y = zeros(K,1);
E = zeros(K,1);
for k=1:K;
    %    for i=1:length(vec(:,k))
    %    Y(k) = Y(k) + vec(i,k)*Ktild(i);
    %end
    %size(vec)
    %size(Ktild)
    Y(k)=vec(:,k)'*Ktild(1:end-1);
    Y(k) = Y(k)/val(k);
    E(k) = Y(k)*sqrt(val(k));
end