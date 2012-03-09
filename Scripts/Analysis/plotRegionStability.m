%
%  plotRegionStability.m
%  VisBack
%
%  Created by Bedeho Mender on 29/04/11.
%  Copyright 2011 OFTNAI. All rights reserved.
%

% PLOT REGION STABILITY HISTORY
% Input=========
% filename: filename of weight file
% region: region to plot, V1 = 1
% depth: region depth to plot
% row: neuron row
% col: neuron column
%  maxEpoch (optional): largest epoch you are interested in
% Output========
%
% 'D:\Oxford\Work\Projects\VisBack\Simulations\1Object\1Epoch\firingRate.dat'

function [] = plotRegionStability(filename, region, depth, objects, maxEpoch)

   % Import global variables
    declareGlobalVars();

    % Open file
    fileID = fopen(filename);
    
    % Read header
    [networkDimensions, historyDimensions, neuronOffsets, headerSize] = loadHistoryHeader(fileID);
    
    % Fill in missing arguments, 
    if nargin < 5,
        maxEpoch = historyDimensions.numEpochs;             % pick all epochs

        if nargin < 4,
            objects = 1:historyDimensions.numObjects;       % pick all transforms

            if nargin < 3,
                depth = 1;                                  % pick first layer
            end
        end
    end
    
    lastTimeStep = historyDimensions.numOutputsPrTransform;        % pick last output     

    % Get history array
    activity = regionHistory(fileID, historyDimensions, neuronOffsets, networkDimensions, region, depth, maxEpoch);
    
    % Plot
    plotDim = ceil(sqrt(historyDimensions.numTransforms));
    stability = zeros(maxEpoch - 1); % To plot
    
    fig = figure();
    
    for t=1:historyDimensions.numTransforms, % pick all transforms

        subplot(plotDim, plotDim, t);

        for o=objects,
            
            pastActive = find(activity(lastTimeStep, t, o, 1, :, :)); % Save first epoch

            for e=2:maxEpoch,
                presentActive = find(activity(lastTimeStep, t, o, e, :, :));        % Find new activity
                stability(e - 1) = newValuesInSecond(pastActive, presentActive);    % Find overlap in new and old activity
                pastActive = presentActive;                                         % Save new activity
            end
            
            plot(stability);
            title(['Transform : ', num2str(t)]);
            hold on;
        end
    end
    
    fclose(fileID);
    
    makeFigureFullScreen(fig);
    
    function [res] = newValuesInSecond(first, second)
        res = 0;
        for s=1:length(second),
            
            new = true; % assume second(s) is not present in first
            for f=1:length(first),
                if(second(s) == first(f)), new = false; end
            end
            
            if(new), res = res + 1; end
        end
        