%
%  inspectRegionInvariance.m
%  VisBack
%
%  Created by Bedeho Mender on 29/04/11.
%  Copyright 2011 OFTNAI. All rights reserved.
%
%  INSPECT CONNECTIVITY
%  Input=========
%  filename: filename of weight file
%  Output========
%

function inspectConnectivity(filename)

    % Import global variables
    declareGlobalVars();

    % Open file
    connectivityFileID = fopen(filename);

    % Read header
    [networkDimensions, neuronOffsets2] = loadWeightFileHeader(connectivityFileID);
    
    % Setup vars
    numRegions = length(networkDimensions);
    depth = 1;

    % Setup dummy weight plots
    for r=1:numRegions,
        
        % Get region dimension
        regionDimension = networkDimensions(r).dimension;
        
        % Save axis
        axisVals(r) = subplot(numRegions, 1, r);
        
        % Only setup callback for V2+
        if r > 1,
            im = imagesc(zeros(regionDimension));
            colorbar;

            set(im, 'ButtonDownFcn', {@connectivityCallBack, r});
        end
        
        axis square;
    end
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % CALLBACKS
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    function connectivityCallBack(varargin)
        
        % Extract region,row,col
        region = varargin{3};
        pos=get(axisVals(region), 'CurrentPoint');
        
        row = imagescClick(pos(1, 2));
        col = imagescClick(pos(1, 1));
        
        disp(['You clicked X:' num2str(col) ', Y:', num2str(row)]);
        
        if region > 2
            updateWeightPlot(region, row, col);
        end
        
        % Do feature plot
        v1Dimension = networkDimensions(1).dimension

        axisVals(1) = subplot(numRegions, 1, 1);

        hold off

        syn = findV1Sources(region, depth, row, col);
        
        for k = 1:length(syn),
            drawFeature(syn(k).row, syn(k).col, syn(k).depth)
        end

        axis([0 v1Dimension+1 0 v1Dimension+1]); 
        axis square;
        
        % sources = cell  of struct  (1..n_i).(col,row,depth, productWeight)  
        function [sources] = findV1Sources(region, depth, row, col)

            THRESHOLD = 0.15;

            if region == 1, % termination condition, V1 cells return them self

                % Make 1x1 struct array
                sources(1).region = region;
                sources(1).row = row;
                sources(1).col = col;
                sources(1).depth = depth;
                %sources(1).compound = 1;

            elseif region > 1, 

                synapses = afferentSynapseList(connectivityFileID, neuronOffsets2, region, depth, row, col);

                sources = [];
                
                for s=1:length(synapses) % For each child

                    % Check that presynaptic neuron is in lower region (in
                    % case feedback network we dont want eternal loop), and
                    % that weight is over threshold
                    if synapses(s).weight > THRESHOLD && synapses(s).region < region
                        sources = [sources findV1Sources(synapses(s).region, synapses(s).depth, synapses(s).row, synapses(s).col)];
                    end
                end
            end
        end

    end

    function [res] = imagescClick(i)

        if i < 0.5
            res = 1;
        else
            res = round(i);
        end
    end

    function updateWeightPlot(region, row, col) 
        
        % Get weightbox
        weights = afferentSynapseMatrix(connectivityFileID, networkDimensions, neuronOffsets2, region, depth, col, row, region - 1, 1);

        % Save axis
        axisVals(region-1) = subplot(numRegions, 1, region-1);
        im2 = imagesc(weights);
        colorbar;
        axis square;

        % Setup callback
        set(im2, 'ButtonDownFcn', {@connectivityCallBack, region});
        
    end

end

function drawFeature(row, col, depth)

    halfSegmentLength = 0.5;
    [orrientation, wavelength, phase] = decodeDepth(depth);
    featureOrrientation = orrientation + 90; % orrientation is the param to the filter, but it corresponds to a perpendicular image feature

    dx = halfSegmentLength * cos(featureOrrientation);
    dy = halfSegmentLength * sin(featureOrrientation);

    x1 = col - dx;
    x2 = col + dx;
    y1 = row - dy;
    y2 = row + dy;
    plot([x1 x2], [y1 y2], '-');
    hold on;

end   

function [orrientation, wavelength, phase] = decodeDepth(depth)

    Phases = [0, 180];
    Orrientations = [0, 45, 90, 135];
    Wavelengths = [4];
    
    depth = uint8(depth)-1; % These formula expect C indexed depth, since I copied from project

    w = mod((idivide(depth, length(Phases))), length(Wavelengths));
    wavelength = Wavelengths(w+1);
    
    ph = mod(depth, length(Phases));
    phase = Phases(ph+1);
    
    o = idivide(depth, (length(Wavelengths) * length(Phases)));
    orrientation = Orrientations(o+1);
end 
