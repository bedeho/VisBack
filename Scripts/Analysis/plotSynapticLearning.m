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
%  maxEpoch: last epoch to plot
%  Output========
%  Plots line plot of activity for spesific neuron

function plotSynapticLearning(folder, region, depth, maxEpoch)

    % Import global variables
    declareGlobalVars();
    
    synapseFile = [folder '/synapticWeights.dat'];
    
    % Open file
    fileID = fopen(synapseFile);
    
    % Read header
    [networkDimensions, historyDimensions, neuronOffsets] = loadSynapseWeightHistoryHeader(fileID);
    
    if nargin < 4,
        maxEpoch = historyDimensions.numEpochs; % pick all epochs
    end
    
    streamSize = maxEpoch * historyDimensions.epochSize;
    dimension = networkDimensions(region).dimension;
    learningMetric = zeros(dimension, dimension);
    
    % Iterate all cells
    for row=1:dimension,
        for col=1:dimension,
            
            % Get history array
            synapses = synapseHistory(fileID, networkDimensions, historyDimensions, neuronOffsets, region, depth, row, col, maxEpoch);
            
            variances = zeros(length(synapses),1);
            for s=1:length(synapses),

                v = synapses(s).activity(:, :, :, 1:maxEpoch);
                variances(s) = var(reshape(v, [1 streamSize]));
            end
            
            %plotSynapseHistory(folder, region, depth, row, col, maxEpoch);
            %variances
            %length(find(variances > 0.001))/length(synapses)
            %pause
            learningMetric(row, col) = length(find(variances > 0.00001))/length(synapses);
        end
    end
    
    figure();
    imagesc(learningMetric);
    colorbar

    fclose(fileID);
    