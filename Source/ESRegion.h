/*
 *  ESRegion.h
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/24/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#ifndef EXTRASTRIATEREGION_H
#define EXTRASTRIATEREGION_H

// Forward declarations
// class Param; forward declaration is not succicient since we need Param enums.
class fstreamWrapper;

// Includes
#include "Region.h"
#include "ESNeuron.h"
#include "Param.h"
#include <vector>
#include <queue>
#include <functional>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include "Utilities.h"

using std::vector;
using std::priority_queue;
using std::greater;
  
class ESRegion : public Region { 
    
    public:

        // Neurons[depth][rows][col]
        vector<vector<vector<ESNeuron> > > Neurons;
        
		// Init - instead of ctor
		void init(u_short regionNr, Param & p, bool learningOn, int nrOfFiles);

        // Destructor
        ~ESRegion();
    
    	// Core computing methods
    	void computeNewFiringRate();
    	void applyLearningRule();						// Update weights of afferent synapses
    	
    	// Housekeeping - calls same routine on neurons
    	void doTimeStep(bool saveState);
    	
    	// Build
    	//void setupAfferentSynapses(Region * region, Param & p);
    	template<class T> void setupAfferentSynapses(T & region, Param & p, gsl_rng * rngController);
    	
    	// Output routines	
		void outputNeuronHistory(
                    fstreamWrapper & firingRate, 
					fstreamWrapper & inhibitedActivation, 
					fstreamWrapper & activation, 
					fstreamWrapper & trace);
	
		void outputSparsityPercentileValueHistory(fstreamWrapper & sparsityPercentileValueFile);
		void outputAfferentSynapses(fstreamWrapper & weightFile, bool onlyPresentState);
		void outputAfferentSynapseList(fstreamWrapper & weightFile);
					
		void outputWeightFile(fstreamWrapper & weightFile);
	
		void resetTrace();
		void clearState(bool resetTrace);
		
		void makeAfferentSynapseListCacheFriendly();

		ESNeuron * getESNeuron(u_short depth, u_short row, u_short col);
		Neuron * getNeuron(u_short depth, u_short row, u_short col);
		
    private:

        unsigned stateSaveCounter;
		float threshold;
		vector<float> sparsityPercentileValue;
	
        // Filters
        vector<vector<float> > inhibitoryFilter;
		vector<vector<float> > somFilter;
       
        // Find better solution later, this is not that nice
		// Param + copied out of actual param for speed and not having to projecting components all the time
		u_short filterWidth;							// duplicate of p.filterWidth[regionNr-1]
		float inhibitoryRadius;						    // duplicate of p.inhibitoryRadius[regionNr-1]
		float inhibitoryContrast;						// duplicate of p.inhibitoryContrast[regionNr-1]
		float somExcitatoryRadius;						// duplicate of p.somExcitatoryRadius[regionNr-1]
		float somExcitatoryContrast;					// duplicate of p.somExcitatoryContrast[regionNr-1]
		float somInhibitoryRadius;						// duplicate of p.somInhibitoryRadius[regionNr-1]
		float somInhibitoryContrast;					// duplicate of p.somInhibitoryContrast[regionNr-1]	
		float sparsenessLevel;							// duplicate of p.sparsities[regionNr-1]
		float sigmoidSlope;							    // duplicate of p.sigmoidSlopes[regionNr-1]
		float learningRate;							    // duplicate of p.learningRates[regionNr-1]
		float eta;										// duplicate of p.etas[regionNr-1]
		float timeConstant;								// duplicate of p.timeConstants[regionNr-1]
		float stepSize;
		float traceTimeConstant;
	
		NEURONTYPE neuronType;
		SPARSENESSROUTINE sparsenessRoutine;
		LEARNING_RULE rule;
		WEIGHTNORMALIZATION weightNormalization;
		LATERAL lateralInteraction;
	
        // SetSparse
        float findThreshold();                          // finds actual percentile value based on sparisity parameter
        
        // Lateral Interaction
		u_short filterCenter;
		void setupFilters();
		void filter();
		void computeNewActivation();					// classic weighted sum of presynaptic firingrates
};

#endif // EXTRASTRIATEREGION_H
