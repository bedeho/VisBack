/*
 *  ESNeuron.cpp
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/30/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#include "ESNeuron.h"
#include "Param.h"
#include "Synapse.h"
#include "Region.h"
#include "ESRegion.h"
#include "V1.h"
#include "V1Neuron.h"
#include <iostream>
#include "utilities.h"

using std::cout;
using std::endl;

void ESNeuron::init(const Region * region, u_short depth, u_short row, u_short col, unsigned nrOfSavedNeuronStates, unsigned nrOfSavedSynapseStates, bool outputNeurons, bool outputWeights, u_short fanInCount) {

	// Call base constructor
    Neuron::init(region, depth, row, col);
	
	// Set vars
	this->nrOfSavedNeuronStates = nrOfSavedNeuronStates;
	this->nrOfSavedSynapseStates = nrOfSavedSynapseStates;
	
	this->outputNeurons = outputNeurons;
	this->outputWeights = outputWeights;
	
    this->activationHistory.resize(nrOfSavedNeuronStates);
    this->inhibitedActivationHistory.resize(nrOfSavedNeuronStates);
    this->firingRateHistory.resize(nrOfSavedNeuronStates);
    this->traceHistory.resize(nrOfSavedNeuronStates);

	this->numberOfAfferentSynapses = 0;
	this->neuronStateCounter = 0;
    this->synapseStateCounter = 0;
	
	// Initialize all state variables to zero
	clearState(true);
}

ESNeuron::~ESNeuron() {
    afferentSynapses.clear();
    activationHistory.clear();
    inhibitedActivationHistory.clear();
    firingRateHistory.clear();
    traceHistory.clear();
}

void ESNeuron::addAfferentSynapse(float weight, const Neuron * preSynapticNeuron) {
    
    afferentSynapses.push_back(Synapse(weight, preSynapticNeuron, this, nrOfSavedSynapseStates));
    numberOfAfferentSynapses = afferentSynapses.size();
	
	// compute new normalization factor , not ideal place to put it, but what the heck
	//normalizationFactor = sqrt((float)numberOfAfferentSynapses/fanInCount);
}

bool ESNeuron::areYouConnectedTo(const Neuron * n) {
    
    for(u_short s = 0;s < numberOfAfferentSynapses;s++)
        if(afferentSynapses[s].preSynapticNeuron == n)
            return true;

    return false;
}

/* 
 NOTE for both setupAfferentSynapses() routines
 =============================================
 Continue to resample bivariate gaussian until we have made enough connections,
 and a sample misses if it
 1) is outside the size of the presynaptic layer, if this happens
 we actualy consider a connection made!!!
 2) is already connected to neuron
*/

void ESNeuron::setupAfferentSynapses(V1 & preSynapticRegion, Param & p, gsl_rng * rngController) {
    
    // Subsample each wavelength to the required degree dictated by waveLengthFanIn
	for(u_short wavelengthIndex = 0;wavelengthIndex < p.waveLengthFanIn.size();wavelengthIndex++) {
        
		u_short connectionsMade = 0;
        
        while(connectionsMade < p.waveLengthFanIn[wavelengthIndex]) {
        	
        	// Sample
            int xSource,ySource;
            samplePresynapticLocation(preSynapticRegion.dimension, p.fanInRadius[region->regionNr - 1], rngController, xSource, ySource);
			
			// Sample phase and orientation
			unsigned phaseIndex = gsl_rng_uniform_int(rngController, preSynapticRegion.filterPhases.size());
			unsigned orientationIndex = gsl_rng_uniform_int(rngController, preSynapticRegion.filterOrientations.size());
			
			V1Neuron * preSynapticNeuron = preSynapticRegion.getV1Neuron(ySource, xSource, orientationIndex, wavelengthIndex, phaseIndex);
			
			// Make sure we don't reconnect
			if(!areYouConnectedTo(preSynapticNeuron)) {
				
				float weight = 0;
				
				if(p.initialWeight != ZERO) 
					weight = static_cast<float>(gsl_rng_uniform(rngController));
				
				addAfferentSynapse(weight, preSynapticNeuron);
				
				connectionsMade++;
			}
        }
    }
}

void ESNeuron::setupAfferentSynapses(ESRegion & preSynapticRegion, Param & p, gsl_rng * rngController) {
    
	u_short connectionsMade = 0;
	
	while(connectionsMade < p.fanInCount[region->regionNr - 1]) {
		
		// Sample
		int xSource,ySource;
        samplePresynapticLocation(preSynapticRegion.dimension, p.fanInRadius[region->regionNr - 1], rngController, xSource, ySource);
		
		ESNeuron * preSynapticNeuron = preSynapticRegion.getESNeuron(0, ySource, xSource);
				
		// Make sure we dont reconnect
		if(!areYouConnectedTo(preSynapticNeuron)) {
			
			float weight = 0;
			
			if(p.initialWeight != ZERO) 
				weight = static_cast<float>(gsl_rng_uniform(rngController));
			
			addAfferentSynapse(weight, preSynapticNeuron);
			
			// Add synapse backwards if feedback is enabled
			if(p.feedback == SYMMETRIC) {
				
				// If they are independent, then sample new weight
				if(p.initialWeight == RANDOMINDEPENDENT) 
					weight = static_cast<float>(gsl_rng_uniform(rngController));
				
				preSynapticNeuron->addAfferentSynapse(weight, this);
			}
			connectionsMade++;
		}
	}  
}

void ESNeuron::samplePresynapticLocation(u_short preSynapticRegionDimension, u_short radius, gsl_rng * rngController, int & xSource, int & ySource) {
    
    /*
        padding = Distance betweene postsynaptic neurons
        offset = Offset that centers destination region over source region 
        
        Solve equations:
         2*offset + padding(top.dimension - 1) = bottom.dimension
         offset = padding/2
    */
    
	float padding = static_cast<float>(preSynapticRegionDimension - 1)/region->dimension;   
    float offset = padding/2;
    
	// Sample
	double x,y;
	gsl_ran_bivariate_gaussian(rngController, radius, radius, 0, &x, &y);
	
	double xMean = col*padding + offset;
    double yMean = row*padding + offset;

    // Transform into samples into presynaptic region indices
	xSource = static_cast<int>(x + xMean);
	ySource = static_cast<int>(y + yMean);
	
	xSource = wrap(xSource, preSynapticRegionDimension);
	ySource = wrap(ySource, preSynapticRegionDimension);
}