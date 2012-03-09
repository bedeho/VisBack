/*
 *  Neuron.h
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/24/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#ifndef NEURON_H
#define NEURON_H

// Forward declarations
class Region;
class Synapse;

// Includes
#include <vector>
#include "Utilities.h"

using std::vector;

class Neuron {

    public:
        
        // Z,X,Y location of cell in region
        u_short depth, row, col;
        
        // Containing region
        // Cant use referenes because of insane copy constructability of vector class
        // u_short short regionNr;
        const Region * region;
        
        float firingRate;     // For postsynaptic activation computation
		float newFiringRate;  // The reason this value was introduced in Neuron class was because we may run model in discrete scheme, which depends new* values

		// Init
        void init(const Region * region, u_short depth, u_short row, u_short col);

        // Forward computation scheme
        bool hasActivation;
        void fire();
        void addEfferentLink(const Synapse *);
        void makeEfferentLinksCacheFriendly();
                
    private:
        
        vector<const Synapse *> efferentLink;
};

#endif // NEURON_H
