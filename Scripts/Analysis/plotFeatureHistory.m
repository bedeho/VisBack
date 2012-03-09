%
%  plotFeatureHistory.m
%  VisBack
%
%  Created by Bedeho Mender on 29/04/11.
%  Copyright 2011 OFTNAI. All rights reserved.
%
%  Input=========
%  filename: filename of weight file
%  region: region to plot, V1 = 1
%  depth: region depth to plot
%  row: neuron row
%  col: neuron column
%  maxEpoch: last epoch to plot
%  Output========
%  Plots line plot of activity for spesific neuron

function plotFeatureHistory(folder, region, depth, row, col, maxEpoch)

    % Import global variables
    declareGlobalVars();
    
    synapseFile = [folder '/synapticWeights.dat'];
    
    % Open file
    fileID = fopen(synapseFile);
    
    % Read header
    [networkDimensions, historyDimensions, neuronOffsets] = loadSynapseWeightHistoryHeader(fileID);
    
    if nargin < 6,
        maxEpoch = historyDimensions.numEpochs; % pick all epochs
    end
    
    streamSize = maxEpoch * historyDimensions.epochSize;
    
    V1Dimension = networkDimensions(1).dimension;
    
    % Setup plotting figure and movie matrix
    fig = figure();
    initCanvas(V1Dimension); % we must call this first so we can record the proper size with get()
    %movegui(fig); % move to main screen, showing on secondary screen in dual screen trips up get().
    %winsize = get(fig, 'Position');
    
    %moviein doesnt work, lets make what moviein
    %would do!
    %A = cell(2,1)%moviein(streamSize, fig, winsize);
    %for i=1:
    %A{1} = uint8(zeros(winsize(4), winsize(3), 3));
    
    % Get history array
    sources = findV1Sources(fileID, networkDimensions, historyDimensions, neuronOffsets, region, depth, row, col, maxEpoch);

    % Iterate history,
    frameCounter = 1;
    h = waitbar(0,'Building Movie...');
    
    for e=1:maxEpoch,
        
        waitbar(e/maxEpoch,h);
        
        for o=1:historyDimensions.numObjects,
            for t=1:historyDimensions.numTransforms,
                for ti=1:historyDimensions.numOutputsPrTransform,
                    
                    % Clears old content and sets up right axis dimensions
                    initCanvas(V1Dimension);
                    
                    % Iterate all afferent synapses above threshold for
                    % this time step and draw
                    
                    synapses = sources{ti, t, o, e};
                    
                    for s=1:length(synapses),
                        drawFeature(synapses(s).row, synapses(s).col, synapses(s).depth, synapses(s).compound);
                    end
                end
            end
        end
        
       A(frameCounter) = getframe(fig);
       frameCounter = frameCounter + 1;
        
    end
    
    close(h);
    
    % Play movie
    movie(fig, A, 5, 3)
    
    % Save movie
    save filename.mat A
    
    fclose(fileID);

% sources = cell  of struct arrays{timestep, transform, object, epoch} => (1..n_i).(col,row,depth, productWeight)  
function [sources] = findV1Sources(fileID, networkDimensions, historyDimensions, neuronOffsets, region, depth, row, col, maxEpoch)

        THRESHOLD = 0.1;

        % Allocate space for history
        sources = cell(historyDimensions.numOutputsPrTransform, historyDimensions.numTransforms, historyDimensions.numObjects, maxEpoch);
            
        if region == 1, % termination condition, V1 cells return them self
            
            % Iterate history,
            for e=1:maxEpoch,
                for o=1:historyDimensions.numObjects,
                    for t=1:historyDimensions.numTransforms,
                        for ti=1:historyDimensions.numOutputsPrTransform,
                            
                            % Make 1x1 struct array
                            v(1).region = region;
                            v(1).row = row;
                            v(1).col = col;
                            v(1).depth = depth;
                            v(1).compound = 1;
                            
                            sources{ti, t, o, e} = v;
                        end
                    end
                end
            end
      elseif region > 1, 
        
            synapses = synapseHistory(fileID, networkDimensions, historyDimensions, neuronOffsets, region, depth, row, col, maxEpoch);
            afferentSynapseCount = length(synapses);
            
            childSources = cell(afferentSynapseCount, 1);
            
            % Notice that we add +1 since the numbers from the file is in
            % 0 based C++ indexing
            for s=1:afferentSynapseCount % For each child
                childSources{s} = findV1Sources(fileID, networkDimensions, historyDimensions, neuronOffsets, synapses(s).region, synapses(s).depth, synapses(s).row, synapses(s).col, maxEpoch);
            end
            
            % Iterate history,
            for e=1:maxEpoch,
                for o=1:historyDimensions.numObjects,
                    for t=1:historyDimensions.numTransforms,
                        for ti=1:historyDimensions.numOutputsPrTransform,
                            
                            % For each synapse, look up pool of features
                            % it is connected to for present time, and
                            % include in .this neurons pool for this time
                            % given certain critirea, e.g. if synapse is
                            % strong enough, or compund weight is strong
                            % enough etc.
                            
                            % Count the number of neurons we will have
                            childSourcesSize = 0;
                            for s=1:afferentSynapseCount, 
                                
                                if synapses(s).activity(ti, t, o, e) > THRESHOLD,
                                    childSourcesSize = childSourcesSize + length(childSources{s}{ti, t, o, e});
                                end
                            end
                            
                            % Make childSourcesSizex1 struct array
                            if childSourcesSize > 0,
                                v(childSourcesSize).region = region;
                                v(childSourcesSize).row = row;
                                v(childSourcesSize).col = col;
                                v(childSourcesSize).depth = depth;
                                v(childSourcesSize).compound = 1;

                                sources{ti, t, o, e} = v;

                                counter = 1;
                                for s=1:afferentSynapseCount, 

                                    % If this synapse is stronger then
                                    % threshold at this time, then we
                                    % included properly, otherwise we put 0 in
                                    % its place
                                    if synapses(s).activity(ti, t, o, e) > THRESHOLD,

                                         for cs=1:length(childSources{s}{ti, t, o, e}),
                                             sources{ti, t, o, e}(counter).region = childSources{s}{ti, t, o, e}(cs).region;
                                             sources{ti, t, o, e}(counter).row = childSources{s}{ti, t, o, e}(cs).row;
                                             sources{ti, t, o, e}(counter).col = childSources{s}{ti, t, o, e}(cs).col;
                                             sources{ti, t, o, e}(counter).depth = childSources{s}{ti, t, o, e}(cs).depth;
                                             sources{ti, t, o, e}(counter).compound = childSources{s}{ti, t, o, e}(cs).compound * synapses(s).activity(ti, t, o, e);
                                             counter = counter + 1;    
                                         end
                                    end
                                end
                            else
                                sources{ti, t, o, e} = [];
                            end
                        end
                    end
                end
            end
        end
    
function initCanvas(dimension)
        
    clf
    axis([0 dimension+1 0 dimension+1]); 
    hold on; % hold axis
    %hold on;
    %axis[0, (dimension-1), 0, (dimension-1)];
    %axis tight;

function drawFeature(row, col, depth, weight)

    halfSegmentLength = 0.5;
    [orrientation, wavelength, phase] = decodeDepth(depth);
    featureOrrientation = orrientation + 90; % orrientation is the param to the filter, but it corresponds to a perpendicular image feature
    
    dx = halfSegmentLength * cos(deg2rad(featureOrrientation));
    dy = halfSegmentLength * sin(deg2rad(featureOrrientation));
    
    x1 = col - dx;
    x2 = col + dx;
    y1 = row - dy;
    y2 = row + dy;
    plot([x1 x2], [y1 y2]);
    hold on;
    

function [orrientation, wavelength, phase] = decodeDepth(depth)

    Phases = [0, 180, -90, 90];
    Orrientations = [0, 45, 90, 135];
    Wavelengths = [4];
    
    depth = uint8(depth)-1; % These formula expect C indexed depth, since I copied from project

    w = mod((idivide(depth, length(Phases))), length(Wavelengths));
    wavelength = Wavelengths(w+1);
    
    ph = mod(depth, length(Phases));
    phase = Phases(ph+1);
    
    o = idivide(depth, (length(Wavelengths) * length(Phases)));
    orrientation = Orrientations(o+1);
    