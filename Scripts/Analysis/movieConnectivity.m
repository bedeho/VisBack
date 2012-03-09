%
%  plotSynapseHistory.m
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

function plotSynapseHistory(folder, region, depth, row, col, maxEpoch)

    % Import global variables
    declareGlobalVars();
    
    synapseFile = [folder '/synapticWeights.dat'];
    
    % Open file
    fileID = fopen(synapseFile);
    
    % Read header
    [networkDimensions, historyDimensions, neuronOffsets] = loadSynapseWeightHistoryHeader(fileID);
    
    if nargin < 6,
        maxEpoch = historyDimensions.numEpochs; % pick all epochs
    end
    
    streamSize = maxEpoch * historyDimensions.epochSize;
    
    % Get history array
    synapses = synapseHistory(fileID, networkDimensions, historyDimensions, neuronOffsets, region, depth, row, col, maxEpoch);
    
    %format('longE'); % output full floats, no rounding!!
    
    figure();
    for s=1:length(synapses),

        % Plot
        v = synapses(s).activity(:, :, :, 1:maxEpoch);
        plot(reshape(v, [1 streamSize]));
        hold on;
    end
    
    fclose(fileID);
    
    %=====================================================================================================================
    % FIRING
    %=====================================================================================================================
    
    firingRateFile = [folder '/firingRate.dat'];
    
    % Open file
    fileID = fopen(firingRateFile);
    
    % Read header
    [networkDimensions, historyDimensions, neuronOffsets] = loadHistoryHeader(fileID);
    
    % Get history array
    activity = neuronHistory(fileID, networkDimensions, historyDimensions, neuronOffsets, region, depth, row, col, maxEpoch);
    
    % Plot
    v = activity(:, :, :, 1:maxEpoch);
    
    streamSize = maxEpoch * historyDimensions.epochSize;
    vect = reshape(v, [1 streamSize]);
    plot(vect,'r');
    hold on;

    fclose(fileID);

    
    axis tight;
    