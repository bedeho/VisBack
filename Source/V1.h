/*
 *  V1.h
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/29/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#ifndef V1_H
#define V1_H

// Forward declarations
class Neuron;
class Param;

// Includes
#include "Region.h"
#include "V1Neuron.h"
#include <vector>
#include <string>
#include "Utilities.h"

using std::vector;
using std::string;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
class V1 : public Region {
	
	private:
		vector<vector<vector<vector<float> > > > buffer; // buffer[fileNr][depth][row][col]
		vector<string> inputNames;
		u_short mapToV1Depth(u_short orientationIndex, u_short scaleIndex, u_short phaseIndex);
    
    public:
        
        // Neurons[depth][rows][col]
        vector<vector<vector<V1Neuron> > > Neurons;
        
		vector<float> filterOrientations;
		vector<u_short> filterWavelengths;
		vector<float> filterPhases;
		vector<u_short> waveLengthFanIn;
	
		u_short nrOfObjects;
		u_short nrOfTransformations;
		u_short nrOfFiles;

		// Init
		void init(Param & p, const char * fileList);

    	~V1();
        
		// Load file names from file list
		void loadFileList(const char * fileList);
	
		// Load files into buffer
		void loadInput(const char * inputDirectory);
	
		// Load switch content from buffer
		void switchToInput(u_short fileNr);
	
    	V1Neuron * getV1Neuron(u_short row, u_short col, u_short orientationIndex, u_short wavelengthIndex, u_short phaseIndex);
    	Neuron * getNeuron(u_short depth, u_short row, u_short col);
		u_short mapToOrientation(u_short d);
		u_short mapToWavelength(u_short d);
		u_short mapToPhase(u_short d);
};

#endif // V1_H
