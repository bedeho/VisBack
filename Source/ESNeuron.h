/*
 *  ESNeuron.h
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/30/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#ifndef EXTRASTRIATENEURON_H
#define EXTRASTRIATENEURON_H

// Forward declarations
class Param;
// class Synapse; Needed for afferentSynapses
class Region;
class ESRegion;
class V1;

// Includes
#include "Neuron.h"
#include "Synapse.h"
#include <vector>
#include <gsl/gsl_randist.h>
#include <cfloat>
#include "Utilities.h"

using std::vector;

class ESNeuron: public Neuron {
	
	private:
        
        // Used to preallocate the required amount of space 
        // to save the state history of neuron and afferent synapses
		unsigned neuronStateCounter;
		unsigned synapseStateCounter;
		unsigned nrOfSavedNeuronStates;
		unsigned nrOfSavedSynapseStates;
		bool outputWeights;
		bool outputNeurons;

    public:
        
        // Data structures
        vector<Synapse> afferentSynapses;
        u_short numberOfAfferentSynapses;                  // simply used to avoid calls to .size() for speed, any point?
        
        // Neuron State
        float activation;                                   // Normal weighted sum of input firing rates
        float inhibitedActivation;                          // Activation after being passed through inhibit routine
        float trace;                                        // Defines trace values for this neuron
        float newActivation;
        float newInhibitedActivation;
        float newTrace;
        
        // State history
		vector<float> activationHistory;
		vector<float> inhibitedActivationHistory;
		vector<float> firingRateHistory;
        vector<float> traceHistory;

		// Init
		void init(const Region * region, u_short depth, u_short row, u_short col, unsigned nrOfSavedNeuronStates, unsigned nrOfSavedSynapseStates, bool outputNeurons, bool outputWeights, u_short fanInCount);
        
        // Destructor
        ~ESNeuron();
        
        void doTimeStep(bool save);
		void clearState(bool resetTrace); // Does not clear history vectors, just state vars
		void saveState();
		
		// Setup network
		void setupAfferentSynapses(V1 & preSynapticRegion, Param & p, gsl_rng * rngController);
		void setupAfferentSynapses(ESRegion & preSynapticRegion, Param & p, gsl_rng * rngController);
		void samplePresynapticLocation(u_short preSynapticRegionDimension, u_short radius, gsl_rng * rngController, int & xSource, int & ySource);
        
        // Synapse utils used when setting up connections
        void addAfferentSynapse(float weight, const Neuron * preSynapticNeuron);
        bool areYouConnectedTo(const Neuron * n);
		void normalize();
		void normalize(float norm);
};

/*
*
* Are placed here because of inlining:
* Read note on: [9.6] How do you tell the compiler to make a non-member function inline?
* http://www.parashift.com/c++-faq-lite/inline-functions.html#faq-9.9
*
*/

#include <float.h>
#include <math.h>

inline void ESNeuron::clearState(bool resetTrace) {
	
	firingRate = 0;
	newFiringRate = 0;
	activation = 0;
    newActivation = 0;             
	inhibitedActivation = 0;
    newInhibitedActivation = 0;
	
	if(resetTrace) {
		trace = 0; 
		newTrace = 0;
	}
}

// Housekeeping - switches old and new variables, and saves
// neuron states if saveState == true.
inline void ESNeuron::doTimeStep(bool save) {
    
	activation = newActivation;
	newActivation = FLT_MIN;
	
	inhibitedActivation = newInhibitedActivation;
	newInhibitedActivation = FLT_MIN;
	
	firingRate = newFiringRate;
	newFiringRate = FLT_MIN;
	
	trace = newTrace;
	// n->newTrace MUST NOT BE RESET TO 0 since it is not always
	// recomputed on every time step (when p.trainAtTimeStepMultiple > 1 in discrete neurons),
    // which results that all learning is cancelled!
	
	if(save) {
		
		if(outputNeurons) {
			activationHistory[neuronStateCounter] = activation;
			inhibitedActivationHistory[neuronStateCounter] = inhibitedActivation;
			firingRateHistory[neuronStateCounter] = firingRate;
			traceHistory[neuronStateCounter] = trace;
			neuronStateCounter++;
		}
		
		if(outputWeights) {
			
			for(u_short s = 0;s < numberOfAfferentSynapses;s++)
				afferentSynapses[s].weightHistory[synapseStateCounter] = afferentSynapses[s].weight;
			
			synapseStateCounter++;
		}
	}
}

inline void ESNeuron::normalize() {
	
	float norm = 0;
	
	for(u_short s = 0;s < numberOfAfferentSynapses;s++)
		norm += afferentSynapses[s].weight * afferentSynapses[s].weight;
	
	normalize(norm);
}

// The reason we have this odd subroutine is because
// this is directly called during learning where norm
// is computed along with the weight update.
inline void ESNeuron::normalize(float norm) {
	
	norm = sqrt(norm);
	for(u_short s = 0;s < numberOfAfferentSynapses;s++)
		afferentSynapses[s].weight /= norm;
}


#endif // EXTRASTRIATENEURON_H
