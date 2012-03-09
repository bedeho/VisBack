%
% PopulationVectors.m
% VisBack
%
% Created by Bedeho Mender on 23/02/12.
% Copyright 2012 OFTNAI. All rights reserved.
%
% Input=========
% filename: filename of firing rate file
%
% Output========
% data = array structured as (transform,object,row,column),
%        so to get th response of neuron row=2,column=15 in the output
%        layer to transform=3 of object=5 you look at data(3,5,2,15).
%        Likewise to get the population vector for transform=3 of object=5
%        you look at data(3,5,:,:)
%
% numberOfObjects = number of objects
% numberOfTransforms = number of transforms

function [data, numberOfObjects, numberOfTransforms, dimension] = PopulationVectors(filename)

    % Import global variables
    declareGlobalVars();

    % Open file
    fileID = fopen(filename);
    
    % Read header
    [networkDimensions, historyDimensions, neuronOffsets, headerSize] = loadHistoryHeader(fileID);
    
    % Region dimension
    lastOutput = historyDimensions.numOutputsPrTransform;
    lastEpoch = historyDimensions.numEpochs;
    lastLayer = length(networkDimensions);
    
    % Get history array
    activity = regionHistory(fileID, historyDimensions, neuronOffsets, networkDimensions, lastLayer, 1, lastEpoch);
 
    % Project out last response in each transform for each object for each
    % cell
    data = squeeze(activity(lastOutput, :, :, lastEpoch, :, :));
    
    % Some extra helpfull data
    numberOfObjects = historyDimensions.numObjects;
    numberOfTransforms = historyDimensions.numTransforms;
    dimension = networkDimensions(end).dimension;

    fclose(fileID);