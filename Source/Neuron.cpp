/*
 *  Neuron.cpp
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/24/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#include "Neuron.h"

void Neuron::init(const Region * region, u_short depth, u_short row, u_short col) {

	this->region = region;
    this->depth = depth;
    this->row = row;
    this->col = col;
    this->firingRate = 0;
	this->newFiringRate = 0;
}

void Neuron::makeEfferentLinksCacheFriendly() {}
