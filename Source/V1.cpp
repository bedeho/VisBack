/*
 *  V1.cpp
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/29/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#include "V1.h"
#include "V1Neuron.h" 
#include "Neuron.h"
#include "Param.h"
#include "FstreamWrapper.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include "utilities.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::fstream;
using std::ostringstream;
using std::stringstream;
using std::ifstream;

void V1::init(Param & p, const char * fileList) {

	Region::init(0,p);

	// Set vars
	this->filterWavelengths = p.filterWavelengths;
	this->filterPhases = p.filterPhases;
	this->filterOrientations = p.filterOrientations;
    this->waveLengthFanIn = p.waveLengthFanIn;
	
    // Allocate neuron space
	vector<vector<vector<V1Neuron> > > tmp1(depth, vector<vector<V1Neuron> >(dimension, vector<V1Neuron>(dimension)));
	Neurons = tmp1;
	
	// Initialize neurons
	for(u_short d = 0;d < depth;d++) {

		float filterOrientation =  p.filterOrientations[mapToOrientation(d)];
		u_short filterWavelength = p.filterWavelengths[mapToWavelength(d)];
		float filterPhase = p.filterPhases[mapToPhase(d)];

        for(u_short i = 0;i < dimension;i++)
            for(u_short j = 0;j < dimension;j++)
                Neurons[d][i][j].init(this, d, i, j, filterOrientation, filterWavelength, filterPhase);
	}
	
	nrOfObjects = 0;
	nrOfTransformations = 0;
	nrOfFiles = 0;
	
	// Load file list if provided
	if(fileList != NULL) {
		
		loadFileList(fileList);
		
		// Allocate space for buffer
		vector<vector<vector<vector<float> > > > tmp2(nrOfObjects * nrOfTransformations, vector<vector<vector<float> > >(depth, vector<vector<float> >(dimension, vector<float>(dimension)))); 
		buffer = tmp2;
	}
}

V1::~V1() {
    Neurons.clear();
	buffer.clear();
}

void V1::loadFileList(const char * fileList) {
    
	// Open file list
	//string f(inputDirectory);
	//f.append("FileList.txt");
	ifstream fileListStream;
	
	// By using this mask we get exception if there is \n as
	// last charachter, I do not understand why, but I've wasted
	// enough time looking into it.
	//fileListStream.exceptions ( ifstream::failbit | ifstream::badbit );

	fileListStream.open(fileList);

	if(fileListStream.fail()) {
		stringstream s;
		s << "Unable to open " << fileList << " for input." << endl;
		cerr << s.str();
		exit(EXIT_FAILURE);
	}
	
	string dirNameBase;						// The "shapeS1T2" part of "shapeS1T2.png"
	u_short filesLoaded = 0;
	u_short lastNrOfTransformsFound = 0; // For validation of file list
	
	cout << "Reading file list:" << endl;
	
	while(getline(fileListStream, dirNameBase)) { 	// Read line from file list
		
		if(dirNameBase.compare("") == 0)
			continue; // Last line may just be empty bcs of matlab script, should be break; really, but what the hell		
		else if(dirNameBase.compare("*") == 0) {	
			
			if(lastNrOfTransformsFound != 0 && lastNrOfTransformsFound != nrOfTransformations) {
				cerr << "Number of transforms varied in file list" << endl;
				exit(EXIT_FAILURE);
			}
				
			nrOfObjects++;
			lastNrOfTransformsFound = nrOfTransformations;
			nrOfTransformations = 0;
			
			continue;
		} else {
			filesLoaded++;
			nrOfTransformations++;
		}
		
		//cout << "#" << filesLoaded << " Loading: " << dirNameBase << endl;
		
		inputNames.push_back(dirNameBase);
	}
	
	nrOfTransformations = lastNrOfTransformsFound;
	
	cout << "Objects: " << nrOfObjects << ", Transforms: " << nrOfTransformations << endl;
	
	nrOfFiles = nrOfObjects * nrOfTransformations;
}

void V1::loadInput(const char * inputDirectory) {
	
	for(int f = 0;f < inputNames.size();f++) {
		
		cout << "Loading Stimuli #" << f << endl;
		
		for(u_short orientation = 0;orientation < filterOrientations.size();orientation++)	// Orientations
			for(u_short wavelength = 0;wavelength < filterWavelengths.size();wavelength++)	// Wavelengths
				for(u_short phase = 0;phase < filterPhases.size();phase++) {				// Phases
					
					// Read input to network
					ostringstream dirStream;
					dirStream << inputDirectory << inputNames[f] << ".flt" << "/" 
					<< inputNames[f] << '.' << filterWavelengths[wavelength] << '.' 
					<< filterOrientations[orientation] << '.' << filterPhases[phase] << ".gbo";
					
					string t = dirStream.str();
					
					// Open&Read gabor filter file
					fstreamWrapper gaborStream;
					
					try {
						float firing;
						gaborStream.open(t.c_str(), std::ios_base::in | std::ios_base::binary);
						
						// Read flat buffer into 2d slice of V1
						u_short d = mapToV1Depth(orientation,wavelength,phase);
						for(u_short i = 0;i < dimension;i++)
							for(u_short j = 0;j < dimension;j++) {
								
								gaborStream >> firing;
								
								if(firing < 0) {
									cerr << "Negative firing loaded from filter!!!" << endl;
									exit(EXIT_FAILURE);
								}
								
								buffer[f][d][i][j] = firing;
							}
						
					} catch (fstream::failure e) {
						stringstream s;
						s << "Unable to open/read from " << t << " for gabor input: " << e.what();
						cerr << s.str();
						exit(EXIT_FAILURE);
					}
				}
	}
}
	
	
void V1::switchToInput(u_short fileNr) {

	#pragma omp for
	for(int d = 0;d < depth;d++)
		for(int i = 0;i < dimension;i++)
			for(int j = 0;j < dimension;j++) {
				Neurons[d][i][j].firingRate = buffer[fileNr][d][i][j];
				Neurons[d][i][j].newFiringRate = buffer[fileNr][d][i][j]; // used in discrete scheme only
			}
}

// inline later
u_short V1::mapToV1Depth(u_short orientationIndex, u_short wavelengthIndex, u_short phaseIndex) {
	
	return orientationIndex * (filterWavelengths.size() * filterPhases.size()) + wavelengthIndex * filterPhases.size() + phaseIndex;
}

u_short V1::mapToOrientation(u_short d) {
	
	return d / (filterWavelengths.size() * filterPhases.size());
}

u_short V1::mapToWavelength(u_short d) {
	
	return (d / filterPhases.size()) % filterWavelengths.size();
}

u_short V1::mapToPhase(u_short d) {
	return d % filterPhases.size();
}

V1Neuron * V1::getV1Neuron(u_short row, u_short col, u_short orientationIndex, u_short wavelengthIndex, u_short phaseIndex) {
    
	int depth = mapToV1Depth(orientationIndex, wavelengthIndex, phaseIndex);
    return &Neurons[depth][row][col];
}

Neuron * V1::getNeuron(u_short depth, u_short row, u_short col) {
    return &Neurons[depth][row][col];
}
