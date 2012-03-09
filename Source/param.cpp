/*
 *  param.cpp
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/29/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#include "Param.h"
#include <iostream>
#include <stdlib.h>
#include <libconfig.h++>
#include <cmath>
#include <cfloat>

using namespace libconfig;
using std::cerr;
using std::endl;

Param::Param(const char * filename, bool learningOn) {

    Config cfg;

	try
	{
		cfg.readFile(filename);

		int tmp;

		cfg.lookupValue("neuronType", tmp);
		neuronType = static_cast<NEURONTYPE>(tmp);
		
		cfg.lookupValue("continuous.traceTimeConstant", traceTimeConstant);
		cfg.lookupValue("continuous.stepSizeFraction", stepSizeFraction);
		cfg.lookupValue("continuous.timePrTransform", timePrTransform);
		cfg.lookupValue("continuous.resetActivity", resetActivity);
		cfg.lookupValue("continuous.outputNeurons", outputNeurons);
		cfg.lookupValue("continuous.outputWeights", outputWeights);
		cfg.lookupValue("continuous.outputAtTimeStepMultiple", tmp);
		outputAtTimeStepMultiple = static_cast<u_short>(tmp);
		
		// training
		cfg.lookupValue("training.rule", tmp);
		rule = static_cast<LEARNING_RULE>(tmp);
		cfg.lookupValue("training.resetTrace", resetTrace);
		cfg.lookupValue("training.saveNetwork", saveNetwork);
		cfg.lookupValue("training.saveNetworkAtEpochMultiple", tmp);
		saveNetworkAtEpochMultiple = static_cast<u_short>(tmp);
		cfg.lookupValue("training.nrOfEpochs", tmp);
		nrOfEpochs = static_cast<u_short>(tmp);

		// stimuli
		//cfg.lookupValue("stimuli.nrOfObjects", tmp);
		//nrOfObjects = static_cast<u_short>(tmp);

		//cfg.lookupValue("stimuli.nrOfTransformations", tmp);
		//nrOfTransformations = static_cast<u_short>(tmp);

		//cfg.lookupValue("stimuli.nrOfEpochs", tmp);
		//nrOfEpochs = static_cast<u_short>(tmp);
		
		// general
		cfg.lookupValue("feedback", tmp);
		feedback = static_cast<FEEDBACK>(tmp);
		
		cfg.lookupValue("initialWeight", tmp);
		initialWeight = static_cast<INITIALWEIGHT>(tmp);
		
		cfg.lookupValue("weightNormalization", tmp);
		weightNormalization = static_cast<WEIGHTNORMALIZATION>(tmp);
		
		cfg.lookupValue("sparsenessRoutine", tmp);
		sparsenessRoutine = static_cast<SPARSENESSROUTINE>(tmp);
		
		cfg.lookupValue("lateralInteraction", tmp);
		lateralInteraction = static_cast<LATERAL>(tmp);
		
		cfg.lookupValue("seed", tmp);
		seed = static_cast<u_short>(tmp);

		// v1
		cfg.lookupValue("v1.dimension", tmp);
		v1dimension = static_cast<u_short>(tmp);

		// phases
		Setting & phases = cfg.lookup("v1.filter.phases");
		for(int i = 0;i < phases.getLength();i++)
			filterPhases.push_back(phases[i]);

		// orientations
		Setting & orientations = cfg.lookup("v1.filter.orientations");
		for(int i = 0;i < orientations.getLength();i++)
			filterOrientations.push_back(orientations[i]);

		// wavelengths
		Setting & wavelengths = cfg.lookup("v1.filter.wavelengths");
		for(int i = 0;i < wavelengths.getLength();i++) {

			wavelengths[i].lookupValue("lambda", tmp);
			filterWavelengths.push_back(static_cast<u_short>(tmp));

			wavelengths[i].lookupValue("fanInCount", tmp);
			waveLengthFanIn.push_back(static_cast<u_short>(tmp));
		}

		// extrastriate
		Setting & extrastriate = cfg.lookup("extrastriate");
		for(int i = 0;i < extrastriate.getLength();i++) {
			
			dimensions.push_back(static_cast<u_short>(static_cast<int>(extrastriate[i]["dimension"])));
			depths.push_back(static_cast<u_short>(static_cast<int>(extrastriate[i]["depth"])));
			fanInRadius.push_back(static_cast<u_short>(static_cast<int>(extrastriate[i]["fanInRadius"])));
			fanInCount.push_back(static_cast<u_short>(static_cast<int>(extrastriate[i]["fanInCount"])));
			epochs.push_back(static_cast<u_short>(static_cast<int>(extrastriate[i]["epochs"])));
			
			learningRates.push_back(extrastriate[i]["learningrate"]);
			etas.push_back(extrastriate[i]["eta"]);
			timeConstants.push_back(extrastriate[i]["timeConstant"]);
			sparsenessLevels.push_back(extrastriate[i]["sparsenessLevel"]);
			sigmoidSlopes.push_back(extrastriate[i]["sigmoidSlope"]);
			inhibitoryRadius.push_back(extrastriate[i]["inhibitoryRadius"]);
			inhibitoryContrast.push_back(extrastriate[i]["inhibitoryContrast"]);
			somExcitatoryRadius.push_back(extrastriate[i]["somExcitatoryRadius"]);
			somExcitatoryContrast.push_back(extrastriate[i]["somExcitatoryContrast"]);
			somInhibitoryRadius.push_back(extrastriate[i]["somInhibitoryRadius"]);
			somInhibitoryContrast.push_back(extrastriate[i]["somInhibitoryContrast"]);
			
			filterWidth.push_back(static_cast<u_short>(static_cast<int>(extrastriate[i]["filterWidth"])));
		}

		validate(learningOn);
	}
	catch(const FileIOException &fioex) {
		cerr << "I/O error while reading parameter file: " << filename << endl;
		exit(EXIT_FAILURE);
	}
	catch(const ParseException &pex) {
		cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
		<< " - " << pex.getError() << "." << endl;
		exit(EXIT_FAILURE);
	}
	catch(const SettingNotFoundException &nfex) {
		cerr << "Setting not found in file." << endl;
		exit(EXIT_FAILURE);
	}
	catch(const SettingTypeException & stex) {
		cerr << "Setting had incompatible type." << endl;
		exit(EXIT_FAILURE);
	}
	// add more exception support later, more cases, catch them all!
}

void Param::validate(bool learningOn) {

	if(neuronType == CONTINUOUS) {
		
		float smallestTimeConstant = FLT_MAX;
		
		// Find the smallest time constant
		for(int i = 0;i < timeConstants.size();i++) {
			smallestTimeConstant = smallestTimeConstant > timeConstants[i] ? timeConstants[i] : smallestTimeConstant;
			
			if(smallestTimeConstant <= 0) {
				cerr << "timeConstant cannot be zero." << endl;
				exit(1);
			}
		}
		
		stepSize = smallestTimeConstant * stepSizeFraction;
		
		// We interpret stepSizeFraction w.r.t the smalest time constant value
		stepsPrTransform = floor(timePrTransform/stepSize);
		
		if(stepsPrTransform < timeConstants.size()) {
			cerr << "Parameter file error: stepsPrTransform <= #Nr of regions" << endl;
			exit(EXIT_FAILURE);
		}
	}
	
	if(traceTimeConstant <= 0) {
		// Cannot be zero, because then traceFactor = -inf, 
		cerr << "traceFactor cannot be zero => traceFactor = -inf => trace = NaN => dW = NaN => firing/activation = NaN." << endl;
		exit(1);
	}
}