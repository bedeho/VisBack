 /*
 *  Synapse.cpp
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/24/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#include "Synapse.h"
#include <cfloat>

Synapse::Synapse(float weight, const Neuron * preSynapticNeuron, const ESNeuron * postSynapticNeuron, unsigned nrOfSavedStates) :
                                                                                                    weight(weight), 
                                                                                                    preSynapticNeuron(preSynapticNeuron),
                                                                                                    postSynapticNeuron(postSynapticNeuron),
                                                                                                    weightHistory(nrOfSavedStates, FLT_MIN)
{}

Synapse::~Synapse() {   
    weightHistory.clear();
}
