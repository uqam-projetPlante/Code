% Finds the best 16 bits sync words in term of autocorrelation, DC
% balance and number of transitions.
% Author: Antoine Collerette, 2017-08-07, SPARK Microsystems.

close all; clc;
clear all;

N = 20;
% Table containing (2^16 - 1) 16 bits vectors
syncwords = zeros(65535,N);
y = 0;

% For every possible 16 bits vectors
for n = 655360:(655360+65535)
    num = n;
    % Convert num to a binary vector
    bin_vector = de2bi(num,N,'left-msb');
    % Compute autocorelation of the vector
    res = xcorr(2*bin_vector - 1);
    % Total amplitude of side lobes 
    ISL = -(N^2);
    % Number of ones (8 = DC balance)
    total_bal = 0;
    % Number of transitions 0<->1 
    total_trans = 0;
    for i = 1:(2*N-1)
        % Compute integretated sidelobe level (ISL)
        ISL = ISL + (abs(res(i))^2);
    end
    for j = 1:N
        % Count the number of ones
        total_bal = total_bal + bin_vector(j);
    end
    for k = 1:(N-1)
        % Count the number of transitions
        if ((bin_vector(k) ~= bin_vector(k+1)))
            total_trans = total_trans +1;
        end
    end
    
    % Select filter to list sync words with desired characteristics 
    %   [  starts with '1'  ]        [ISL]       [                       DC balance                       ]  [number of transitions]
    %-----------------------------------------------------------------------------------------------------------------------------------------
    %if ((bin_vector(1) == 1) && (ISL <= 240) &&                      (total_bal == 8)                      && (total_trans == 12))
    %if ((bin_vector(1) == 1) && (ISL <= 232) && ((total_bal == 8) || (total_bal == 7) || (total_bal == 9)) && (total_trans == 12))
    %if ((bin_vector(1) == 1) && (ISL <= 160) &&                      (total_bal == 8)                      && (total_trans == 11))
    %if ((bin_vector(1) == 1) && (ISL <= 152) && ((total_bal == 8) || (total_bal == 7) || (total_bal == 9)) && (total_trans == 11)) 
    if ((bin_vector(5) == 1) && (ISL <= 250)  &&                      (total_bal == 10)                      && (total_trans == 14))
    %if ((bin_vector(1) == 1) && (ISL <= 88)  && ((total_bal == 8) || (total_bal == 7) || (total_bal == 9)) && (total_trans == 10))
    %-----------------------------------------------------------------------------------------------------------------------------------------        
        y = y + 1;     
        syncwords(y,1:N) = bin_vector;    
    end   
end

lags = -(N-1):(N-1);
%figure;

% Display autocorrelation of each vectors found
for n = 1:y
    figure;
    % Converts sync words in hex (comment lines 81 to 85 in bin2hex.m file if an error occur)
    str = mat2str(syncwords(n,1:N))
    %hex_syncwords = bin2hex(strcat(str(2),str(4),str(6),str(8),str(10),str(12),str(14),str(16),str(18),str(20),str(22),str(24),str(26),str(28),str(30),str(32)));
    hex_syncwords = '';
    % Compute autocorrelation of sync words
    sync_word = syncwords(n,1:N);
    res = xcorr(2*sync_word - 1);

    % Plot each sync words
    %subplot(y,1,n);
    stem(lags, res);
    hold;
    
    % Display syncwords in hex
    title(['Sync word: ', hex_syncwords, ' ']);
    set(gca, 'fontsize', 12, 'linewidth', 1);
    xlabel('Lag', 'fontsize', 16);
    ylim([-20 20]);
    grid on;
    grid minor;
end



