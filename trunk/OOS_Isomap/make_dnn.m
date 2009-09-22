function [Dnn I] = make_dnn(Di,Dj,Ds,neighborhood_size)
%[Dnn I]=make_dnn(Di,Dj,Ds,neighborhood_size)
%removes outliers, returns connected indicies in I

    N=length(Di(1,:));
    %Dnn = sparse(reshape(Di(1:neighborhood_size,:),N*neighborhood_size,1),...
    %             reshape(Dj(1:neighborhood_size,:),N*neighborhood_size,1),...
    %             reshape(Ds(1:neighborhood_size,:),N*neighborhood_size,1)); 
    Dnn=sparse(Di(1:neighborhood_size,:),Dj(1:neighborhood_size,:),Ds(1:neighborhood_size,:),N,N);
    %So that if X is a neighbor of Y, Y is also a neighbor of X
    Dnn=max(Dnn,Dnn');
    
    %Find largest Comp and remove outliers
    d=dijkstra(Dnn,1);
    others=find(d==inf);
    Comp_Size=N-length(others);
    Comp_Start=1;
    while ~isempty(others)
        if(length(others)>Comp_Size) %Might be a larger comp out there
            newstart=others(1);
            d=dijkstra(Dnn(others,others),1);
            others=others(find(d==inf));
            if length(d)-length(others) > Comp_Size;
                Comp_Size=length(d)-length(others);
                Comp_Start=newstart;
            end
        else
            others=[];
        end
    end
    %this is a repeat calc
    d=dijkstra(Dnn,Comp_Start);
    I=find(d~=inf);
    Dnn=Dnn(I,I);