%% ����ͼƬ�ļ�
img1 = imread('pic/sysu.jpg');
img2 = imread('pic/oldpic.png');
% ������ֵ��Ϊ��׼��������0~1֮�䣩
img1 = im2double(img1);
img2 = im2double(img2);
% ��ʾ����ԭͼ
subplot(2,2,1)
imshow(img1);
title('ԴͼƬ')
subplot(2,2,2);
imshow(img2);
title('Ŀ��ͼƬ')

%% ��img1��img2��RGB�ռ�ӳ�䵽l-alpha-beta�ռ� 
[wid1, hei1, dep1] = size(img1);
img1 = rgb2lab(img1, wid1, hei1);
[wid2, hei2, dep2] = size(img2);
img2 = rgb2lab(img2, wid2, hei2);

%% �����ֵ�ͱ�׼��
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

%% ����ͼ��任
% Դͼ���ֵ��Ϊ0
img1(:,:,1) = img1(:,:,1) - mean_l1;
img1(:,:,2) = img1(:,:,2) - mean_alpha1;
img1(:,:,3) = img1(:,:,3) - mean_beta1;
% img1�����ΪĿ��ͼ�񷽲img3�ڴ˻����ϳ���10
img1(:,:,1) = img1(:,:,1) * var_l2/var_l1;
img1(:,:,2) = img1(:,:,2) * var_alpha2/var_alpha1;
img1(:,:,3) = img1(:,:,3) * var_beta2/var_beta1;
img3 = img1(:,:,:);
img3(:,:,2) = img1(:,:,2)/10;
img3(:,:,3) = img1(:,:,3)/10;
% ͨ����ֵ��Ϊimg2�ľ�ֵ
img1(:,:,1) = img1(:,:,1) + mean_l2;
img1(:,:,2) = img1(:,:,2) + mean_alpha2;
img1(:,:,3) = img1(:,:,3) + mean_beta2;
img3(:,:,1) = img3(:,:,1) + mean_l2;
img3(:,:,2) = img3(:,:,2) + mean_alpha2;
img3(:,:,3) = img3(:,:,3) + mean_beta2;


%% ��ԴͼƬ��l-alpha-beta�ռ仹ԭ��RGB�ռ�
img1 = lab2rgb(img1, wid1, hei1);
img3 = lab2rgb(img3, wid1, hei1);

%% ��ӡ���
% �����img2һ�µ�ͼƬ
subplot(2,2,3);
imshow(img1);
title('���1��������Ŀ��ͼƬ��ͬ��')
% ����������10��ͼƬ
subplot(2,2,4)
imshow(img3);
title('���2����ɫͨ������������10��')

%% ��ͼ���RGB�ռ�ת����l-alpha-beta�ռ�
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

%% ��ͼ���l-alpha-beta�ռ�ת����RBG�ռ�
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