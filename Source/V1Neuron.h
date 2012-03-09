/*
 *  V1Neuron.h
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/29/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#ifndef V1NEURON_H
#define V1NEURON_H

// Forward declarations

// Includes
#include "Neuron.h"
#include "Utilities.h"

class V1Neuron : public Neuron {
    
    public: 
		float filterPhase;
		float filterOrientation;
		u_short filterWavelength;

		void init(const Region * region, u_short depth, u_short row, u_short col, float filterOrientation, u_short filterWavelength, float filterPhase);
};

#endif // V1NEURON_H
