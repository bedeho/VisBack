%
%  synapseHistory.m
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
%  struct array of synapse activities: synapse(row,col,depth,activity = 4-d
%  matrix (timestep, transform, object, epoch)

function [synapses] = synapseHistory(fileID, networkDimensions, historyDimensions, neuronOffsets, region, depth, row, col, maxEpoch)

    % Import global variables
    global SOURCE_PLATFORM_USHORT;
    global SOURCE_PLATFORM_FLOAT;
    global SOURCE_PLATFORM_FLOAT_SIZE;
    
    % Validate input
    validateNeuron('synapseHistory.m', networkDimensions, region, depth, row, col);
      
    if maxEpoch < 1 || maxEpoch > historyDimensions.numEpochs,
        error([file ' error: epoch ' num2str(maxEpoch) ' does not exist'])
    end
   
    % Find offset of synapse list of neuron region.(depth,i,j)
    fseek(fileID, neuronOffsets{region}(row, col, depth).offset, 'bof');
    
    % Allocate synapse struct array
    count = neuronOffsets{region}(row, col, depth).afferentSynapseCount;
    synapses(count).region = [];
    synapses(count).depth = [];
    synapses(count).row = [];
    synapses(count).col = [];
    synapses(count).activity = [];
    
    % Read into buffer
    streamSize = maxEpoch * historyDimensions.epochSize;
    
    %h = waitbar(0,'Loading Synapses History...');
    
    % Fill synapses
    for s = 1:count,
        
        %waitbar(s/count,h);
        
        v = fread(fileID, 4, SOURCE_PLATFORM_USHORT);
        
        synapses(s).region = v(1)+1;
        synapses(s).depth = v(2)+1;
        synapses(s).row = v(3)+1;
        synapses(s).col = v(4)+1;
        
        buffer = fread(fileID, streamSize, SOURCE_PLATFORM_FLOAT);
        synapses(s).activity = reshape(buffer, [historyDimensions.numOutputsPrTransform historyDimensions.numTransforms historyDimensions.numObjects maxEpoch]);
        
        % If we did not consume full stream, we must fseek to stream
        % for next neuron
        if maxEpoch < historyDimensions.numEpochs,
             fseek(fileID, (historyDimensions.numEpochs - maxEpoch)*historyDimensions.epochSize * SOURCE_PLATFORM_FLOAT_SIZE, 'cof');
        end
    end
    
    %close(h);
