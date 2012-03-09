%
% platNetworkHistory.m
% VisBack
%
% Created by Bedeho Mender on 29/04/11.
% Copyright 2011 OFTNAI. All rights reserved.
%
% Input=========
% filename: filename of weight file
% depth: 
% maxEpoch (optional): last epoch to plot
% Output========
% Plots one 2d activity plot of network pr. output

function plotNetworkHistory(filename, depth, maxEpoch)

    % Import global variables
    declareGlobalVars();

    % Open file
    fileID = fopen(filename);
    
    % Read header
    [networkDimensions, historyDimensions, neuronOffsets, headerSize] = loadHistoryHeader(fileID);
    
    % Fill in missing arguments
    if nargin < 3,
        maxEpoch = historyDimensions.numEpochs;           % pick all epochs
        
        if nargin < 2,
            depth = 1;                                      % pick top layer by default
        end
    end

    numRegions = length(networkDimensions);
    activity = cell(numRegions - 1, 1);
    
    % Get history array
    for r=2:numRegions,
        activity{r-1} = regionHistory(fileID, historyDimensions, neuronOffsets, networkDimensions, r, depth, maxEpoch);
    end
    
    % Plot
    for e=1:maxEpoch,
        for o=1:historyDimensions.numObjects,
            for t=1:historyDimensions.numTransforms,

                fig = figure();
                title(['Epoch: ', num2str(e), ', Object:', num2str(o), ', Transform:', num2str(t)]);
                plotCounter = 1;
                
                for r=2:numRegions,
                    
                    dimension = networkDimensions(r).dimension;
                    
                    for ti=1:historyDimensions.numOutputsPrTransform,

                        subplot(numRegions-1, historyDimensions.numOutputsPrTransform, plotCounter);

                        a = activity{r-1}(ti, t, o, e, :, :);
                        imagesc(reshape(a, [dimension dimension]));
                        axis square;
                        %colorbar
                        hold on;

                        plotCounter = plotCounter + 1;
                    end
                end

                makeFigureFullScreen(fig);
                pause
            end
        end
    end 
   
    fclose(fileID);
