/*
 *  Synapse.h
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/24/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#ifndef SYNAPSE_H
#define SYNAPSE_H

// Forward declarations
class Neuron;
class ESNeuron;

// Includes
#include <vector>

using std::vector;

class Synapse {
    
    public:
		float weight;
		const Neuron * preSynapticNeuron;     // Presynaptic neuron syanpse to our target
		const ESNeuron * postSynapticNeuron;  // This pointer is used when we have forward firing computation routine
		
		// Preallocate the required amount of space to save the history of the network
        vector<float> weightHistory;
		
		Synapse(float weight, const Neuron * preSynapticNeuron, const ESNeuron * postSynapticNeuron, unsigned nrOfSavedStates);
		~Synapse();
};

#endif // SYNAPSE_H
