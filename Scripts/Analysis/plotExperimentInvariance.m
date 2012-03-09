%
%  plotRegionInvariance.m
%  VisBack
%
%  Created by Bedeho Mender on 29/04/11.
%  Copyright 2011 OFTNAI. All rights reserved.
%
%  PLOT REGION INVARIANCE FOR ALL SIMULATION FILES IN ALL SIMULATIONS
%  Input=========
%  experiment: experiment name

function plotExperimentInvariance(experiment)

    % Import global variables
    declareGlobalVars();
    
    global EXPERIMENTS_FOLDER;

    experimentFolder = [EXPERIMENTS_FOLDER experiment '/'];
    
    % Iterate simulations in this experiment folder
    listing = dir(experimentFolder); 
    
    % Save results for summary
    filename = [experimentFolder 'Summary_' num2str(length(listing)) '.html']; % Make name that is always original so we dont overwrite old summary which is from previous xGridCleanup run of partial results from this same parameter search
    fileID = fopen(filename, 'w'); % did note use datestr(now) since it has string
    
    fprintf(fileID, '<html><head>\n');
    fprintf(fileID, '<style type="text/css" title="currentStyle">\n');
    fprintf(fileID, '@import "/Network/Servers/mac0.cns.ox.ac.uk/Volumes/Data/Users/mender/Dphil/Projects/VisBack/Scripts/DataTables-1.8.2/media/css/demo_page.css";\n');
	fprintf(fileID, '@import "/Network/Servers/mac0.cns.ox.ac.uk/Volumes/Data/Users/mender/Dphil/Projects/VisBack/Scripts/DataTables-1.8.2/media/css/demo_table.css";\n');
	fprintf(fileID, '</style>\n');
	fprintf(fileID, '<script type="text/javascript" language="javascript" src="/Network/Servers/mac0.cns.ox.ac.uk/Volumes/Data/Users/mender/Dphil/Projects/VisBack/Scripts/DataTables-1.8.2/media/js/jquery.js"></script>\n');
	fprintf(fileID, '<script type="text/javascript" language="javascript" src="/Network/Servers/mac0.cns.ox.ac.uk/Volumes/Data/Users/mender/Dphil/Projects/VisBack/Scripts/DataTables-1.8.2/media/js/jquery.dataTables.js"></script>\n');
	fprintf(fileID, '<script type="text/javascript" charset="utf-8">\n');
	fprintf(fileID, '$(document).ready(function() { $("#example").dataTable();});\n');
	fprintf(fileID, '</script>\n');
    fprintf(fileID, '</head>\n');
    fprintf(fileID, '<body>\n');
    fprintf(fileID, '<h1>%s - %s</h1>\n', experiment, datestr(now));
    fprintf(fileID, '<table id="example" class="display" cellpadding="10" style="border: solid 1px">\n');
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Find an example of simulation directory to extract column names- HORRIBLY CODED
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    for d = 1:length(listing),

        simulation = listing(d).name;

        if listing(d).isdir && ~any(strcmp(simulation, {'Filtered', 'Images', '.', '..'})),
            [parameters, nrOfParams] = getParameters(listing(d).name);
            break;
        end
    end
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    fprintf(fileID, '<thead><tr>');
    fprintf(fileID, '<th>Name</th>');
    fprintf(fileID, '<th>Network</th>');
    fprintf(fileID, '<th>Result</th>');
    for p = 1:nrOfParams,
        fprintf(fileID, ['<th>' parameters{p,1} '</th>']);
    end
    fprintf(fileID, '<th>#</th>');
    fprintf(fileID, '<th>Mean</th>');
    fprintf(fileID, '<th>SCA</th>');
    fprintf(fileID, '<th>MCA</th>');
    fprintf(fileID, '<th>Action</th>');
    fprintf(fileID, '</tr></thead>');
    
    %h = waitbar(0, 'Plotting&Infoanalysis...');
    counter = 1;
    
    format('short');
    
    fprintf(fileID, '<tbody>\n');
    for d = 1:length(listing),

        % We are only looking for directories, but not the
        % 'Filtered' directory, since it has filtered output
        simulation = listing(d).name;

        if listing(d).isdir && ~any(strcmp(simulation, {'Filtered', 'Images', '.', '..'})),
            
            % Waitbar messes up -nodisplay option
            %waitbar(counter/(nnz([listing(:).isdir]) - 2), h);
            disp(['******** Doing ' num2str(counter) ' out of ' num2str((nnz([listing(:).isdir]) - 2)) '********']); 
            counter = counter + 1;
            
            summary = plotSimulationRegionInvariance(experiment, simulation);

            for s=1:length(summary),
                
                netDir = [experimentFolder  simulation '/' summary(s).directory];
                
                figCommand                  = ['matlab:open(\''' netDir '/invariance.fig\'')'];
                fig2Command                 = ['matlab:open(\''' netDir '/sparsityPercentileValue.fig\'')'];
                inspectorCommand            = ['matlab:inspectRegionInvariance(\''' netDir '\'',\''' summary(s).directory '.txt\'')'];
                firingCommand               = ['matlab:plotNetworkHistory(\''' netDir '/firingRate.dat\'')'];
                activationCommand           = ['matlab:plotNetworkHistory(\''' netDir '/activation.dat\'')'];
                inhibitedActivationCommand  = ['matlab:plotNetworkHistory(\''' netDir '/inhibitedActivation.dat\'')'];
                traceCommand                = ['matlab:plotNetworkHistory(\''' netDir '/trace.dat\'')'];
                
                % Start row
                fprintf(fileID, '<tr>'); %flip color here
                
                    % Name
                    fprintf(fileID, '<td> %s </td>', simulation);
                    
                    % Network
                    fprintf(fileID, '<td> %s </td>', summary(s).directory);
                    
                    % Result
                    fprintf(fileID, '<td><img src="%s" width="370px" height="300px"/></td>', [netDir '/invariance.png']);
                     
                    % Parameters
                    parameters = getParameters(simulation);
                    
                    for p = 1:nrOfParams,
                        fprintf(fileID, ['<td> ' parameters{p,2} ' </td>']);
                    end
                    
                    % #
                    fprintf(fileID, '<td> %d </td>', summary(s).fullInvariance);
                    
                    % Mean
                    fprintf(fileID, '<td> %d </td>', summary(s).meanInvariance);
                    
                    % SCA
                    if summary(s).nrOfSingleCell < 0.1,
                        fprintf(fileID, '<td style=''background-color:green;''> %d </td>', summary(s).nrOfSingleCell);
                    else    
                        fprintf(fileID, '<td> %d </td>', summary(s).nrOfSingleCell);
                    end
                    
                    % MCA
                    if summary(s).multiCell < 0.1,
                        fprintf(fileID, '<td style=''background-color:green;''> %d </td>', summary(s).multiCell);
                    else
                        fprintf(fileID, '<td> %d </td>', summary(s).multiCell);
                    end

                    % Action
                    fprintf(fileID, '<td>');
                    fprintf(fileID, '<input type="button" value="Figure" onclick="document.location=''%s''"/></br>', figCommand);
                    fprintf(fileID, '<input type="button" value="Inspect" onclick="document.location=''%s''"/></br>', inspectorCommand);
                    fprintf(fileID, '<input type="button" value="Firing" onclick="document.location=''%s''"/></br>', firingCommand);
                    fprintf(fileID, '<input type="button" value="Activation" onclick="document.location=''%s''"/></br>', activationCommand);
                    fprintf(fileID, '<input type="button" value="IActivation" onclick="document.location=''%s''"/></br>', inhibitedActivationCommand);
                    fprintf(fileID, '<input type="button" value="Trace" onclick="document.location=''%s''"/></br>', traceCommand);
                    fprintf(fileID, '<input type="button" value="Percentile" onclick="document.location=''%s''"/></br>', fig2Command);
                    fprintf(fileID, '</td>');
                
                fprintf(fileID, '</tr>\n');
            end
            
        end
    end
    
    %close(h);
    
    fprintf(fileID, '</table></body></html>');
    fclose(fileID);
    
    %web(filename);
    
    % Thanks to DanTheMans excellent advice, now we dont
    % have to fire up terminal insessantly
    
    
    disp('DONE...');
    system('stty echo');
    
    
    function [parameters, nrOfParams] = getParameters(experiment)
        
        % Get a sample simulation name
        columns = strsplit(experiment, '_');
        nrOfParams = length(columns) - 1;
        
        parameters = cell(nrOfParams,2);
        
        for p = 1:nrOfParams,
            pair = strsplit(char(columns(p)),'='); % columns(p) is a 1x1 cell
            parameters{p,1} = char(pair(1));
            parameters{p,2} = char(pair(2));
        end
    
    