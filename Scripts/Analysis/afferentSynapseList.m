%
%  afferentSynapseList.m
%  VisBack
%
%  Created by Bedeho Mender on 29/04/11.
%  Copyright 2011 OFTNAI. All rights reserved.
%
%  AFFERENT SYNAPSES FOR ONE NEURON
%  Input=========
%  fileID: fileID of open weight file
%  region: neuron region
%  col: neuron column
%  row: neuron row
%  depth: neuron depth
%  sourceRegion: afferent region id (V1 = 1)
%  sourceDepth: depth to plot in source region (first layer = 1)
%  Output========
%  synapses: Returns struct array of all synapses (region, depth, row, col, weight) into neuron

function [synapses] = afferentSynapseList(fileID, neuronOffsets, region, depth, row, col)

    % Import global variables
    global SOURCE_PLATFORM_USHORT;
    global SOURCE_PLATFORM_FLOAT;
   
    % Find offset of synapse list of neuron region.(depth,i,j)
    fseek(fileID, neuronOffsets{region}(row, col, depth).offset, 'bof');
    
    % Allocate synapse struct array
    afferentSynapseCount = neuronOffsets{region}(row, col, depth).afferentSynapseCount;
    synapses(afferentSynapseCount).region = [];
    synapses(afferentSynapseCount).depth = [];
    synapses(afferentSynapseCount).row = [];
    synapses(afferentSynapseCount).col = [];
    synapses(afferentSynapseCount).weight = [];
    
    % Fill synapses
    for s = 1:afferentSynapseCount,
        v = fread(fileID, 4, SOURCE_PLATFORM_USHORT);
        
        synapses(s).region = v(1)+1;
        synapses(s).depth = v(2)+1;
        synapses(s).row = v(3)+1;
        synapses(s).col = v(4)+1;
        synapses(s).weight = fread(fileID, 1, SOURCE_PLATFORM_FLOAT);
    end
