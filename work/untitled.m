clear all;
close all;

tbl = readmatrix('tbl.txt');
x = readmatrix('graf.txt');

len_t = (1 : length(tbl))';
len_x = (1 : length(x))';

figure;
hold on;
plot(len_x, x);
plot(len_t, tbl,'o');
title('sinus');
legend('x', 'tbl');
hold off;