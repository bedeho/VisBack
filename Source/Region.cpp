/*
 *  Region.cpp
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/24/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */
#include "Region.h"
#include "Param.h"

void Region::init(u_short regionNr, Param & p) {
	
	this->regionNr = regionNr; 
    this->dimension = regionNr == 0 ? p.v1dimension : p.dimensions[regionNr-1];
    this->depth = regionNr == 0 ? p.filterWavelengths.size()*p.filterPhases.size()*p.filterOrientations.size() : p.depths[regionNr-1];
    this->hasActivationCount = 0;
    this->hasActivation.resize(dimension*dimension*depth);
}

Region::~Region() {
    hasActivation.clear();
}

void Region::fire() {}

void Region::setupEfferentLinks() {
    
    makeEfferentLinksCacheFriendly();
}

void Region::makeEfferentLinksCacheFriendly() {}
