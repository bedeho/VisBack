#!/usr/bin/perl

	#
	#  Run.pl
	#  VisBack
	#
	#  Created by Bedeho Mender on 29/04/11.
	#  Copyright 2011 OFTNAI. All rights reserved.
	#

	use strict;
    use warnings;
    use POSIX;
	use File::Copy;
	use Data::Dumper;
	use myConfig;

	my $command;
	if($#ARGV < 0) {

		print "To few arguments passed.\n";
		print "Usage:\n";
		print "Arg. 1:";
		print " * build\n";
		print " * train\n";
		print " * test\n";
		print " * loadtest\n";
		print "Arg. 2: experiment name\n";
		print "Arg. 3: simulation name\n";
		print "Arg. 4: stimuli name\n";
		exit;
	}
	else {
        $command = $ARGV[0];
	}
	
	my $experiment;
	if($#ARGV >= 1) {
        $experiment = $ARGV[1];
	}
	else {
		die "No experiment name provided\n";
	}
	
	my $experimentFolder 		= $BASE."Experiments/".$experiment."/";
	my $parameterFile 			= $experimentFolder."Parameters.txt";
	
	# copy stuff into testing training folders
	if($command eq "build") {
        system($PROGRAM, "build", $parameterFile, $experimentFolder) == 0 or die "Could not execute simulator, or simulator returned 0.\n";

	} else {
		
		my $simulation;
		if($#ARGV >= 2) {
	        $simulation = $ARGV[2];
		} else {
	        die "No simulation name provided\n";
		}
		
		my $simulationFolder 		= $experimentFolder.$simulation."/";
		
		if ($command eq "loadtest") {

			# Add md5 test here
			my $networkFile;
			if($#ARGV >= 3) {
				$networkFile = $experimentFolder.$ARGV[3];
			} else {
				$networkFile = $experimentFolder."BlankNetwork.txt";
			}
			
			system($PROGRAM, $command, $parameterFile, $networkFile, $simulationFolder) == 0 or die "Could not execute simulator, or simulator returned 0.\n";
        } else {
		
			my $stimuli;
			if($#ARGV >= 3) {
		        $stimuli = $ARGV[3];
			} else {
		        die "No stimuli name provided\n";
			}
			
			my $stimuliFolder 			= $BASE."Stimuli/".$stimuli."/";
			my $parameterFile 			= $simulationFolder."Parameters.txt";
	
	        if($command eq "test") {
	
				if($#ARGV >= 4) {
					doTest($PROGRAM, $parameterFile, $ARGV[4], $experimentFolder, $stimuliFolder, $simulationFolder);
				} else {
				
					# Call doTest() for all files with file name *Network.txt, this will include
					# 1. trained net (TrainedNetwork)
					# 2. intermediate nets (TrainedNetwork_epoch_transform)
					# 3. untrained control nets (BlankNetwork)
					opendir(DIR, $simulationFolder) or die $!;
					
					while (my $file = readdir(DIR)) {

						# We only want files
						next unless (-f $simulationFolder.$file);
	
						# Use a regular expression to find files of the form *Network*
						next unless ($file =~ m/Network/);

						# Run simulation
						doTest($PROGRAM, $parameterFile, $file, $experimentFolder, $stimuliFolder, $simulationFolder);
					}
					
					closedir(DIR);
				}
	                
	        } elsif($command eq "train") {
	        	
				my $networkFile = "${experimentFolder}BlankNetwork.txt";
				system($PROGRAM, $command, $parameterFile, $networkFile, "${stimuliFolder}FileList.txt", "${stimuliFolder}Filtered/", $simulationFolder) == 0 or die "Could not execute simulator, or simulator returned 0.\n";
				
				# Cleanup
				my $destinationFolder = $simulationFolder."Training";
				
				if(!(-d $destinationFolder)) {
					#print "Making result $destinationFolder \n";
					mkdir($destinationFolder, 0777) || print $!;
				} else {
			    	die "Result directory already exists\n";
			    }
				
				# Move result files into result folder
				system("mv ${simulationFolder}*.dat $destinationFolder");
	        }
        }
	}
	
	# Run test on network, make result folder
	sub doTest {

		my ($PROGRAM, $parameterFile, $net, $experimentFolder, $stimuliFolder, $simulationFolder) = @_;
		
		my $networkFile = $simulationFolder.$net;
		
		system($PROGRAM, "test", $parameterFile, $networkFile, "${stimuliFolder}FileList.txt", "${stimuliFolder}Filtered/", $simulationFolder) == 0 or die "Could not execute simulator, or simulator returned 0.\n";
		
		# Make result directory
		my $newFolder = substr $net, 0, length($net) - 4;
		my $destinationFolder = $simulationFolder.$newFolder;
		
	   	if(!(-d $destinationFolder)) {
			print "Making result directory $destinationFolder \n";
			mkdir($destinationFolder, 0777) || print $!;
	    } else {
	    	die "Result directory already exists\n";
	    }
	    
	    # Move result files into result folder
	    system("mv ${simulationFolder}*.dat ${destinationFolder}");
	    
	    # Move network into result folder
		system("mv $networkFile $destinationFolder");
	}