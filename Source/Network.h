/*
 *  Network.h
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/29/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#ifndef NETWORK_H
#define NETWORK_H

//#define OMP_DISABLE

// Forward declarations
class ESNeuron;
class fstreamWrapper;
class ESRegion;
class V1;

// Includes
#include "V1.h"
#include "Param.h"
#include <vector>
#include "Utilities.h"

using std::vector;

class Network {
    
    private:
        
        // Outputing
        void outputHistory(const char * outputDirectory, bool learningOn);
        void openHistoryFile(fstreamWrapper & file, const char * outputDirectory, const char * filename, bool learningOn);
        
        // Utility functions
        void buildESPathway();
        void setupAfferentSynapsesV2();
		void setupAfferentSynapsesForV3AndAbove(u_short esPathwayIndex);
		void normalize(ESNeuron * n);

		// Network structure header for output files
		void outputNetworkHeader(fstreamWrapper & file);
	
		bool verbose;
		Param p;
	
    public:
    	vector<ESRegion> ESPathway;
    	V1 striateCortex;
    	
		// Build new network based on these parameters
		Network(const char * parameterFile, bool verbose);
	
    	// Load network from weight file
    	Network(const char * fileList, const char * parameterFile, bool verbose, const char * inputWeightFile, bool learningOn);
    	    	
    	// Destructor, frees ESPathway and rngController
    	~Network();
    	
    	// Run based on parameter file supplied in constructor, argument is file list
		// learningOn = true means that we apply learning rule in param file and run the number of epochs in the param file
		// learningOn = false means that we have no learning and only run ONE epoch
    	void run(const char * inputDirectory, const char * outputDirectory, bool learningOn, int numberOfThreads, bool xgrid);
		u_short runDiscrete(const char * outputDirectory, bool learningOn, bool xgrid);
		u_short runContinous(const char * outputDirectory, bool learningOn, bool xgrid);
	
    	// Save network weight file
        void saveNetwork(const char * outputWeightFile);
};

#endif // NETWORK_H
