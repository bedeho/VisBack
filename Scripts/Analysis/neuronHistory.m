%
%  neuronHistory.m
%  VisBack
%
%  Created by Bedeho Mender on 29/04/11.
%  Copyright 2011 OFTNAI. All rights reserved.
%
%  Input=========
%  fileID: fileID of open weight file
%  networkDimensions: 
%  historyDimensions: 
%  neuronOffsets: cell array giving byte offsets (rel. to 'bof') of neurons 
%  region: neuron region
%  col: neuron column
%  row: neuron row
%  depth: neuron depth
%  maxEpoch (optional): largest epoch you are interested in
%  Output========
%  Activity history of region: 4-d matrix (timestep, transform, object, epoch)

function [activity] = neuronHistory(fileID, networkDimensions, historyDimensions, neuronOffsets, region, depth, row, col, maxEpoch)

    % Import global variables
    global SOURCE_PLATFORM_FLOAT;

    % Validate input
    validateNeuron('neuronHistory.m', networkDimensions, region, depth, row, col);
      
    if maxEpoch < 1 || maxEpoch > historyDimensions.numEpochs,
        error([file ' error: epoch ' num2str(maxEpoch) ' does not exist'])
    end
    
    % Seek to offset of neuron region.(depth,i,j)'s data stream
    fseek(fileID, neuronOffsets{region}(row, col, depth).offset, 'bof');
    
    % Read into buffer
    streamSize = maxEpoch * historyDimensions.epochSize;
    [buffer count] = fread(fileID, streamSize, SOURCE_PLATFORM_FLOAT);
    
    if count ~= streamSize,
        error(['Read ' num2str(count) ' bytes, ' num2str(streamSize) ' expected ']);
    end
    
    % Make history array
    activity = reshape(buffer, [historyDimensions.numOutputsPrTransform historyDimensions.numTransforms historyDimensions.numObjects maxEpoch]);