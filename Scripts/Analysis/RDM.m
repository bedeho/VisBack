%
% RDM.m
% VisBack
%
% Created by Bedeho Mender on 23/02/12.
% Copyright 2012 OFTNAI. All rights reserved.
%
% Input=========
% filename: filename of firing rate file
%
% Output========
% RDM plot

function RDM(filename)

    % Import global variables
    declareGlobalVars();
    
    % Get population data
    [data, numberOfObjects, numberOfTransforms, dimension] = PopulationVectors(filename); % structure is (transform, object, row, column), look at PopulationVectors for explanation
    numberOfStimuli = numberOfObjects * numberOfTransforms;
    populationSize = dimension * dimension;
    
    % Reshufle data to get X = (outputvector,stimuli), where
    % stimuli = a particular transform of a particular stimuli, and
    % outputvector = population response.
    % Note: Order is preserved in stimuli dimension of X as required.
    permuted = permute(data,[4 3 1 2]); % [1 2 3 4] => [4 3 1 2]
    stimuliBasedData = reshape(permuted, [populationSize numberOfStimuli]);
    
    % Compute correlation coefficient for different stimuli
    corr = corrcoef(stimuliBasedData);
    
    % Compute RDM = 1 - corr
    RDM = 1 - corr;
    
    % Plot RDM
    imagesc(RDM);
    colorbar
    title('Representational Similarity Analysis');