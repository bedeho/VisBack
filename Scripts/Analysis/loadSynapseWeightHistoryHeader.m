%
%  loadSynapseWeightHistoryHeader.m
%  VisBack
%
%  Created by Bedeho Mender on 29/04/11.
%  Copyright 2011 OFTNAI. All rights reserved.
%
%  LOAD HEADER OF WEIGHT FILE
%  Input=========
%  fileID: Id of open file
%  Output========
%  networkDimensions: struct array (dimension,depth) of regions (incl. V1)
%  neuronOffsets: cell array of structs {region}{col,row,depth}.(afferentSynapseCount,offsetCount)
%  headerSize: bytes read, this is where the file pointer is left
function [networkDimensions, historyDimensions, neuronOffsets] = loadSynapseWeightHistoryHeader(fileID)

    % Import global variables
    global SOURCE_PLATFORM_USHORT;
    global SOURCE_PLATFORM_USHORT_SIZE;
    global SOURCE_PLATFORM_FLOAT_SIZE;
    
    % Seek to start of file
    frewind(fileID);
    
    % Read history dimensions & number of regions
    v = fread(fileID, 5, SOURCE_PLATFORM_USHORT);
    
    historyDimensions.numEpochs = v(1);
    historyDimensions.numObjects = v(2);
    historyDimensions.numTransforms = v(3);
    historyDimensions.numOutputsPrTransform = v(4);
    numRegions = v(5);
   
    % Compound stream sizes
    historyDimensions.transformSize = historyDimensions.numOutputsPrTransform;
    historyDimensions.objectSize = historyDimensions.transformSize * historyDimensions.numTransforms;
    historyDimensions.epochSize = historyDimensions.objectSize * historyDimensions.numObjects;
    historyDimensions.streamSize = historyDimensions.epochSize * historyDimensions.numEpochs;

    % Preallocate struct array
    networkDimensions(numRegions).dimension = [];
    networkDimensions(numRegions).depth = [];
    
    % Allocate cell data structure, NOT counting V1, {0} is left empty
    neuronOffsets = cell(numRegions,1); 
    
    % Read dimensions and setup data structure & counter
    nrOfNeurons = 0;
    for r=1:numRegions,
        dimension = fread(fileID, 1, SOURCE_PLATFORM_USHORT);
        depth = fread(fileID, 1, SOURCE_PLATFORM_USHORT);
        
        networkDimensions(r).dimension = dimension;
        networkDimensions(r).depth = depth;
        
        neuronOffsets{r}(dimension, dimension, depth).afferentSynapseCount = [];
        neuronOffsets{r}(dimension, dimension, depth).offset = [];
        
        if r > 1,
            nrOfNeurons = nrOfNeurons + dimension * dimension * depth;
        end
    end
    
    % Build list of afferentSynapse count for all neurons, and
    % cumulative sum over afferentSynapseLists up to each neuron (count),
    % this is for file seeking
    
    buffer = fread(fileID, nrOfNeurons, SOURCE_PLATFORM_USHORT);
    
    % We compute the size of header just read
    offset = SOURCE_PLATFORM_USHORT_SIZE*(5 + 2 * numRegions + nrOfNeurons);
    counter = 0;
    for r=2:numRegions,
        for d=1:networkDimensions(r).depth, % Region depth
            for row=1:networkDimensions(r).dimension, % Region row
                for col=1:networkDimensions(r).dimension, % Region col
                    
                    afferentSynapseCount = buffer(counter + 1);
                    neuronOffsets{r}(row, col, d).afferentSynapseCount = afferentSynapseCount;
                    neuronOffsets{r}(row, col, d).offset = offset;
                    
                    offset = offset + afferentSynapseCount * (4 * SOURCE_PLATFORM_USHORT_SIZE + SOURCE_PLATFORM_FLOAT_SIZE * historyDimensions.streamSize);
                    counter = counter + 1;
                end
            end
        end
    end
    
    if counter ~= nrOfNeurons,
        error('ERROR, unexpected number of neurons');
    end
    
    % We compute the size of header just read
    % headerSize = SOURCE_PLATFORM_USHORT_SIZE*(1 + 2 * numRegions + nrOfNeurons);