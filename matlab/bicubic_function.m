clc;clear;
m = 540;
n = 960;
data_num = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46];
% data_num = [1];
file = fullfile('down.mat');
load(file);
file = fullfile('picture.mat');
load(file);
ex=4;%设置放大倍数



for z = 1:size(data_num,2)
    input_num = data_num(z);
    input = eval(['dwonscale_',num2str(input_num)]);
    refer = eval(['input_',num2str(input_num)]);
    f = input;
    g1 =zeros(ex*m,ex*n,3);
    
for k = 1:3
    a=f(1,:,k);
    c=f(m,:,k);            %将待插值图像矩阵前后各扩展两行两列,共扩展四行四列
    
    
    b=[f(1,1,k),f(1,1,k),f(:,1,k)',f(m,1,k),f(m,1,k)];
    
    
    
    d=[f(1,n,k),f(1,n,k),f(:,n,k)',f(m,n,k),f(m,n,k)];
    a1=[a;a;f(:,:,k);c;c];
    b1=[b;b;a1';d;d];
    ffff=b1';
    f1=double(ffff);
    
    
for i=1:ex*m  %利用双三次插值公式对新图象所有像素赋值
       i1=floor((i+0.5)/4+0.25)+2;
       u = ((i+0.5)/4+0.25)+2-i1;
       A=[sw(1+u) sw(u) sw(u-1) sw(u-2)];  
  for j=1:ex*n
     j1=floor((j+0.5)/4+0.25)+2;
     v = ((j+0.5)/4+0.25)+2-j1;
     C=[sw(1+v);sw(v);sw(v-1);sw(v-2)];
     B=[f1(i1-1,j1-1) f1(i1-1,j1) f1(i1-1,j1+1) f1(i1-1,j1+2)
       f1(i1,j1-1)   f1(i1,j1)  f1(i1,j1+1)   f1(i1,j1+2)
       f1(i1+1,j1-1)   f1(i1+1,j1) f1(i1+1,j1+1) f1(i1+1,j1+2)
       f1(i1+2,j1-1) f1(i1+2,j1) f1(i1+2,j1+1) f1(i1+2,j1+2)];
   
   
%      B1 = f1(i1-1,j1-1)*sw(1+v)+f1(i1-1,j1)*sw(v)+f1(i1-1,j1+1)*sw(1-v)+f1(i1-1,j1+2)*sw(2-v);
%      B2 = f1(i1,j1-1)*sw(1+v)+f1(i1,j1)*sw(v)+f1(i1,j1+1)*sw(1-v)+f1(i1,j1+2)*sw(2-v);
%      B3 = f1(i1+1,j1-1)*sw(1+v)+f1(i1+1,j1)*sw(v)+f1(i1+1,j1+1)*sw(1-v)+f1(i1+1,j1+2)*sw(2-v);
%      B4 = f1(i1+2,j1-1)*sw(1+v)+f1(i1+2,j1)*sw(v)+f1(i1+2,j1+1)*sw(1-v)+f1(i1+2,j1+2)*sw(2-v);
     g1(i,j,k)=A*B*C;
   end
end
end


    g=uint8(g1);
    % g = imbilatfilt(g);
    peaksnr(1,z) = psnr(g,refer);
    ssimda(1,z) = ssim(g,refer);
end


function A = sw(w1)

    w = abs(w1);
    a = -0.5;
    if w <1 && w >=0
        A = 1 - (a+3)*w^2 + (a+2)*w^3;
    else if w>=1 && w<2
        A = a*w^3 - 5*a*w^2 + (8*a)*w - 4*a;
        else
        A = 0;
        end
    end
end