/*
 *  param.h
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/29/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#ifndef PARAM_H
#define PARAM_H

#include <vector>
#include "Utilities.h"

using std::vector;

// Read parameter file for full explanation
enum FEEDBACK				{ NOFEEDBACK = 0,		SYMMETRIC = 1 };
enum LEARNING_RULE			{ TRACE = 0,			HEBBIAN = 1 };
enum SPARSENESSROUTINE		{ NOSPARSENESS = 0,		HEAP = 1 };
enum WEIGHTNORMALIZATION	{ NONORMALIZATION = 0,	CLASSIC = 1 };
enum NEURONTYPE				{ DISCRETE = 0,			CONTINUOUS = 1 };
enum INITIALWEIGHT          { ZERO = 0,				RANDOMEQUAL = 1,	RANDOMINDEPENDENT = 2 };
enum LATERAL				{ NONE = 0,				COMP = 1,			SOM= 2};

// In the future, make esregion/striate param internal classes
// that keep projects of these vectors, and thenmake param_layer class
// that is a flat copy of Param +projection, this is passed to esregions etc
class Param  {

    public:
		u_short seed;
		//u_short nrOfTransformations;
		//u_short nrOfObjects;
		u_short nrOfEpochs;
		u_short outputAtTimeStepMultiple;
		u_short saveNetworkAtEpochMultiple;

		// Values derived from other parameters
		u_short stepsPrTransform;
		float stepSize;
		// Values derived from other parameters
	
		float timePrTransform;
		float traceTimeConstant;
		
		bool outputNeurons;
		bool outputWeights;
		bool resetTrace;
		bool resetActivity;
		bool saveNetwork; 

		u_short v1dimension;					// only V1
		vector<u_short> waveLengthFanIn;	    // only V1
		vector<float> filterPhases;		        // only V1
		vector<u_short> filterWavelengths;		// only V1
		vector<float> filterOrientations;       // only V1

		vector<u_short> depths;					// Not for V1
		vector<u_short> dimensions;				// Not for V1
		vector<u_short> fanInRadius;		    // Not for V1
		vector<u_short> fanInCount;				// Not for V1

		vector<float> learningRates;			// Not for V1
		vector<float> timeConstants;			// Not for V1
		vector<float> etas;					    // Not for V1
		vector<float> sparsenessLevels;		    // Not for V1
		vector<float> sigmoidSlopes;			// Not for V1
	
		vector<u_short> filterWidth;			// Not for V1
		vector<float> inhibitoryRadius;		    // Not for V1
		vector<float> inhibitoryContrast;		// Not for V1
		vector<float> somExcitatoryRadius;		// Not for V1
		vector<float> somExcitatoryContrast;	// Not for V1
		vector<float> somInhibitoryRadius;		// Not for V1
		vector<float> somInhibitoryContrast;	// Not for V1
		vector<u_short> epochs;					// Not for V1
	
		NEURONTYPE neuronType;
		WEIGHTNORMALIZATION weightNormalization;
		SPARSENESSROUTINE sparsenessRoutine;
		FEEDBACK feedback;
		LEARNING_RULE rule;
		INITIALWEIGHT initialWeight;
		LATERAL lateralInteraction;

    	// Load param file
    	Param(const char * filename, bool learningOn);

	private:
	
		float stepSizeFraction; // not used by rest of simulator directyl, but is in parameter file
        void validate(bool learningOn);
};

#endif // PARAM_H
