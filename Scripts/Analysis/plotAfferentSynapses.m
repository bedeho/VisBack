%
%  plotAfferentSynapses.m
%  VisBack
%
%  Created by Bedeho Mender on 29/04/11.
%  Copyright 2011 OFTNAI. All rights reserved.
%
% Input=========
% filename: filename of weight file
% region: neuron region, V1 = 1
% col: neuron column
% row: neuron row
% depth: neuron depth
% sourceRegion: afferent region id (V1 = 1)
% sourceDepth: depth to plot in source region (first layer = 1)
% Output========
% Sums the 2d connectivity matrixes for all synapeses to row x col in region
% from sourceRegion at SourceDepth

function plotAfferentSynapses(filename, region, depth, row, col, sourceRegion, sourceDepth)

    % Import global variables
    declareGlobalVars();

    % Open file
    fileID = fopen(filename);
    
    % Read header
    [networkDimensions, neuronOffsets] = loadWeightFileHeader(fileID);
    
    regionDimension = networkDimensions(region).dimension;
    
    if nargin < 7,
        
        sourceDepth = 1;                                % pick top layer
        
        if nargin < 6
            sourceRegion = region - 1;                  % pick region below
        
            % If no planar cordinate of cell are provided, pick full layer
            if nargin < 4
                rowRange = 1:regionDimension;
                colRange = 1:regionDimension;
                
                if nargin < 3
                    depth = 1;
                end
            else
                rowRange = row:row;
                colRange = col:col;
            end
        end
    end
    
    sourceRegionDimension = networkDimensions(sourceRegion).dimension;
    
    weightBox = zeros(sourceRegionDimension,sourceRegionDimension);

    for i=rowRange, % Region row
        for j=colRange, % Region col
            % Get afferent synapse matrix
            weightBox = weightBox + afferentSynapseMatrix(fileID, networkDimensions, neuronOffsets, region, depth, i, j, sourceRegion, sourceDepth);
        end
    end
    
    % Plot
    imagesc(weightBox);
    colorbar
    
    fclose(fileID);