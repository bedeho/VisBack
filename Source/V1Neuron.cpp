/*
 *  V1Neuron.h
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/24/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#include "V1Neuron.h"

void V1Neuron::init(const Region * region, u_short depth, u_short row, u_short col, float filterOrientation, u_short filterWavelength, float filterPhase) {

    Neuron::init(region, depth, row, col), 
	this->filterPhase = filterPhase;
	this->filterOrientation = filterOrientation;
	this->filterWavelength = filterWavelength;
}