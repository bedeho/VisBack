%
%  plotRegionPercentile.m
%  VisBack
%
%  Created by Bedeho Mender on 29/04/11.
%  Copyright 2011 OFTNAI. All rights reserved.
%
%  PLOT REGION PERCENTILE
%  Input=========
%  filename: filename of weight file
%  Output========
%

function [fig] = plotRegionPercentile(filename)

    % Import global variables
    declareGlobalVars();
    
    % Import global variables
    global SOURCE_PLATFORM_FLOAT;

    % Open file
    fileID = fopen(filename);
    
    % Read header
    [networkDimensions, historyDimensions, neuronOffsets, headerSize] = loadHistoryHeader(fileID);
    numRegions = length(networkDimensions) - 1;
    streamSize = historyDimensions.streamSize;
    
    % Load in all data
    rawStream = fread(fileID, numRegions * streamSize, SOURCE_PLATFORM_FLOAT);
    percentileStreams = reshape(rawStream, [streamSize numRegions]);
    
    % Setup figure
    fig = figure();
    
    % Do one plot pr. region
    for r = 1:numRegions,
        
        subplot(numRegions,1,r);
        
        % Plot
        plot(squeeze(percentileStreams(:, r)));

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
        if historyDimensions.numEpochs > 1,
            x = historyDimensions.epochSize : historyDimensions.epochSize : streamSize;
            gridxy(x, 'Color', 'k', 'Linestyle', '-');
        end
    
    end
    
    fclose(fileID);

    