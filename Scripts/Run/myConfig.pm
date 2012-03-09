	
	
	#
	#  myConfig.pm
	#  SMI
	#
	#  Created by Bedeho Mender on 21/11/11.
	#  Copyright 2011 OFTNAI. All rights reserved.
	#
	
	package myConfig;
	
	use strict;
	use warnings FATAL => 'all';
	use base qw(Exporter);

	our @EXPORT = qw($BASE $PERL_RUN_SCRIPT $PROGRAM $MATLAB_SCRIPT_FOLDER $MATLAB);

	our $BASE = "/Network/Servers/mac0.cns.ox.ac.uk/Volumes/Data/Users/mender/Dphil/Projects/VisBack/";  # must have trailing slash, "D:/Oxford/Work/Projects/"
 
    ################################################################################################################################################################################################
    # Don't touch
    ################################################################################################################################################################################################
    
    our $PERL_RUN_SCRIPT 		= $BASE."Scripts/Run/Run.pl";
	our $PROGRAM				= $BASE."Source/build/Release/VisBack";
	our $MATLAB_SCRIPT_FOLDER 	= $BASE."Scripts/Analysis/";  # must have trailing slash
	our $MATLAB 				= "/Volumes/Apps/MATLAB_R2011b.app/bin/matlab -nosplash -nodisplay"; # -nodesktop

	1;