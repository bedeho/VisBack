%
%  plotNeuronInvariance.m
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
%  Plots 2matrix of transform

function plotNeuronInvariance(filename, region, depth, row, col)

    % Import global variables
    declareGlobalVars();

    % Open file
    fileID = fopen(filename);
    
    % Read header
    [networkDimensions, historyDimensions, neuronOffsets] = loadHistoryHeader(fileID);

    % Get history array
    activity = neuronHistory(fileID, networkDimensions, historyDimensions, neuronOffsets, region, depth, row, col, historyDimensions.numEpochs);
    
    % Plot (timestep, transform, object, epoch)
    figure();
    for o=1:historyDimensions.numObjects,
        subplot(1, historyDimensions.numObjects, o);
        v = activity(historyDimensions.numOutputsPrTransform, :, o, historyDimensions.numEpochs);
        
        inv = zeros(sqrt(historyDimensions.numTransforms));
        
        inv(find(v)) = 1;
        
        imagesc(inv');
        
        colorbar
        colormap(jet(historyDimensions.numTransforms + 1));
        
        axis tight;
    end
    
    fclose(fileID);
    