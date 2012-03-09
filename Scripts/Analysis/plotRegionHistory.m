%
% platRegionHistory.m
% VisBack
%
% Created by Bedeho Mender on 29/04/11.
% Copyright 2011 OFTNAI. All rights reserved.
%
% Input=========
% filename: filename of weight file
% region: region to plot
% depth: region depth to plot
% maxEpoch (optional): last epoch to plot
% Output========
% Plots one 2d activity plot of region/depth pr. transform in each figure and one figure for each epoch, always 
% picks last outputted time step of every transform

function plotRegionHistory(filename, region, depth, maxEpoch)

    % Import global variables
    declareGlobalVars();

    % Open file
    fileID = fopen(filename);
    
    % Read header
    [networkDimensions, historyDimensions, neuronOffsets, headerSize] = loadHistoryHeader(fileID);
    
    % Fill in missing arguments
    if nargin < 4,
        maxEpoch = historyDimensions.numEpochs;           % pick all epochs
        
        if nargin < 3,
            depth = 1;                                      % pick top layer by default
        end
    end

    dimension = networkDimensions(region).dimension;
    
    % Get history array
    activity = regionHistory(fileID, historyDimensions, neuronOffsets, networkDimensions, region, depth, maxEpoch);
    
    % Plot
    plotDim = ceil(sqrt(historyDimensions.numTransforms));
    
    for e=1:maxEpoch,
        for o=1:historyDimensions.numObjects,
            
            figure();
            title(['Epoch: ', num2str(e), ', Object:', num2str(o), ', Tick: LAST']);
             for t=1:historyDimensions.numTransforms,

                subplot(plotDim,plotDim, t);
                a = activity(historyDimensions.numOutputsPrTransform, t, o, e, :, :);
                imagesc(reshape(a, [dimension dimension]));
                colorbar
                title(['Transform:', num2str(t)]);
                hold on;
             end
        end
    end 
   
    fclose(fileID);