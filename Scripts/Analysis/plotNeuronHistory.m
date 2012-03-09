%
%  plotNeuronHistory.m
%  VisBack
%
%  Created by Bedeho Mender on 29/04/11.
%  Copyright 2011 OFTNAI. All rights reserved.
%
%  Input=========
%  filename: filename of weight file
%  region: region to plot, V1 = 1
%  depth: region depth to plot
%  row: neuron row
%  col: neuron column
%  maxEpoch: last epoch to plot
%  Output========
%  Plots line plot of activity for spesific neuron

function plotNeuronHistory(filename, region, depth, row, col, maxEpoch)

    % Import global variables
    declareGlobalVars();

    % Open file
    fileID = fopen(filename);
    
    % Read header
    [networkDimensions, historyDimensions, neuronOffsets] = loadHistoryHeader(fileID);
    
    if nargin < 6,
        maxEpoch = historyDimensions.numEpochs; % pick all epochs
    end
    
    % Get history array
    activity = neuronHistory(fileID, networkDimensions, historyDimensions, neuronOffsets, region, depth, row, col, maxEpoch);
    
    % Plot
    v = activity(:, :, :, 1:maxEpoch);
    
    streamSize = maxEpoch * historyDimensions.epochSize;
    vect = reshape(v, [1 streamSize]);
    fig = figure();
    plot(vect);
    
    % Draw vertical divider for each transform
    if historyDimensions.numOutputsPrTransform > 1,
        x = historyDimensions.numOutputsPrTransform : historyDimensions.numOutputsPrTransform : streamSize;
        gridxy(x, 'Color', 'c', 'Linestyle', ':');
    end
    
    % Draw vertical divider for each object
    if historyDimensions.numObjects > 1,
        x = historyDimensions.objectSize : historyDimensions.objectSize : streamSize;
        gridxy(x, 'Color', 'b', 'Linestyle', '--');
    end
    
    % Draw vertical divider for each epoch
    if maxEpoch > 1,
        x = historyDimensions.epochSize : historyDimensions.epochSize : streamSize;
        gridxy(x, 'Color', 'k', 'Linestyle', '-');
    end
    
    axis([1 streamSize -.1 (max(vect)+0.2)])
    
    fclose(fileID);
    
    makeFigureFullScreen(fig);
    