function [data3,data2,data1] = genRippleSwissRoll(inoutR, turns, width, inoutF, amp, numpts)
%[3d 2d 1d] = genSwissRoll([innerRadius outerRadius],turns,width,[innerFreq outerFreq],amplitude,numpts)
innerR=inoutR(1);
outerR=inoutR(2);
innerF=inoutF(1);
outerF=inoutF(2);

%generate first dimension
x=rand(1,numpts);

%convert to standard polar swiss roll
theta=x*2*turns*pi;  %set number of turns
rho=x*(outerR-innerR)+innerR; %set in and outer radii

%add the ripple
theta2=x*pi;
frequency=x*(outerF-innerF)+innerF; %frequency should change to stay even
offset=amp*sin(frequency.*theta2); %set amplitude
rho=rho+offset;

y=rand(1,numpts);

%should x and y be converted to geodesic length?
data1=x;
data2=[x;y];
data3=[rho.*cos(theta);rho.*sin(theta);y*width];

