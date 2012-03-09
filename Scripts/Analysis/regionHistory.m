%
%  regionHistory.m
%  VisBack
%
%  Created by Bedeho Mender on 29/04/11.
%  Copyright 2011 OFTNAI. All rights reserved.
%
%  Input=========
%  fileID: fileID of open weight file
%  historyDimensions:
%  neuronOffsets: cell array giving byte offsets (rel. to 'bof') of neurons 
%  region: neuron region
%  col: neuron column
%  row: neuron row
%  depth: neuron depth
%  maxEpoch (optional): largest epoch you are interested in
%  Output========
%  Activity history of region/depth: 6-d array (timestep, transform, object, epoch, row, col) 

function [activity] = regionHistory(fileID, historyDimensions, neuronOffsets, networkDimensions, region, depth, maxEpoch)

    % Import global variables
    global SOURCE_PLATFORM_FLOAT;

    % Validate input
    validateNeuron('regionHistory.m', networkDimensions, region, depth);
    
    % Process input
    if nargin < 7,
        maxEpoch = historyDimensions.numEpochs;
    else
        if maxEpoch < 1 || maxEpoch > historyDimensions.numEpochs,
            error([file ' error: epoch ' num2str(maxEpoch) ' does not exist'])
        end
    end
    
    dimension = networkDimensions(region).dimension;
    
    % When we are looking for full epoch history, we can get it all in one chunk
    if maxEpoch == historyDimensions.numEpochs,
        
        % Seek to offset of neuron region.(depth,1,1)'s data stream
        fseek(fileID, neuronOffsets{region}(1, 1, depth).offset, 'bof');

        % Read into buffer
        streamSize = dimension * dimension * maxEpoch * historyDimensions.epochSize;
        [buffer count] = fread(fileID, streamSize, SOURCE_PLATFORM_FLOAT);
        
        if count ~= streamSize,
            error(['Read ' num2str(count) ' bytes, ' num2str(streamSize) ' expected ']);
        end

        activity = reshape(buffer, [historyDimensions.numOutputsPrTransform historyDimensions.numTransforms historyDimensions.numObjects maxEpoch dimension dimension]);
        
        % Because file is saved in row major,
        % and reshape fills in buffer in column major,
        % we have to permute the last two dimensions (row,col)
        activity = permute(activity, [1 2 3 4 6 5]);
    else
        %When we are looking for partial epoch history, then we have to
        %seek betweene neurons, so we just use neuronHistory() routine
        
        activity = zeros(historyDimensions.numOutputsPrTransform, historyDimensions.numTransforms, historyDimensions.numObjects, maxEpoch, dimension, dimension);
        
        for row=1:dimension,
            for col=1:dimension,
                activity(:, :, :, :, row, col) = neuronHistory(fileID, networkDimensions, historyDimensions, neuronOffsets, region, depth, row, col, maxEpoch);
            end
        end
    end