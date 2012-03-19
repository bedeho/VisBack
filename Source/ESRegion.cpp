/*
 *  ESRegion.cpp
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/24/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

#include "ESRegion.h"
#include "ESNeuron.h"
#include "Synapse.h"
#include "FstreamWrapper.h"
#include "V1Neuron.h"
#include "V1.h"
#include <math.h>
#include <sstream>
#include <algorithm>
#include <queue>
#include <iostream>
#include "Utilities.h"

using std::cout;
using std::endl;

void ESRegion::init(u_short regionNr, Param & p, bool learningOn, int nrOfFiles) {
	
	// Call base constructor
	Region::init(regionNr, p);
    
	// Set vars
	this->stateSaveCounter = 0;
	this->filterWidth = p.filterWidth[regionNr-1]; 
	this->inhibitoryRadius = p.inhibitoryRadius[regionNr-1]; 
	this->inhibitoryContrast = p.inhibitoryContrast[regionNr-1];
	this->somExcitatoryRadius = p.somExcitatoryRadius[regionNr-1]; 
	this->somExcitatoryContrast = p.somExcitatoryContrast[regionNr-1];
	this->somInhibitoryRadius = p.somInhibitoryRadius[regionNr-1]; 
	this->somInhibitoryContrast = p.somInhibitoryContrast[regionNr-1];
	this->sparsenessLevel = p.sparsenessLevels[regionNr-1];
	this->sigmoidSlope = p.sigmoidSlopes[regionNr-1];												
	this->learningRate = p.learningRates[regionNr-1];
	this->eta = p.etas[regionNr-1];
	this->timeConstant = p.timeConstants[regionNr-1];
	this->stepSize = p.stepSize;
	this->traceTimeConstant = p.traceTimeConstant;
	this->neuronType = p.neuronType;
	this->sparsenessRoutine = p.sparsenessRoutine;
	this->rule = p.rule;
	this->weightNormalization = p.weightNormalization;
    this->lateralInteraction = p.lateralInteraction;
	this->Neurons.resize(dimension);
	
	// Build - this part is identical in v1 as well, but not for long, so we dont put it in region
	vector<vector<vector<ESNeuron> > > tmp1(depth,vector<vector<ESNeuron> >(dimension,vector<ESNeuron>(dimension)));
	Neurons = tmp1;

	// State tracking depends on whether we have learning on not
	unsigned nrOfSavedNeuronStates, nrOfSavedSynapseStates;
	
	bool outputNeurons = p.outputNeurons;
	bool outputWeights = p.outputWeights; 
	
	if(p.neuronType == CONTINUOUS) {
	
		unsigned outputAt = (learningOn ? p.outputAtTimeStepMultiple : p.stepsPrTransform);
		unsigned stepsPrEpoch = p.stepsPrTransform * nrOfFiles;
		
		if(learningOn) {
			nrOfSavedNeuronStates = p.outputNeurons ? (stepsPrEpoch * p.nrOfEpochs)/outputAt : 0;
			nrOfSavedSynapseStates = p.outputWeights ? (stepsPrEpoch * p.nrOfEpochs)/outputAt : 0;
			
		} else {
			nrOfSavedNeuronStates = stepsPrEpoch/outputAt;
			nrOfSavedSynapseStates = 0;
			
			outputNeurons = true; // when testing we disregard this value
			outputWeights = false;
		}
		
		// Allocate space for sparsityPercentileValue
		sparsityPercentileValue = vector<float>((stepsPrEpoch * p.nrOfEpochs)/outputAt, 0);
		
	} else if(p.neuronType == DISCRETE) { // no saving of data during training
		
		nrOfSavedNeuronStates = nrOfFiles;
		nrOfSavedSynapseStates = 0;
		
		outputNeurons = true;
		outputWeights = false;
		
		// Allocate space for sparsityPercentileValue, only outputted in testing
		sparsityPercentileValue = vector<float>(nrOfFiles, 0);
	}
	
	for(int d = 0;d < depth;d++)
        for(int i = 0;i < dimension;i++)
            for(int j = 0;j < dimension;j++)
                Neurons[d][i][j].init(this, d, i, j, nrOfSavedNeuronStates, nrOfSavedSynapseStates, outputNeurons, outputWeights, p.fanInCount[regionNr-1]);
	
	// This is how matlab determines filter center with in conv2
	// http://www.mathworks.com/help/techdoc/ref/conv2.html
	// -1 from what matlab does, to make it index, and divisor in c++ takes care of floor()
	filterCenter = (filterWidth - 1)/2;
    
	// Setup spatial filter and the partial sum table for the filter
	vector<vector<float> > tmp2(dimension, vector<float>(dimension, 0));
	inhibitoryFilter = tmp2;
	somFilter = tmp2;
	
	setupFilters();
}

ESRegion::~ESRegion() {
    Neurons.clear();
	inhibitoryFilter.clear();
	somFilter.clear();
}

void ESRegion::setupFilters() {
	
	float nonCenterCumulativeSum = 0;
	
    for(int i = 0;i < filterWidth;i++)
        for(int j = 0;j < filterWidth;j++) {
            
            u_short a = abs(filterCenter - i);
            u_short b = abs(filterCenter - j);
			
			// Inhibitory filter
            if(a != 0 || b != 0) { // a == 0 && b == 0 is handled below
                inhibitoryFilter[i][j] = -1 * inhibitoryContrast * exp (-1 * (float)(a*a + b*b) / (inhibitoryRadius*inhibitoryRadius));
                nonCenterCumulativeSum += inhibitoryFilter[i][j];
            }
			
			// SOM
			somFilter[i][j] = -1 * somInhibitoryContrast * exp (-1 * (float)(a*a + b*b) / (somInhibitoryRadius * somInhibitoryRadius)) + somExcitatoryContrast * exp (-1 * (float)(a*a + b*b) / (somExcitatoryRadius * somExcitatoryRadius));
        }
	
    // Set the center of the filter with the special case formula
    inhibitoryFilter[filterCenter][filterCenter] = 1-nonCenterCumulativeSum;
}

void ESRegion::computeNewFiringRate() {
	
	// Compute activation
	computeNewActivation();
	
	// Do local inhibition
	// Even if we do not run .inhibit(), the activation
	// values will still have been copied through to 
	// n->newInhibitedActivation by .computeNewActivation(),
	// hence all future calculations that expect inhibited values
	// will still work.
	
	if(lateralInteraction != NONE)
		filter();
	
	// this value is written to once by each thread,
	// but it is the same value is computed in all threads,
	// so it does not matter
	if(sparsenessRoutine != NOSPARSENESS)
		threshold = findThreshold();
	
	// Compute firing rate using contrast enhancement
	for(int d = 0;d < depth;d++)
	{
		#pragma omp for nowait
		for(int i = 0;i < dimension; i++)
			for(int j = 0;j < dimension; j++)
				Neurons[d][i][j].newFiringRate =  1/(1+exp(-2*sigmoidSlope*(Neurons[d][i][j].newInhibitedActivation-threshold))); //Neurons[d][i][j].newInhibitedActivation > threshold ? 1 : 0;
	}
}

// Save outout in newActivation (also newInhibitedActivation)
void ESRegion::computeNewActivation() {
	
	for(int d = 0;d < depth;d++)
	{
		#pragma omp for
    	for(int i = 0;i < dimension; i++)
    		for(int j = 0;j < dimension; j++) {
                
                ESNeuron * n = &Neurons[d][i][j];
                int a = n->numberOfAfferentSynapses;
				float weightedSum = 0;
				
				if(neuronType == CONTINUOUS) {
					
					for(int s = 0;s < a;s++)
						weightedSum += n->afferentSynapses[s].weight * n->afferentSynapses[s].preSynapticNeuron->firingRate;
					
					n->newActivation = (1 - stepSize/timeConstant) * n->activation + (stepSize/timeConstant) * weightedSum;
					
				} else {
					
					for(int s = 0;s < a;s++)
						weightedSum += n->afferentSynapses[s].weight * n->afferentSynapses[s].preSynapticNeuron->newFiringRate;
					
					n->newActivation = weightedSum;
				}
				
    			// Is copied forward in case do not have inhibition routine
    			// turned on in parameter file
    			n->newInhibitedActivation = n->newActivation;
    		}	
	}
}

// CLASSIC
void ESRegion::filter() {

	int n_i, n_j;		 // neuron being inspected by filter
	float convolutionResult;
	
	// Choose neuron to center filter on
	#pragma omp for
	for(int i = 0;i < dimension;i++)
        for(int j = 0;j < dimension;j++) {
			
            convolutionResult = 0;
			
            // Iterate over neighberhood of (i,j)
            for(int f_i = 0; f_i < filterWidth;f_i++)
                for(int f_j = 0; f_j < filterWidth;f_j++) {

					n_i = i + f_i - filterCenter;
					n_j = j + f_j - filterCenter;
					
					// Wrap around
					n_i = wrap(n_i, dimension);
					n_j = wrap(n_j, dimension);
					
					convolutionResult += Neurons[0][n_i][n_j].newActivation * (lateralInteraction == COMP ? inhibitoryFilter[f_i][f_j] : somFilter[f_i][f_j]);
                }
            
            // Save result convolutionResult
            Neurons[0][i][j].newInhibitedActivation = convolutionResult;
        }	
}

float ESRegion::findThreshold() {

    u_short percentileSize = static_cast<u_short>(depth*dimension*dimension*(1-sparsenessLevel));
	u_short added = 0;
	
	// Clear existing content from last run, there is no clear method, so we have to reset
	// Slows everything down that we cannot preallocate appropriate space and just keep buffer arround across calls,
	// hopefully still faster then qsort().
	priority_queue<float,vector<float>,greater<float> > minimumHeap = priority_queue<float,vector<float> ,greater<float> >();
	
	// Iterate layer, and only replace top of heap if bigger then .top is found
	for(int d = 0;d < depth;d++)
		for(int i = 0;i < dimension;i++)
			for(int j = 0;j < dimension;j++) {
				
				float x = Neurons[d][i][j].newInhibitedActivation;
				
				if(added < percentileSize) {
					minimumHeap.push(x);
					added++;
				} else if(minimumHeap.top() < x) {
					minimumHeap.pop();
					minimumHeap.push(x);
				}
			}
	
	// Find percentile
	return minimumHeap.top();
}

void ESRegion::applyLearningRule() {
	
    for(int d = 0; d < depth; d++)
		#pragma omp for nowait
        for(int i = 0; i < dimension; i++)
            for(int j = 0; j < dimension; j++) {
				
                ESNeuron * n = &Neurons[d][i][j];
                int a = n->numberOfAfferentSynapses;
                float norm = 0;
				
				if(neuronType == CONTINUOUS) {
					
					for(int s = 0;s < a;s++) {
						n->afferentSynapses[s].weight += learningRate * stepSize * (rule == HEBBIAN ? n->firingRate : n->trace) * n->afferentSynapses[s].preSynapticNeuron->firingRate;
						norm += n->afferentSynapses[s].weight * n->afferentSynapses[s].weight;
					}
					
					n->newTrace = (1 - stepSize/traceTimeConstant)*n->trace + (stepSize/traceTimeConstant)*n->firingRate;
					
				} else {
					
					for(int s = 0;s < a;s++) {
						n->afferentSynapses[s].weight += learningRate * (rule == HEBBIAN ? n->newFiringRate : n->newTrace) * n->afferentSynapses[s].preSynapticNeuron->newFiringRate;
						norm += n->afferentSynapses[s].weight * n->afferentSynapses[s].weight;
					}
					
					// Update trace term, whether it is being used or not
					// in theory we do not need newTrace, we can just use trace and
					// copy back in since trace term is only read/written two for one neuron
					n->newTrace = eta*n->newTrace + (1-eta)*n->newFiringRate;
				}

				// Normalization
				if(weightNormalization == CLASSIC)
					n->normalize(norm);
            }
}

void ESRegion::doTimeStep(bool save) {
	
	for(int d = 0;d < depth;d++)
		#pragma omp for
		for(int i = 0;i < dimension; i++)
    		for(int j = 0;j < dimension; j++)
               Neurons[d][i][j].doTimeStep(save);
	
	#pragma omp single	
	{	
		if(save) {
			sparsityPercentileValue[stateSaveCounter] = threshold;
			stateSaveCounter++;
		}
	}
}

void ESRegion::resetTrace() {
	
	for(int d = 0;d < depth;d++)
		#pragma omp for
    	for(int i = 0;i < dimension; i++)
    		for(int j = 0;j < dimension; j++) {
    			
    			// Reseting .trace is not enough, because .newTrace
    			// will be copid back into .trace in .doTimeStep() 
    			// if there is no learning in next time step.
    			// Also, if we switch to symmetric trace rule,
    			// then newTrace is used directly
                Neurons[d][i][j].trace = 0;
    			Neurons[d][i][j].newTrace = 0; 
    		}
}

void ESRegion::clearState(bool resetTrace) {
	
	for(int d = 0;d < depth;d++)
		#pragma omp for
		for(int i = 0;i < dimension; i++)
    		for(int j = 0;j < dimension; j++)
                Neurons[d][i][j].clearState(resetTrace); 
}

template<class T>
void ESRegion::setupAfferentSynapses(T & region, Param & p, gsl_rng * rngController) {
    
    for(int d = 0;d < depth;d++)
        for(int i = 0;i < dimension;i++)
            for(int j = 0;j < dimension;j++) {
				
    			ESNeuron & postSynapticNeuron = Neurons[d][i][j];
                    
                postSynapticNeuron.setupAfferentSynapses(region, p, rngController);
    			
    			if(p.weightNormalization == CLASSIC)
                    postSynapticNeuron.normalize();
            }  
}

// Template instansiation
template void ESRegion::setupAfferentSynapses(ESRegion & region, Param & p, gsl_rng * rngController);
template void ESRegion::setupAfferentSynapses(V1 & region, Param & p, gsl_rng * rngController);

void ESRegion::outputNeuronHistory(fstreamWrapper & firingRateFile, 
                                    fstreamWrapper & inhibitedActivationFile, 
                                    fstreamWrapper & activationFile, 
                                    fstreamWrapper & trace) {
	
    for(int d = 0;d < depth;d++)                   
    	for(int i = 0;i < dimension;i++)
    		for(int j = 0;j < dimension;j++)
    			for(unsigned t = 0;t < stateSaveCounter;t++) {
					
        			firingRateFile << Neurons[d][i][j].firingRateHistory[t];
        			activationFile << Neurons[d][i][j].activationHistory[t];
					inhibitedActivationFile << Neurons[d][i][j].inhibitedActivationHistory[t];
        			trace << Neurons[d][i][j].traceHistory[t];
                }
}

void ESRegion::outputSparsityPercentileValueHistory(fstreamWrapper & sparsityPercentileValueFile) {
	
	for(unsigned t = 0;t < stateSaveCounter;t++)
		sparsityPercentileValueFile << sparsityPercentileValue[t];
}

void ESRegion::outputAfferentSynapses(fstreamWrapper & weightFile, bool onlyPresentState) {
    
    for(int d = 0;d < depth;d++)
    	for(int i = 0;i < dimension;i++)
    		for(int j = 0;j < dimension;j++) {
    			
                const ESNeuron * n = &Neurons[d][i][j];
            	
            	for(int s = 0;s < n->numberOfAfferentSynapses;s++) {
            		
    				const Neuron * preSynapticNeuron = n->afferentSynapses[s].preSynapticNeuron;
    				
            		weightFile << preSynapticNeuron->region->regionNr 
							<< preSynapticNeuron->depth 
							<< preSynapticNeuron->row 
							<< preSynapticNeuron->col;
					
    				if(onlyPresentState)
    				    weightFile << n->afferentSynapses[s].weight;
    				else
            			for(unsigned t = 0;t < stateSaveCounter;t++)
                			weightFile << n->afferentSynapses[s].weightHistory[t];
            	}
    		}
}

void ESRegion::outputAfferentSynapseList(fstreamWrapper & weightFile) {
    
    for(int d = 0;d < depth;d++)
    	for(int i = 0;i < dimension;i++)
    		for(int j = 0;j < dimension;j++)
            	weightFile << Neurons[d][i][j].numberOfAfferentSynapses;
}

ESNeuron * ESRegion::getESNeuron(u_short depth, u_short row, u_short col) {
    return &Neurons[depth][row][col];
}

Neuron * ESRegion::getNeuron(u_short depth, u_short row, u_short col) {
    return &Neurons[depth][row][col];
}
