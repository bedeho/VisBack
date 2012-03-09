%
%  plotRegionInvariance.m
%  VisBack
%
%  Created by Bedeho Mender on 29/04/11.
%  Copyright 2011 OFTNAI. All rights reserved.
%
%  PLOT REGION INVARIANCE
%  Input=========
%  filename: filename of weight file
%  standalone: whether gui should be shown (i.e standalone == true)
%  region: region to plot, V1 = 1
%  depth: region depth to plot
%  row: neuron row
%  col: neuron column
%  
%  Output========
%

% 'D:\Oxford\Work\Projects\VisBack\Simulations\1Object\1Epoch\firingRate.dat'

function [fig, figImg, fullInvariance, meanInvariance, nrOfSingleCell, multiCell] = plotRegionInvariance(filename, region, depth)

    % Import global variables
    declareGlobalVars();
    
    global INFO_ANALYSIS_FOLDER;

    % Open file
    fileID = fopen(filename);
    
    % Read header
    [networkDimensions, historyDimensions, neuronOffsets, headerSize] = loadHistoryHeader(fileID);
    
    % Fill in missing arguments    
    if nargin < 3,
        depth = 1;                                  % pick top layer
        
        if nargin < 2,
            region = length(networkDimensions);     % pick last region
        end
    end
    
    if region < 2,
        error('Region is to small');
    end
    
    numEpochs = historyDimensions.numEpochs;
    numTransforms = historyDimensions.numTransforms;
    regionDimension = networkDimensions(region).dimension; 
    MaxInfo = log2(historyDimensions.numObjects);
    numCells = regionDimension*regionDimension;
    
    % Allocate data structure
    invariance = zeros(regionDimension, regionDimension, historyDimensions.numObjects);
    bins = zeros(numTransforms + 1,1);
    
    % Setup Max vars
    fullInvariance = 0;
    meanInvariance = 0;
    
    fig = figure();
    figImg = figure();
    
    floatError = 0.1;
    
    tic
    
    [pathstr, name, ext] = fileparts(filename);
    
    disp(['***Processing' pathstr]);
    
    barPlot = zeros(historyDimensions.numObjects, numTransforms);
    
    % Iterate objects
    for o = 1:historyDimensions.numObjects,           % pick all objects,
        
        % Zero out from last object
        bins = 0*bins;
        
        % Old school: this was before I started using regionHistory and was
        % array jedi!
        for row = 1:regionDimension,

            for col = 1:regionDimension,

                % Get history array
                activity = neuronHistory(fileID, networkDimensions, historyDimensions, neuronOffsets, region, depth, row, col, numEpochs); % pick last epoch

                % Count number of non zero elements
                count = length(find(activity(historyDimensions.numOutputsPrTransform, :, o, numEpochs) > floatError));

                % Save in proper bin and in invariance surface
                invariance(row, col, o) = count;
                bins(count + 1) = bins(count + 1) + 1;
            end
        end
        
        b = bins(2:length(bins));
        figure(fig); % Set as present figure
        subplot(3, 1, 1);
        plot(b);
        hold all;
        
        barPlot(o,:) = b;
        
        %figure(figImg); % Set as present figure
        %plot(b); %./numCells Normalize
        %hold all;
        
        % Update max values
        fullInvariance = fullInvariance + b(numTransforms); % The latter is the number of neurons that are fully invariant
        meanInvariance = meanInvariance + dot((b./(sum(b))),1:numTransforms); % The latter is the mean level of invariance
    end
    
    fclose(fileID);
    
    figure(figImg); % Set as present figure
    bar(barPlot'); %./numCells Normalize
    hold all;
    axis tight;
    
    figure(fig); % Set as present figure
    axis tight;
    
    % Convert firingRate file into NetStates file
    disp('Converting to NetStates1...');
    netStatesFilename = convertToNetstates(filename);
    
    title(pathstr);
    tmpAnalysisDir = [pathstr '/TempInfoAnalysis'];
    
    % Make folder
    status = mkdir(tmpAnalysisDir);
    
    if ~status,
        error(['Could not make folder ' tmpAnalysisDir]);
    end
    
    % Copy 
    disp('Copying infoanalysis folder...');
    [status, message] = copyfile(INFO_ANALYSIS_FOLDER, tmpAnalysisDir);
    
    if ~status,
        error(['Could not copy ' INFO_ANALYSIS_FOLDER ' to ' tmpAnalysisDir]);
    end
    
    % Copy NetStates1 to info analysis folder
    [status, message] = copyfile(netStatesFilename, tmpAnalysisDir);
    
    if ~status,
        error(['Could not copy ' netStatesFilename ' to ' tmpAnalysisDir]);
    end

    % Change present working directory to infoanalysis folder, and run analysis there
    disp('Doing infoanalysis...');
    initialPwd = pwd;
    cd(tmpAnalysisDir);   % We have to be in the working directory of infoanalysis and run it from there, otherwise it will not find its file
    [status, result] = system(['./infoanalysis -f 1 -b 10 -l ' num2str(region - 2)]);
    
    if status,
        error(['Infoanalysis error: ' result]);
    end
    
    % Load single cell & plot
    disp('Doing single cell infoplot...');
    [status, result] = system(['./infoplot -s -f 1 -x ' num2str(numCells) ' -y 3 -z -0.5 -l ' num2str(region - 2) ' -n "trace" -t "Single cell Analysis"']);
    
    if status,
        error(['Infoplot error: ' result]);
    else
        load data0s;
        subplot(3, 1, 2);
        plot(data0s(:,2));
        hold all;
        line([1 numCells], [MaxInfo MaxInfo]);
        axis([1 numCells 0 (MaxInfo+0.5)]);
        title('Single cell');
    end
    
    % Load multiple cell & plot
    disp('Doing multiple cell infoplot...');
    [status, result] = system(['./infoplot -m -f 1 -x ' num2str(numCells) ' -y 3 -z -0.5 -l ' num2str(region - 2) ' -n "trace" -t "Single cell Analysis"']);
    
    if status,
        error(['Infoplot error: ' result]);
    else
        load data0m;
        subplot(3, 1, 3);
        plot(data0m(:,2));
        hold all;
        line([1 (historyDimensions.numObjects * 13)], [MaxInfo MaxInfo]); % I have no clue what x axis of multiple cell is, but seems to lie close to historyDimensions.numObjects * 10
        axis([1 (historyDimensions.numObjects * 13) 0 (MaxInfo+0.5)]);
        title('Multiple cell');
    end
    
    elapsedTime = toc;
    
    format('short')

    meanInvariance = single(meanInvariance / historyDimensions.numObjects);
    nrOfSingleCell = single(MaxInfo - max(data0s(:,2))); %max(data0s(:,2) >= MaxInfo) % Count cells 
    multiCell = single(MaxInfo - max(data0m(:,2))); %nnz(data0m(:,2) >= MaxInfo) % Count cells 
    
    % Give report
    %disp('***Summary');
    disp(['elapsedTime(s):' num2str(elapsedTime)]);
    disp(['meanInvariance:' num2str(meanInvariance)]);
    disp(['nrOfSingleCell:' num2str(nrOfSingleCell)]);
    disp(['multiCell:' num2str(multiCell)]);
    
    % Make copy of single and multiple cell files
    copyfile('data0s', pathstr);
    copyfile('data0m', pathstr);
    
    % Cleanup
    rmdir(tmpAnalysisDir, 's');
    cd(initialPwd);
    