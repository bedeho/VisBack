#!/usr/bin/perl

	#
	#  ParamSearch.pl
	#  VisBack
	#
	#  Created by Bedeho Mender on 29/04/11.
	#  Copyright 2011 OFTNAI. All rights reserved.
	#

	use strict;
    use warnings;
    use POSIX;
	use File::Copy;
	use File::Copy "cp"; # allows us to copy while preserving permissions, important when copying binary to keep it executable
	use Data::Dumper;
	use Cwd 'abs_path';
	use myConfig;
	use myLib;
	
	################################################################################################################################################################################################
    # Input
    ################################################################################################################################################################################################
	
	my $experiment 						= "test"; # , test
	my $stimuliTraining 				= "TR_2O_9T_2L"; #projectBenTraining,TR_2O_9T_2L,CT_2O_81T_16L
	my $stimuliTesting 					= "TR_2O_9T_2L"; #projectBenTesting, TR_2O_9T_2L
	my $xgrid 							= "0"; # "0" = false, "1" = true
	
	# FIXED PARAMS - non permutable
    my $wavelengths						= "{lambda = 2; fanInCount = 201;}"; #lambda=2 is Trace, lambda=16 is CT
    my $phases							= "0.0,180.0,90.0,-90.0";
    my $orientations					= "0.0,45.0,90.0,135.0";
    
	my $neuronType						= 0; # 0 = discrete, 1 = continuous
    my $learningRule					= 0; # 0 = trace, 1 = hebb
    
    my $nrOfEpochs						= 30;
    my $saveNetworkAtEpochMultiple 		= 99;
	my $outputAtTimeStepMultiple		= 101;
	
    my $lateralInteraction				= 1; # 0 = none, 1 = COMP, 2 = SOM
    my $resetTrace						= "true"; # "false", Reset trace between objects of training
    my $resetActivity					= "true"; # "false", Reset activation between objects of training
    
    # RANGE PARAMS - permutable
    
    # Notice, layer one needs 3x because of small filter magnitudes, and 5x because of
    # number of afferent synapses, total 15x.
    my @learningRates 					= (
    									# Trace
    									#["5.0000"	,"0.5000"	,"0.5000"	,"0.5000"],
    									#["1.0000"	,"1.0000"	,"1.0000"	,"1.0000"],
    									#["0.0000"	,"0.0000"	,"0.0000"	,"0.0000"]     														
    									#["1.0000"	,"0.1000"	,"0.0010"	,"0.0010"]
    									
    									#["0.0005"	,"0.001"	,"0.001"	,"0.001"],
    									#["0.0005"	,"0.001"	,"0.001"	,"0.001"]
    									
    									# CT
    									#["0.1000"	,"0.0100"	,"0.0100"	,"0.0100"]
    									#["0.1000"	,"0.0100"	,"0.0100"	,"0.0010"],
    									#["0.1000"	,"0.0100"	,"0.0010"	,"0.0001"]
    									["0.1000"	,"0.1000"	,"0.1000"	,"0.1000"]
    									
    									#["0.1000"	,"0.0670"	,"0.0500"	,"0.0400"]
    									#["0.0100"	,"0.0067"	,"0.0050"	,"0.0040"],
    									#["0.0010"	,"0.00067"	,"0.0005"	,"0.0004"]
    									);
    									
 	die "Invalid array: learningRates" if !validateArray(\@learningRates);

    my @sparsenessLevels				= ( 
    									# Trace
    									["0.992"	,"0.980"	,"0.880"	,"0.900"]
    									#["0.992"	,"0.980"	,"0.880"	,"0.850"],
    									
    									#["0.90"		,"0.90"		,"0.90"		,"0.90"]
    									#["0.960"	,"0.960"	,"0.960"	,"0.960"],
    									#["0.970"	,"0.970"	,"0.970"	,"0.970"]
    									
    									
    									
    									
    									#["0.992"	,"0.980"	,"0.880"	,"0.990"],
    									
    									#["0.992"	,"0.900"	,"0.880"	,"0.800"],
    									#["0.992"	,"0.900"	,"0.880"	,"0.850"],
    									#["0.992"	,"0.900"	,"0.880"	,"0.910"],
    									#["0.992"	,"0.900"	,"0.880"	,"0.960"],
    									#["0.992"	,"0.900"	,"0.880"	,"0.990"],
    									
    									###["0.992"	,"0.900"	,"0.800"	,"0.800"],
    									###["0.992"	,"0.900"	,"0.800"	,"0.850"],
    									###["0.992"	,"0.900"	,"0.800"	,"0.910"],
    									#["0.992"	,"0.900"	,"0.800"	,"0.960"],
    									#["0.992"	,"0.900"	,"0.800"	,"0.990"]
    									
    									#["0.992"	,"0.800"	,"0.700"	,"0.700"],
    									#["0.992"	,"0.800"	,"0.700"	,"0.800"],
    									#["0.992"	,"0.800"	,"0.700"	,"0.900"]
    									
    									###["0.992"	,"0.900"	,"0.700"	,"0.700"]
    									#["0.992"	,"0.900"	,"0.700"	,"0.800"],
    									#["0.992"	,"0.900"	,"0.700"	,"0.900"]
    									
    									# CT
    									#["0.992"	,"0.800"	,"0.700"	,"0.700"],
    									#["0.992"	,"0.800"	,"0.700"	,"0.800"],
    									#["0.992"	,"0.800"	,"0.700"	,"0.900"],
    									
    									#["0.992"	,"0.900"	,"0.700"	,"0.700"],
    									#["0.992"	,"0.900"	,"0.700"	,"0.800"],
    									#["0.992"	,"0.900"	,"0.700"	,"0.900"]
    									);
    die "Invalid array: sparsenessLevels" if !validateArray(\@sparsenessLevels);
    
    my @timeConstants					= ( 
    									#["0.050"	,"0.050"	,"0.050"	,"0.050"]
    
    									# Trace
    									#["0.010"	,"0.030"	,"0.050"	,"0.300"],
    									#["0.010"	,"0.030"	,"0.090"	,"0.300"],
    									#["0.010"	,"0.030"	,"0.100"	,"0.300"],
    									
    									#["0.010"	,"0.050"	,"0.100"	,"0.400"],
    									#["0.010"	,"0.050"	,"0.150"	,"0.400"],
    									#["0.010"	,"0.050"	,"0.250"	,"0.400"],
    									
    									#["0.010"	,"0.080"	,"0.150"	,"0.400"],
    									#["0.010"	,"0.080"	,"0.250"	,"0.400"],
    									#["0.010"	,"0.080"	,"0.350"	,"0.400"],
    									
    									#["0.010"	,"0.100"	,"0.150"	,"0.400"],
    									#["0.010"	,"0.100"	,"0.250"	,"0.400"],
    									["0.050"	,"0.100"	,"0.350"	,"0.400"]
    									   									
    									# CT
    									#["0.010"	,"0.050"	,"0.100"	,"0.200"]
    									);
    die "Invalid array: timeConstants" if !validateArray(\@timeConstants);
    								
 	my @timePrTransform					= ("0.150"); # TIME EACH TRANSFORM IS ACTIVE/USED AS INPUT
 	die "Invalid array: timePrTransform" if !validateArray(\@timePrTransform);
 	
    my @stepSizeFraction				= ("0.5"); #("3.00","2.00","1.00","0.500","0.100","0.050","0.02"); #,"0.050"); #, 0.1 = 1/10, 0.05 = 1/20, 0.02 = 1/50
    die "Invalid array: stepSizeFraction" if !validateArray(\@stepSizeFraction);
    
    my @traceTimeConstant				= ("1.500"); #("0.100", "0.050", "0.010")
	die "Invalid array: traceTimeConstant" if !validateArray(\@traceTimeConstant);
	
    my $pathWayLength					= 4;
    my $v1Dimension						= 128;
    my @dimension						= (32,32,32,32);
    my @depth							= (1,1,1,1);
    my @fanInRadius 					= (6,6,9,12);
    my @fanInCount 						= (100,100,100,100);
    my @learningrate					= ("0.1","0.1","0.1","0.1"); # < === is permuted below
    my @eta								= ("0.8","0.8","0.8","0.8");
    my @timeConstant					= ("0.1","0.1","0.1","0.1"); # < === is permuted below
    my @sparsenessLevel					= ("0.1","0.1","0.1","0.1"); # < === is permuted below
    my @sigmoidSlope 					= ("190.0","40.0","75.0","26.0");
    my @inhibitoryRadius				= ("1.38","2.7","4.0","6.0");
    my @inhibitoryContrast				= ("1.5","1.5","1.6","1.4");
   	my @somExcitatoryRadius				= ("0.7","0.55","0.4","0.6");
    my @somExcitatoryContrast			= ("5.35","33.15","117.57","120.12");
   	my @somInhibitoryRadius				= ("1.38","2.7","4.0","6.0");
    my @somInhibitoryContrast			= ("1.5","1.5","1.6","1.4");
    my @filterWidth						= (7,11,17,25);
    my @epochs							= (40,40,40,40); #(50,150,200,200);
    
    ################################################################################################################################################################################################
    # Preprocessing
    ################################################################################################################################################################################################
    
    # Do some validation
    print "Uneven parameter length." if 
    	$pathWayLength != scalar(@dimension) || 
    	$pathWayLength != scalar(@depth) || 
    	$pathWayLength != scalar(@fanInRadius) || 
    	$pathWayLength != scalar(@fanInCount) || 
    	$pathWayLength != scalar(@learningrate) || 
    	$pathWayLength != scalar(@eta) || 
    	$pathWayLength != scalar(@timeConstant) ||
    	$pathWayLength != scalar(@sparsenessLevel) ||
    	$pathWayLength != scalar(@sigmoidSlope) ||
    	$pathWayLength != scalar(@inhibitoryRadius) ||
    	$pathWayLength != scalar(@inhibitoryContrast) ||
    	$pathWayLength != scalar(@somExcitatoryRadius) ||
    	$pathWayLength != scalar(@somExcitatoryContrast) ||
    	$pathWayLength != scalar(@somInhibitoryRadius) ||
    	$pathWayLength != scalar(@somInhibitoryContrast) ||
    	$pathWayLength != scalar(@filterWidth) ||
    	$pathWayLength != scalar(@epochs);
    
    # Build template parameter file from these    	    	    	    	    
    my @esRegionSettings;
   	for(my $r = 0;$r < $pathWayLength;$r++) {

     	my %region   	= ('dimension'       	=>      $dimension[$r],
                         'depth'             	=>      $depth[$r],
                         'fanInRadius'       	=>      $fanInRadius[$r],
                         'fanInCount'        	=>      $fanInCount[$r],
                         'learningrate'      	=>      $learningrate[$r],
                         'eta'               	=>      $eta[$r],
                         'timeConstant'      	=>      $timeConstant[$r],
                         'sparsenessLevel'   	=>      $sparsenessLevel[$r],
                         'sigmoidSlope'      	=>      $sigmoidSlope[$r],
                         'inhibitoryRadius'  	=>      $inhibitoryRadius[$r],
                         'inhibitoryContrast'	=>      $inhibitoryContrast[$r],
                         'somExcitatoryRadius'  =>      $somExcitatoryRadius[$r],
                         'somExcitatoryContrast'=>      $somExcitatoryContrast[$r],
                         'somInhibitoryRadius'  =>      $somInhibitoryRadius[$r],
                         'somInhibitoryContrast'=>      $somInhibitoryContrast[$r],
                         'filterWidth'   		=>      $filterWidth[$r],
                         'epochs'   		 	=>      $epochs[$r]
                         );

         push @esRegionSettings, \%region;
    }
    
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
    my $firstTime = 1;
    
	my $experimentFolder 		= $BASE."Experiments/".$experiment."/";
	my $sourceFolder			= $BASE."Source";	
	my $stimuliFolder 			= $BASE."Stimuli/".$stimuliTraining."/";
    my $xgridResult 			= $BASE."Xgrid/".$experiment."/";
    my $untrainedNet 			= $experimentFolder."BlankNetwork.txt";

	# Check if experiment folder exists
	if(-d $experimentFolder) {
		
		print "Experiment folder already exists, do you want to remove it ? (y/n): ";
		my $input = <STDIN>;
		chomp($input); # remove trailing CR

		if($input eq "y") {
			system("rm -r $experimentFolder");
		} else {
			die("Well played.\n"); 
		}
	}
    
	# Make experiment folder
	mkdir($experimentFolder);
	
	# Make blank network #################
	
		# Make temporary parameter file
		my $tmpParameterFile = $experimentFolder."Parameters.txt";
		my $paramResult = makeParameterFile(\@esRegionSettings, "0.1", "0.1", "0.1");
		open (PARAMETER_FILE, '>'.$tmpParameterFile) or die "Could not open file '$tmpParameterFile'. $!\n";
		print PARAMETER_FILE $paramResult;
		close (PARAMETER_FILE);
		
		# Run build command
		system($PERL_RUN_SCRIPT, "build", $experiment) == 0 or exit;
		
		# Remove temporary file
		unlink($tmpParameterFile);
		
		# Copy source code as backup
		# Gives tons of error messages
		#system "cp -R $sourceFolder ${BASE}Experiments/${experiment}" or die "Make source copy: $!\n";
		
	# Make blank network #################

	# Prepare for xgird
	if($xgrid) {
		
		# Copy file list to experiment folder
		cp($stimuliFolder."FileList.txt", $experimentFolder."FileList.txt") or die "Cannot make copy of file list: $!\n";

		# Copy binary, if this is xgrid run
		cp($PROGRAM, $experimentFolder."VisBack") or die "Cannot make copy of binary: $!\n";
		
        # Make xgrid file
        open (XGRID_FILE, '>'.$experimentFolder.'xgrid.txt') or die "Could not open file '${experimentFolder}xgrid.txt'. $!\n";
        print XGRID_FILE '-in '.substr($experimentFolder, 0, -1).' -files '.$stimuliFolder.'xgridPayload.tbz ';
        
        # Make simulation file
        open (SIMULATIONS_FILE, '>'.$experimentFolder.'simulations.txt') or die "Could not open file '${experimentFolder}simulations.txt'. $!\n";
                        
        # Make result directory
        mkdir($xgridResult);
	}
	
	# Make copy of this script as summary of parameter space explored
    my $thisScript = abs_path($0);
	cp($thisScript, $experimentFolder."ParametersCopy.pl") or die "Cannot make copy of parameter file: $!\n";
	
    ################################################################################################################################################################################################
    # Permuting
    ################################################################################################################################################################################################
    
	for my $tpT (@timePrTransform) {
		for my $tC (@timeConstants) {
			for my $sSF (@stepSizeFraction) {
				for my $ttC (@traceTimeConstant) {
					for my $l (@learningRates) {
						for my $s (@sparsenessLevels) {
							
							# Layer spesific parameters
							my @learningRateArray = @{ $l };
							my @sparsityArray = @{ $s };
							my @timeConstantArray = @{ $tC };
							
							print "Uneven parameter length found while permuting." if 
    							$pathWayLength != scalar(@learningRateArray) || 
    							$pathWayLength != scalar(@sparsityArray) || 
    							$pathWayLength != scalar(@timeConstantArray);
							
							# Smallest eta value, it is used with ssF
							my $layerCounter = 0;
							my $minTc = LONG_MAX;
							
							for my $region ( @esRegionSettings ) {
								
								$region->{'learningrate'} = $learningRateArray[$layerCounter];
								$region->{'sparsenessLevel'} = $sparsityArray[$layerCounter];
								$region->{'timeConstant'} = $timeConstantArray[$layerCounter];
								
								# Find the smallest eta, it is the what sSF is calculated out of
								$minTc = $region->{'timeConstant'} if $minTc > $region->{'timeConstant'};
								
								$layerCounter++;
							}
							
							my $Lstr = "@learningRateArray";
							$Lstr =~ s/\s/-/g;
							
							my $Sstr = "@sparsityArray";
							$Sstr =~ s/\s/-/g;
							
							my $tCstr = "@timeConstantArray";
							$tCstr =~ s/\s/-/g;
							
							# Build name so that only varying parameters are included.
							my $simulationCode = "";
							$simulationCode .= "tpT=${tpT}_" if ($neuronType == 1) && scalar(@timePrTransform) > 1;
							$simulationCode .= "tC=${tCstr}_" if ($neuronType == 1) && scalar(@timeConstants) > 1;
							$simulationCode .= "sSF=${sSF}_" if ($neuronType == 1) && scalar(@stepSizeFraction) > 1;
							$simulationCode .= "ttC=${ttC}_" if ($neuronType == 1) && scalar(@traceTimeConstant) > 1;
							$simulationCode .= "L=${Lstr}_" if scalar(@learningRates) > 1;
							$simulationCode .= "S=${Sstr}_" if scalar(@sparsenessLevels) > 1;
							
							# If there is only a single parameter combination being explored, then just give a long precise name,
							# it's essentially not a parameter search.
							if($simulationCode eq "") {
								$simulationCode = "tpT=${tpT}_tC=${tCstr}_sSF=${sSF}_ttC=${ttC}_" if ($neuronType == 1);
								$simulationCode = "L=${Lstr}_S=${Sstr}_";
							}
							
							# Number of timesteps pr. transform
							my $nrOfTimeSteps = floor($tpT/($minTc * $sSF));
							
							# Test that there are actual time steps in continous case, and
							# that it is sufficient to get nonzero stimulation to the top region
							if ($neuronType == 1 && ($nrOfTimeSteps == 0 || $nrOfTimeSteps < $pathWayLength)) {
								print "Discarding simulation...\n";
								next;
							}
																		
							my $timeStepStr = "";
							#$timeStepStr = "\t\t\t\t\t| $nrOfEpochs |\t $nrOfObjects |\t $nrOfTransformations |\t $nrOfTimeSteps" if $neuronType == 1;
							$timeStepStr = "\t\t $nrOfTimeSteps" if $neuronType == 1;
							
							if($xgrid) {
								
								my $parameterFile = $experimentFolder.$simulationCode.".txt";
								
								# Make parameter file
								print "\tWriting new parameter file: ". $simulationCode . $timeStepStr . " \n";
								
								my $result = makeParameterFile(\@esRegionSettings, $sSF, $ttC, $tpT);
								
								open (PARAMETER_FILE, '>'.$parameterFile) or die "Could not open file '$parameterFile'. $!\n";
								print PARAMETER_FILE $result;
								close (PARAMETER_FILE);
								
								# Add reference to simulation name file
								print SIMULATIONS_FILE $simulationCode.".txt\n";
								
								# Add line to batch file
								print XGRID_FILE "\n" if !$firstTime;
								print XGRID_FILE "VisBack --xgrid train ${simulationCode}.txt BlankNetwork.txt";
								
								$firstTime = 0;
							} else {
								
								# New folder name for this iteration
								my $simulation = $simulationCode;
								
								my $simulationFolder = $experimentFolder.$simulation."/";
								my $parameterFile = $simulationFolder."Parameters.txt";
								
								my $blankNetworkSRC = $experimentFolder."BlankNetwork.txt";
								my $blankNetworkDEST = $simulationFolder."BlankNetwork.txt";
							
								if(!(-d $simulationFolder)) {
									
									# Make simulation folder
									#print "Making new simulation folder: " . $simulationFolder . "\n";
									mkdir($simulationFolder, 0777) || print "$!\n";
									
									# Make parameter file and write to simulation folder
									print "Writing new parameter file: ". $simulationCode  . $timeStepStr . " \n";
									my $result = makeParameterFile(\@esRegionSettings, $sSF, $ttC, $tpT);
									
									open (PARAMETER_FILE, '>'.$parameterFile) or die "Could not open file '$parameterFile'. $!\n";
									print PARAMETER_FILE $result;
									close (PARAMETER_FILE);
									
									# Run training
									system($PERL_RUN_SCRIPT, "train", $experiment, $simulation, $stimuliTraining) == 0 or exit;
									
									# Copy blank network into folder so that we can do control test automatically
									#print "Copying blank network: ". $blankNetworkSRC . " \n";
									cp($blankNetworkSRC, $blankNetworkDEST) or die "Copying blank network failed: $!\n";
									
									# Run test
									system($PERL_RUN_SCRIPT, "test", $experiment, $simulation, $stimuliTesting) == 0 or exit;
									
								} else {
									print "Could not make folder (already exists?): " . $simulationFolder . "\n";
									exit;
								}
							}
						}
					}
				}
			}
		}
	}
	
	# If we just setup xgrid parameter search
	if($xgrid) {
		
		# close xgrid batch file
		close(XGRID_FILE);
		
		# close simulation name file
		close(SIMULATIONS_FILE);
		
		# submit job to grid
		# is manual for now!
		
		# start listener
		# is manual for now! #system($PERL_XGRIDLISTENER_SCRIPT, $experiment, $counter);
	}
	else {
		# Call matlab to plot all
		system($MATLAB . " -r \"cd('$MATLAB_SCRIPT_FOLDER');plotExperimentInvariance('$experiment');\"");	
	}
	
	sub makeParameterFile {
		
		my ($a, $stepSizeFraction, $traceTimeConstant, $timePrTransform) = @_;

		@esRegionSettings = @{$a}; # <== 2h of debuging to find, I have to frkn learn PERL...
		
        my @timeData = localtime(time);
		my $stamp = join(' ', @timeData);

	    my $str = <<"TEMPLATE";
/*
*
* GENERATED IN ParamSearch.pl on $stamp
*
* VisBack parameter file
*
* Created by Bedeho Mender on 02/02/11.
* Copyright 2010 OFTNAI. All rights reserved.
*
* Note:
* This parameter file follows the libconfig hierarchical
* configuration file format, see:
* http://www.hyperrealm.com/libconfig/libconfig_manual.html#Introducion
* The values of some parameters may cause
* other parameters to not be used, but ALL must
* always be present for parsing.
* New content adhering to the libconfig standard
* is not harmful.
*/

/*
* Tells run command what type
* of activation function to use:
* 0 = rate coded, 1 = leaky integrator
*/
neuronType = $neuronType;

continuous : {
	/*
	* This fraction of timeConstant is the step size of the forward euler solver
	*/
	stepSizeFraction = $stepSizeFraction;
				
	/*
	* Time used on each transform, the number of time steps
	* pr. transform is therefor: floor(timePrTransform/(traceTimeConstant * stepSizeFraction));
	*/
	timePrTransform = $timePrTransform;
	
	/*
	* Time constant for trace term
	*/
	traceTimeConstant = $traceTimeConstant;
	
	/*
	* Whether or not to reset activity across objects in training
	*/
	resetActivity = $resetActivity;
	
	/*
	* Parameters controlling what values to output, what layers is governed by "output" parameter in each layer.
	*/
	outputNeurons = false;
	outputWeights = false;
	outputAtTimeStepMultiple = $outputAtTimeStepMultiple; /* Only used in training, may lead to no output!, in testing only last time step is outputted*/
};

training: {
	/*
	* What type of learning rule to apply.
	* 0 = trace, 1 = hebbian
	*/
	rule = $learningRule;
	
	/*
	* Whether or not to reset trace term across objects in training
	*/
	resetTrace = $resetTrace;
	
	/*
	* Saving intermediate network states
	* as independent network files
	*/
	saveNetwork = true;
	saveNetworkAtEpochMultiple = $saveNetworkAtEpochMultiple;
	
	/* 
	* Only used in continouys models:
	* An epoch is one run through the file list.
	*/
	nrOfEpochs = $nrOfEpochs; 
};

/*
* Only used in build command:
* No feedback = 0, 
* symmetric feedback = 1, 
* probabilistic feedback = 2
*/
feedback = 0;

/*
* Only used in build command:
* The initial weight set on synapses
* 0 = zero, 
* 1 = same [0,1] uniform random weight used feedbackorward&backward,
* 2 = two independent [0,1] uniform random weights used forward&backward
*/
initialWeight = 1;

/*
* What type of weight normalization will be applied after learning.
* 0 = NONE, 
* 1 = CLASSIC
*/
weightNormalization = 1;

/*
* What type of sparsification routine to apply.
* 0 = NONE, 
* 1 = HEAP
*/
sparsenessRoutine = 1;

/*
* What type of lateral interaction to use.
* 0 = NONE, 
* 1 = COMP, 
* 2 = SOM
*/
lateralInteraction = $lateralInteraction;

/*
* Only used in build command:
* Random seed used to setup initial weight strength
* and setup connectivity based on radii parameter.
*/
seed = 55;

v1: {
	dimension = $v1Dimension; /* Classic value: 128 */
	
	/*
	* The next values are for the parameter values used by the Gabor filter that produced the input to this netwok,
	* The parameter values are required to be able to deduce the input file names and to process the files properly,
	* as well as setup V1 structure.
	* Parameter explanation : http://matlabserver.cs.rug.nl/edgedetectionweb/web/edgedetection_params.html
	* Good visualization tool : http://www.cs.rug.nl/~imaging/simplecell.html
	*
	* NOTE: All filter params except .count must be in decimal form!, otherwise
	* the libconfig will throw a SettingTypeException exception.
	*/
	filter: {
		phases = ($phases);                           /* on/off bar detectors*/
		orientations = ($orientations);
		
		/* lambda is a the param, count is the number of V2 projections from each wavelength (subsampling) */
		/* Visnet values for count: 201,50,13,8 */
		wavelengths = ( $wavelengths );
	};
};

/*
* Params controlling extrastriate regions
*
* Order is relevant, goes V2,V3,...
*
* dimensions                    = the side of a square region. MUST BE EVEN and increasing with layers                                                  classic: 32,32,32,32
* fanInRadius                   = radius of each gaussian connectivity cone betweene layers, only used with build command.                              classic: 6,6,9,12
* fanInCount                    = Number of connections into a neuron in V3 and above (connections from V1 to V2 have separate param: samplecount)      classic: 272,100,100,100
* learningRate                  = Learningrates used in hebbian&trace learning.                                                                         classic: 25,6.7,5.0,4.0
* etas                          = Etas used in trace learning in non V1 layers of discrete model, and used as time constant in continous model          classic: 0.8,0.8,0.8,0.8
* sparsenessLevel               = Sparsity levels used for setSparse routine.                                                                           classic: 0.992,0.98,0.88,0.91
* sigmoidSlope                  = Sigmoid slope used in sigmoid activation function.                                                                    classic: 190,40,75,26
* inhibitoryRadius              = Radius (sigma) parameter for inhibitory filter.                                                                       classic: 1.38,2.7,4.0,6.0
* inhibitoryContrast            = Contrast (sigma) parameter for inhibitory filter.                                                                     classic: 1.5,1.5,1.6,1.4
* inhibitoryWidth               = Size of each side of square inhibitory filter. MUST BE ODD.                                                           classic: 7,11,17,25
*
* The following MUST be in decimal format: learningrate,eta,sparsenessLevel,sigmoidSlope,
*/

extrastriate: (
TEMPLATE
		
		for my $region ( @esRegionSettings ) {
			
			my %tmp = %{ $region }; # <=== perl bullshit

			$str .= "\n{\n";
			$str .= "\tdimension         		= ". $tmp{"dimension"} .";\n";
			$str .= "\tdepth             		= ". $tmp{"depth"} .";\n";
			$str .= "\tfanInRadius       		= ". $tmp{"fanInRadius"} .";\n";
			$str .= "\tfanInCount        		= ". $tmp{"fanInCount"} .";\n";
			$str .= "\tlearningrate      		= ". $tmp{"learningrate"} .";\n";
			$str .= "\teta               		= ". $tmp{"eta"} .";\n";
			$str .= "\ttimeConstant			= ". $tmp{"timeConstant"} .";\n";
			$str .= "\tsparsenessLevel   		= ". $tmp{"sparsenessLevel"} .";\n";
			$str .= "\tsigmoidSlope      		= ". $tmp{"sigmoidSlope"} .";\n";
			$str .= "\tinhibitoryRadius  		= ". $tmp{"inhibitoryRadius"} .";\n";
			$str .= "\tinhibitoryContrast		= ". $tmp{"inhibitoryContrast"} .";\n";
			$str .= "\tsomExcitatoryRadius		= ". $tmp{"somExcitatoryRadius"} .";\n";
            $str .= "\tsomExcitatoryContrast	= ". $tmp{"somExcitatoryContrast"} .";\n";
			$str .= "\tsomInhibitoryRadius		= ". $tmp{"somInhibitoryRadius"} .";\n";
            $str .= "\tsomInhibitoryContrast	= ". $tmp{"somInhibitoryContrast"} .";\n";
            $str .= "\tfilterWidth   			= ". $tmp{"filterWidth"} .";\n";
            $str .= "\tepochs					= ". $tmp{"epochs"} .";\n";
                        
			$str .= "},";
		}
        # Cut away last ',' and add on closing paranthesis and semi-colon
        chop($str);
        return $str." );";
	}