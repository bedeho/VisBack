%
%  afferentSynapseMatrix.m
%  VisBack
%
%  Created by Bedeho Mender on 29/04/11.
%  Copyright 2011 OFTNAI. All rights reserved.
%
%  BUILD SYNAPTIC WEIGHT MATRIX FOR A PARTICULAR DEPTH
%  Input=========
%  fileID: file id of open file
%  headerSize: byte size of full header
%  list: struct array (afferentSynapseCount,offsetCount) of neurons
%  region: neuron region, V1 = 1
%  col: neuron column
%  row: neuron row
%  depth: neuron depth
%  sourceRegion: afferent region id (V1 = 1)
%  sourceDepth: depth to plot in source region (first layer = 1)
%  Output========

function [weightBox] = afferentSynapseMatrix(fileID, networkDimensions, neuronOffsets, region, depth, row, col, sourceRegion, sourceDepth)

    % Read file
    synapses = afferentSynapseList(fileID, neuronOffsets, region, depth, row, col);
    
    % Weight box
    sourceRegionDimension = networkDimensions(sourceRegion).dimension;
    weightBox = zeros(sourceRegionDimension, sourceRegionDimension);
    
    for s = 1:length(synapses),
        if synapses(s).region == sourceRegion && synapses(s).depth == sourceDepth
            weightBox(synapses(s).row, synapses(s).col) = synapses(s).weight;
        end
    end