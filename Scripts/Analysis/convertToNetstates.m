%
% platNetworkHistory.m
% VisBack
%
% Created by Bedeho Mender on 29/04/11.
% Copyright 2011 OFTNAI. All rights reserved.
%
% Input=========
% filename: filename of firing rate file
% Output========
% Netstates file that VNFRates and Infoanalysis can use

function [netStatesFilename] = convertToNetstates(filename)

    % Import global variables
    declareGlobalVars();

    % Open file
    fileID = fopen(filename);
    
    % Read header
    [networkDimensions, historyDimensions, neuronOffsets, headerSize] = loadHistoryHeader(fileID);
    
    maxEpoch = historyDimensions.numEpochs;
    timestep = historyDimensions.numOutputsPrTransform;
    depth = 1;
    numRegions = length(networkDimensions);
    activity = cell(numRegions - 1, 1);
    
    % Get history array
    for r=2:numRegions,
        activity{r-1} = regionHistory(fileID, historyDimensions, neuronOffsets, networkDimensions, r, depth, maxEpoch);
    end
    
    fclose(fileID);
    
    % Open 
    [pathstr, name, ext] = fileparts(filename);
    netStatesFilename = [pathstr '/NetStates1'];
    netStates = fopen(netStatesFilename, 'w+');
    
        % Write Netstates header
        fprintf(netStates, 'Version 1.00\n');
        fprintf(netStates, '%d ', numRegions - 1);

        % Index numbers for each layer (pointless?)
        for r=2:numRegions,
            fprintf(netStates, '%d ', r - 2);
        end

        % Half dimension of each region
        for r=2:numRegions-1,
            fprintf(netStates, '%d ', networkDimensions(r).dimension/2);
        end
        fprintf(netStates, '%d\n', networkDimensions(numRegions).dimension/2);

        % Dummy data
        fprintf(netStates, '32 -1\n');

        % History dimensions
        fprintf(netStates, '%d %d\n', historyDimensions.numObjects, historyDimensions.numTransforms);
        
        % Dummy data
        for t=1:historyDimensions.numTransforms,
           fprintf(netStates, '%d %d\n', t-1, t-1); 
        end
    
    % Iterate data and output in netstates format
    for o=1:historyDimensions.numObjects,
        for t=1:historyDimensions.numTransforms,
            for r=2:numRegions,
                
                % Output region
                dimension = networkDimensions(r).dimension;
                
                % NBNB, check that cells that order of cols rows is correct
                a = activity{r-1}(timestep, t, o, maxEpoch, :, :);
                b = reshape(a, [dimension*dimension 1] );
                    
                fwrite(netStates, b, 'float');
            end
        end
    end
   
    fclose(netStates);
    