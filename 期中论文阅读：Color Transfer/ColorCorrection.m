%% 读入图片文件
img1 = imread('pic/sysu.jpg');
img2 = imread('pic/sunset.png');
% 将像素值改为标准浮点数（0~1之间）
img1 = im2double(img1);
img2 = im2double(img2);

%% 将img1和img2从RGB空间映射到l-alpha-beta空间 
[wid1, hei1, dep1] = size(img1);
img1 = rgb2lab(img1, wid1, hei1);
[wid2, hei2, dep2] = size(img2);
img2 = rgb2lab(img2, wid2, hei2);

%% 计算均值和标准差
mean_l1 = mean2(img1(:,:,1));
mean_alpha1 = mean2(img1(:,:,2));
mean_beta1 = mean2(img1(:,:,3));
mean_l2 = mean2(img2(:,:,1));
mean_alpha2 = mean2(img2(:,:,2));
mean_beta2 = mean2(img2(:,:,3));
var_l1 = std2(img1(:,:,1));
var_alpha1 = std2(img1(:,:,2));
var_beta1 = std2(img1(:,:,3));
var_l2 = std2(img2(:,:,1));
var_alpha2 = std2(img2(:,:,2));
var_beta2 = std2(img2(:,:,3));

%% 进行图像变换
% 源图像均值改为0
img1(:,:,1) = img1(:,:,1) - mean_l1;
img1(:,:,2) = img1(:,:,2) - mean_alpha1;
img1(:,:,3) = img1(:,:,3) - mean_beta1;
% 源图像方差改为目标图像方差
img1(:,:,1) = img1(:,:,1) * var_l2/var_l1;
img1(:,:,2) = img1(:,:,2) * var_alpha2/var_alpha1;
img1(:,:,3) = img1(:,:,3) * var_beta2/var_beta1;
% img1颜色通道均值保持为0，img3通道均值为img2的均值
img1(:,:,1) = img1(:,:,1) + mean_l2;

%% 修改不同的a和b均值并输出图片
img3 = img1(:,:,:);
changeMean(img3, 0.01, 0.01, 1);
changeMean(img3, 0.05, 0.01, 2);
changeMean(img3, 0.10, 0.01, 3);
changeMean(img3, 0.15, 0.01, 4);




%% 将图像从RGB空间转换到l-alpha-beta空间
function [img] = rgb2lab(img, wid, hei)
    for i = 1:wid
        for j = 1:hei
           % RGB -> LMS
           tmp = [0.3811 0.5783 0.0402;0.1967 0.7244 0.0782;0.0241 0.1228 0.8444] * ...
           [img(i,j,1);img(i,j,2);img(i,j,3)];
           % LMS -> lab
           tmp = [sqrt(1/3) 0 0;0 sqrt(1/6) 0;0 0 sqrt(1/2)] * ...
           [1 1 1;1 1 -2;1 -1 0] * ...
           [log10(tmp(1));log10(tmp(2));log10(tmp(3))];
           img(i,j,1) = tmp(1);
           img(i,j,2) = tmp(2);
           img(i,j,3) = tmp(3);
        end
    end
end

%% 将图像从l-alpha-beta空间转换到RBG空间
function [img] = lab2rgb(img, wid, hei)
    for i = 1:wid
        for j = 1:hei
           % lab -> LMS
           tmp = [1 1 1;1 1 -1;1 -2 0] * ...
           [sqrt(1/3) 0 0;0 sqrt(1/6) 0;0 0 sqrt(1/2)] * ...
           [img(i,j,1);img(i,j,2);img(i,j,3)];
           % LMS -> RGB
           tmp = [4.4679 -3.5873 0.1193;-1.2186 2.3809 -0.1624;0.0497 -0.2439 1.2045] * ...
               [10^tmp(1);10^tmp(2);10^tmp(3)];
           img(i,j,1) = tmp(1);
           img(i,j,2) = tmp(2);
           img(i,j,3) = tmp(3);
        end
   end
end

%% 修改图像的a和b均值，并输出最终结果
function [] = changeMean(img, a_mean, b_mean, pos)
    % 修改颜色通道的均值
    img(:,:,2) = img(:,:,2) + a_mean;
    img(:,:,3) = img(:,:,3) + b_mean;
    % 变换回RGB空间并输出图片结果
    [wid, hei, dep] = size(img);
    img = lab2rgb(img, wid, hei);
    subplot(2,2,pos);
    imshow(img);
    str = 'α='+ string(a_mean)+', β=' + string(b_mean);
    title(str);
end