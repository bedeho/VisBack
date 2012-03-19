/*
 *  Network.cpp
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/29/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#include "Network.h"
#include "ESRegion.h"
#include "ESNeuron.h"
#include "V1.h"
#include "FstreamWrapper.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string> 
#include <sstream>
#include <ctime>
#include <cmath>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include "Utilities.h"

#ifdef OMP_ENABLE
	#include <omp.h>
#endif

using std::cerr;
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::ofstream;
using std::stringstream;

// Do not use this constructor if you intend to train later, use the one 
// below, if constructors where named then this amgibuity could have been
// avoided
Network::Network(const char * parameterFile, bool verbose) :
		verbose(verbose),
		p(parameterFile, false),
		ESPathway(p.dimensions.size()) {

    // Init regions
	striateCortex.init(p, NULL);
    for(u_short i = 0;i < ESPathway.size();i++)
        ESPathway[i].init(i+1, p, false, 0); // The constructor we are in now is the build constructor, so no learning will happen

    // Make afferent synapses for V2,V3,V4,V5,...
	gsl_rng * rngController = gsl_rng_alloc(gsl_rng_taus);	// Setup GSL RNG with seed
    gsl_rng_set(rngController, p.seed);
	
	ESPathway[0].setupAfferentSynapses(striateCortex, p, rngController);
    for(u_short i = 1;i < ESPathway.size();i++)
        ESPathway[i].setupAfferentSynapses(ESPathway[i - 1], p, rngController);
	
	gsl_rng_free(rngController);

    // Make efferent links for V2,V3,V4,V5,...
    striateCortex.setupEfferentLinks();
    
    for(u_short i = 0;i < ESPathway.size();i++)
        ESPathway[i].setupEfferentLinks();
}

Network::Network(const char * fileList, const char * parameterFile, bool verbose, const char * inputWeightFile, bool learningOn) :
		verbose(verbose),
		p(parameterFile, learningOn),
		ESPathway(p.dimensions.size())  {
																								
	striateCortex.init(p, fileList);
																								
	fstreamWrapper weightFile;
	
	try {
		weightFile.open(inputWeightFile, std::ios_base::in | std::ios_base::binary);
	} catch (fstream::failure e) {
		stringstream s;
		s << "Unable to open " << inputWeightFile << " for input: " << e.what() << endl;
		cerr << s.str();
		exit(EXIT_FAILURE);
	}
	
    // Read number of regions and list of dimensions
	// These values are not actually used!!!
	// We just have to consume them from the
	// file stream. We use the parameter file settings
	// to get the actual dimensions.
	// 
	// The reason these values are here is for matlab
	// matlab analysis/completeness/generality
	
	{
		u_short regions, dimension, depth;

		// Number of regions, including v1
		weightFile >> regions; 

		// striate cortex
		weightFile >> dimension;
		weightFile >> depth;
   
		for(u_short i = 0;i < regions-1;i++) {
			weightFile >> dimension;
			weightFile >> depth;
		}
	}
    
    // Init regions																					

																								
    for(u_short i = 0;i < ESPathway.size();i++)
        ESPathway[i].init(i+1, p, learningOn, striateCortex.nrOfFiles);

	// Buffer for reading header with numberOfAfferentSynapses
	vector<vector<vector<vector<u_short> > > > numberOfAfferentSynapses(ESPathway.size());

	for(u_short k = 0;k < ESPathway.size();k++) {
		vector<vector<vector<u_short> > > region(ESPathway[k].depth);

		for(u_short d = 0;d < ESPathway[k].depth;d++) {
			vector<vector<u_short> > sheet(ESPathway[k].dimension);

			for(u_short i = 0;i < ESPathway[k].dimension;i++) {
				vector<u_short> row(ESPathway[k].dimension);

				for(u_short j = 0;j < ESPathway[k].dimension;j++)
					weightFile >> row[j];
				
				sheet[i] = row;
			}
			region[d] = sheet;
		}
		numberOfAfferentSynapses[k] = region;
	}
        
    // Setup afferent synaptic connections and weights (NOT FOR V1)
    for(u_short k = 0;k < ESPathway.size();k++)
        for(u_short d = 0;d < ESPathway[k].depth;d++)
            for(u_short i = 0;i < ESPathway[k].dimension;i++)
                for(u_short j = 0;j < ESPathway[k].dimension;j++) {

					for(u_short m = 0;m < numberOfAfferentSynapses[k][d][i][j];m++) {
    					
						u_short regionNr, depth, row, col;
						float weight;
						
						weightFile >> regionNr >> depth >> row >> col >> weight;

    					Neuron * n;

    					if(regionNr == 0)
    						n = striateCortex.getNeuron(depth,row,col);
    					else
    						n = ESPathway[regionNr-1].getNeuron(depth,row,col);
    					
                        ESPathway[k].Neurons[d][i][j].addAfferentSynapse(weight, n);
                    }
                }
    
    weightFile.close();
}

Network::~Network() {
    ESPathway.clear(); 
}

void Network::run(const char * inputDirectory, const char * outputDirectory, bool learningOn, int numberOfThreads, bool xgrid) {
	
	// Load all inputs
	striateCortex.loadInput(inputDirectory);

	#ifdef OMP_ENABLE
		omp_set_num_threads(numberOfThreads);
		double start = omp_get_wtime();

		if(numberOfThreads == 1) {

			cout << endl;
			cout << "**********************************" << endl;
			cout << "**** ONLY SINGLE THREADED !!! ****" << endl;
			cout << "**********************************" << endl;
			cout << endl;
		}
		else
			cout << "Number of threads: " << numberOfThreads << endl;
	#else
		cout << endl;
		cout << "*******************" << endl;
		cout << "**** no OpenMP ****" << endl;
		cout << "*******************" << endl;
		cout << endl;
	#endif

	u_short nrOfEpochs;
	
	if(p.neuronType == CONTINUOUS)
		nrOfEpochs = runContinous(outputDirectory, learningOn, xgrid);
	else if(p.neuronType == DISCRETE)
		nrOfEpochs = runDiscrete(outputDirectory, learningOn, xgrid);
	
	#ifdef OMP_ENABLE
	double finish = omp_get_wtime();
	double elapsed = (double)(finish - start);
	
	cout << "Total run time = " <<  (int)(elapsed)/60 << " minutes: " << (int)(elapsed)%60 << " seconds" << endl;
	cout << "Run time for one epoch = " << elapsed/nrOfEpochs << " seconds" << endl;
	#endif
}

u_short Network::runDiscrete(const char * outputDirectory, bool learningOn, bool xgrid) {
	
	u_short nrOfEpochs = 0, totalEpochCounter = 0;
	u_short nrOfFiles = striateCortex.nrOfFiles;
	u_short nrOfTransformations = striateCortex.nrOfTransformations;
	
	if(learningOn) {
		
		// Find total number of epochs, used in xgrid progress report
		for(unsigned r = 0; r < p.epochs.size();r++)
			nrOfEpochs += p.epochs[r];
		
		#pragma omp parallel private(totalEpochCounter)
		{
			// Iterate each region
			for(unsigned r = 0; r < ESPathway.size();r++) {
				
				// Train each region r epochs[r] times
				for(int e = 0; e < p.epochs[r];e++) {
					
					#pragma omp single
					{
						cout << ">> layer #" << r << " >> epoch #" << e << endl;
						
						if(xgrid)
							cout << "<xgrid>{control = statusUpdate; percentDone = " << static_cast<int>(((float)(++totalEpochCounter)*100)/nrOfEpochs) << "; }</xgrid>";
					}
					
					// For each file
					for(int s = 0; s < nrOfFiles;s++) {
						
						striateCortex.switchToInput(s);
						
						// Compute new firing rates
						for(unsigned k = 0; k <= r;k++) {
							
							ESPathway[k].computeNewFiringRate();
							#pragma omp barrier
						}
						
						// Learn in present layer
						ESPathway[r].applyLearningRule();
						
						// Need barrier due to nowait in applyLearningRule()
						#pragma omp barrier
						
						// Reset trace in present layer on last transform of present object
						if((s+1) % nrOfTransformations == 0 && p.rule == TRACE && p.resetTrace)
							ESPathway[r].resetTrace();
					}
				}
			}
		}
	} else {
		
		// Testing
		#pragma omp parallel
		{
			#pragma omp single
			{
				cout << ">> epoch #1" << endl;
			}

			// For each file
			for(int s = 0; s < nrOfFiles;s++) {

				striateCortex.switchToInput(s);
				
				// Compute new firing rates
				for(unsigned k = 0; k < ESPathway.size();k++) {
					
					ESPathway[k].computeNewFiringRate();
					#pragma omp barrier	
				}

				// Save activity
				for(unsigned k = 0;k < ESPathway.size();k++)
					ESPathway[k].doTimeStep(true);
			}
		}
		
		cout << "Saving history..." << endl;
		outputHistory(outputDirectory, learningOn);
		nrOfEpochs = 1;
	}
	
	return nrOfEpochs;
}

u_short Network::runContinous(const char * outputDirectory, bool learningOn, bool xgrid) {
	
	unsigned outputAt = learningOn ? p.outputAtTimeStepMultiple : p.stepsPrTransform;
	u_short nrOfEpochs = learningOn ? p.nrOfEpochs : 1;
	u_short nrOfFiles = striateCortex.nrOfFiles;
	u_short nrOfTransformations = striateCortex.nrOfTransformations;
	
	cout << "*** " << p.stepsPrTransform << " TIME STEPS PER TRANSFORM ***" << endl;
	
	#pragma omp parallel
	{
		for(u_short e = 0; e < nrOfEpochs;e++) {
			
			// We cannot continue without reseting old values from
			// the last time step in the last epoch.
			for(unsigned k = 0;k < ESPathway.size();k++)
				ESPathway[k].clearState(true);
			
			#pragma omp single
			{
				cout << ">> epoch #" << e << endl;
				
				if(xgrid)
					cout << "<xgrid>{control = statusUpdate; percentDone = " << static_cast<int>(((float)(e+1)*100)/nrOfEpochs) << "; }</xgrid>";
			}
			
			// For each file
			for(u_short s = 0; s < nrOfFiles;s++) {
				
				striateCortex.switchToInput(s);
				
				// For each timestep
				for(u_short t = 0; t < p.stepsPrTransform;t++) {
					
					// Compute new firing rates
					for(unsigned k = 0; k < ESPathway.size();k++)
						ESPathway[k].computeNewFiringRate();
					
					// We need barrier due to nowait in computeNewFiringRate()
					#pragma omp barrier
					
					// Do learning
					if(learningOn) {
						for(unsigned k = 0; k < ESPathway.size();k++)
							ESPathway[k].applyLearningRule();
					}
					
					// We need barrier due to nowait in applyLearningRule()
					#pragma omp barrier
					
					// Make time step for each region, and save data if we are on appropriate time step
					bool save = ((t+1) % outputAt) == 0;
					for(unsigned k = 0;k < ESPathway.size();k++)
						ESPathway[k].doTimeStep(save); 
				}
				
				// During learning, reset activity/trace on last transform of object
				if(learningOn) {
					
					if((s+1) % nrOfTransformations == 0 && p.resetActivity) {
						
						for(unsigned k = 0;k < ESPathway.size();k++)
							ESPathway[k].clearState(p.resetTrace);
					}
					
				} else { // In testing we MUST reset betweene transforms when we are testing with continous neurons
					
					for(unsigned k = 0;k < ESPathway.size();k++)
						ESPathway[k].clearState(p.resetTrace); // does not matter if trace is reset here
				}
			}
			
			// Do intermediate network saves
			if(learningOn && p.saveNetwork && (e+1) % p.saveNetworkAtEpochMultiple == 0) {
				
				#pragma omp single
				{
					cout << "Saving: TrainedNetwork_e" << e+1 << ".txt" << endl;
					
					stringstream ss;
					ss << outputDirectory << "TrainedNetwork_e" << e+1 << ".txt";
					string name = ss.str();
					saveNetwork(name.c_str());
				}
			}
		}		
	}
	
	cout << "Saving history..." << endl;
	outputHistory(outputDirectory, learningOn);
	return nrOfEpochs;
}

void Network::outputHistory(const char * outputDirectory, bool learningOn) {
	
	// Always output sparsity percentile history
	fstreamWrapper sparsityPercentileValue;
	openHistoryFile(sparsityPercentileValue, outputDirectory, "sparsityPercentileValue.dat", learningOn);
	for(u_short k = 0;k < ESPathway.size();k++)
		ESPathway[k].outputSparsityPercentileValueHistory(sparsityPercentileValue);
	
    sparsityPercentileValue.close();
	
	// output neuron history if we are testing,
	// or if there is learning and we have explicitly turned on outputing neurons
	if(!learningOn || (learningOn && p.outputNeurons)) {
		
		// Outputing streams  
		fstreamWrapper firingRate, activation, inhibitedActivation, trace;
		
		openHistoryFile(firingRate, outputDirectory, "firingRate.dat", learningOn);
		openHistoryFile(activation, outputDirectory, "activation.dat", learningOn);
		openHistoryFile(inhibitedActivation, outputDirectory, "inhibitedActivation.dat", learningOn);
		openHistoryFile(trace, outputDirectory, "trace.dat", learningOn);
		
		// Output history
		for(u_short k = 0;k < ESPathway.size();k++)
			ESPathway[k].outputNeuronHistory(firingRate, inhibitedActivation, activation, trace);
		
		firingRate.close();
		activation.close();
		inhibitedActivation.close();
		trace.close();
	}
	
	// output synaptic weight history if there is learning and we have
	// explicitly turned on outputing weights
	if(learningOn && p.outputWeights) {
		
		// Outputing streams  
		fstreamWrapper weightFile;

		openHistoryFile(weightFile, outputDirectory, "synapticWeights.dat", learningOn);
            
        // Write out dump of synaptic indegree, used for file seeking in matlab
        for(u_short k = 0;k < ESPathway.size();k++)
            ESPathway[k].outputAfferentSynapseList(weightFile);
		
		// Write out afferent synaptic weights for each region
		for(u_short k = 0;k < ESPathway.size();k++)
            ESPathway[k].outputAfferentSynapses(weightFile, false);
		
		weightFile.close();
	}	
}

void Network::openHistoryFile(fstreamWrapper & file, const char * outputDirectory, const char * filename, bool learningOn) {
    
    string s(outputDirectory);
    s.append(filename);

	// Open file
	try {
		file.open(s.c_str(), std::ios::out | std::ios_base::binary);
	} catch (fstream::failure e) {
		stringstream s;
		s << "Unable to open " << s << " for output: " << e.what() << endl;
		cerr << s.str();
		exit(EXIT_FAILURE);
	}

	// Header
    file << static_cast<u_short>(learningOn ? p.nrOfEpochs : 1); //
    file << striateCortex.nrOfObjects;
    file << striateCortex.nrOfTransformations;
	
	// The number of steps outputted
	u_short outputted;
	
	if(p.neuronType == CONTINUOUS) {
		// If there is learning, we output at every desired multiple,
		// In testing we only output the very last time step.
		
		outputted = (learningOn ? p.stepsPrTransform/p.outputAtTimeStepMultiple : 1);

	} else if(p.neuronType == DISCRETE)
		outputted = 1;
	
	file << outputted;

	outputNetworkHeader(file);
}

void Network::saveNetwork(const char * outputWeightFile) {
	
	fstreamWrapper file;

	// Open file
	try {
		file.open(outputWeightFile, std::ios::out | std::ios_base::binary);
	} catch (fstream::failure e) {
		stringstream s;
		s << "Unable to open " << outputWeightFile << " for output: " << e.what() << endl;
		cerr << s.str();
		exit(EXIT_FAILURE);
	}
    
	// Header
	outputNetworkHeader(file);

	// Synaptic indegree, used for file seeking in matlab
    for(u_short k = 0;k < ESPathway.size();k++)
        ESPathway[k].outputAfferentSynapseList(file);
    
	// Write out afferent synaptic weights for each region
    for(u_short k = 0;k < ESPathway.size();k++)
        ESPathway[k].outputAfferentSynapses(file, true);
    
    file.close();
}

void Network::outputNetworkHeader(fstreamWrapper & file) {
	
	file << static_cast<u_short>(ESPathway.size() + 1);
	file << striateCortex.dimension;
	file << striateCortex.depth;
	
    for(u_short k = 0;k < ESPathway.size();k++) {
        file << ESPathway[k].dimension;
		file << ESPathway[k].depth;
	}
}
