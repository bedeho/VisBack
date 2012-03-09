%
%  plotRegionConfusion.m
%  VisBack
%
%  Created by Bedeho Mender on 29/04/11.
%  Copyright 2011 OFTNAI. All rights reserved.
%
%  PLOT REGION INVARIANCE
%  Input=========
%  filename: filename of weight file
%  region: region to plot, V1 = 1
%  depth: region depth to plot
%  Output========


function plotRegionConfusion(filename, region, depth)

    % Import global variables
    declareGlobalVars();

    % Open file
    fileID = fopen(filename);
    
    % Read header
    [networkDimensions, historyDimensions, neuronOffsets, headerSize] = loadHistoryHeader(fileID);
    
    % Fill in missing arguments    
    if nargin < 3,
        depth = 1;                                  % pick top layer
        
        if nargin < 2,
            region = length(networkDimensions);     % pick last region
        end
    end
    
    if region < 2,
        error('Region is to small');
    end
    
    numEpochs = historyDimensions.numEpochs;
    numObjects = historyDimensions.numObjects;
    numTransforms = historyDimensions.numTransforms;
    regionDimension = networkDimensions(region).dimension;
    
    confusion = zeros(numObjects, numObjects);

    floatError = 0.1;
    
    regionActivity = regionHistory(fileID, historyDimensions, neuronOffsets, networkDimensions, region, depth, numEpochs);
    
    objectThreshold = 1;

    for o1=1:numObjects,
        for o2=1:numObjects,

            activity_o1 = regionActivity(historyDimensions.numOutputsPrTransform, :, o1, numEpochs, :, :);
            activity_o2 = regionActivity(historyDimensions.numOutputsPrTransform, :, o2, numEpochs, :, :);
            
            q_o1 = find(squeeze(sum(activity_o1 > floatError) >= objectThreshold)); %find indices of all cells responding to some transform of o1
            q_o2 = find(squeeze(sum(activity_o2 > floatError) >= objectThreshold)); %find indices of all cells responding to some transform of o2
            
            if o1 ~= o2,
                confusion(o1,o2) = length(intersect(q_o1, q_o2));
            end
        end
    end
    
    fig = figure();
    imagesc(confusion);
    colorbar

    fclose(fileID);
    
    makeFigureFullScreen(fig);

    