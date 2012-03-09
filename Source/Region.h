/*
 *  Region.h
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/24/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#ifndef REGION_H
#define REGION_H

// Forward declarations
class Neuron;
class Param;

// Includes
#include <vector>
#include "Utilities.h"

using std::vector;

class Region {
    	
	public:
        u_short regionNr, dimension, depth;
		
		// Init
		void init(u_short regionNr, Param & p);
		~Region();
		
		// Virtual method redefined in ESRegion/V1
		virtual Neuron * getNeuron(u_short depth, u_short row, u_short col) = 0;
		
        // Forward computation scheme
		// only Network as friend class should really be able to see these
		u_short hasActivationCount;
		vector<Neuron *> hasActivation;
		
		void fire();
		void setupEfferentLinks();
		void makeEfferentLinksCacheFriendly();
};

#endif // REGION_H
