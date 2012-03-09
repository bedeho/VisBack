/*
 *  Utilities.cpp
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/29/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

/*
 inline u_short ESNeuron::wrap(int x, u_short d) {
 
 // One cannot trust result of (x % b) with negative
 // input x for various compilers:
 // http://www.learncpp.com/cpp-tutorial/32-arithmetic-operators/
 // Hence we take abs(x) first
 
 if(x > 0)
 return x % d;
 else if((-x) % d == 0)
 return 0;
 else
 return d - ((-x) % d);
 }
 */

/*
 void makeAfferentSynapseListCacheFriendly();
 void dumpSynapses(int start, int count);
 //bool sortOnDepth(Synapse & s1, Synapse & s2);
 //bool sortOnRow(Synapse & s1, Synapse & s2);
 //bool sortOnCol(Synapse & s1, Synapse & s2);
 void bubble(int start, int count, bool (*cmp)(Synapse & s1, Synapse & s2));
 */

/*
// ========================================================================
// ========================================================================

// Reshufle the presynaptic pointers to make this cache
// friendly!, this may break if synapses are extended
// with some other data in the setupafferent synaps
// routine... really need unit testing to catch that!!!
// Could have overloaded = operator on synapses and just used
// sort, but that is just inviting trouble in terms of bugs when
// I dont have unit testing... eek!

void ESNeuron::dumpSynapses(int start, int count) {
	
	for(int i = 0; i < count;i++)
		cout << afferentSynapses[i+start].preSynapticNeuron->depth << "::" << afferentSynapses[i+start].preSynapticNeuron->row << "::" << afferentSynapses[i+start].preSynapticNeuron->col << endl;
}

bool sortOnDepth(Synapse & s1, Synapse & s2) {
	return s1.preSynapticNeuron->depth > s2.preSynapticNeuron->depth;
}

bool sortOnRow(Synapse & s1, Synapse & s2) {
	return s1.preSynapticNeuron->row > s2.preSynapticNeuron->row;
}

bool sortOnCol(Synapse & s1, Synapse & s2) {
	return s1.preSynapticNeuron->col > s2.preSynapticNeuron->col;
}

// Making this whole thing a n-dimensional problem
// would have made a nice IOI problem :)
void ESNeuron::makeAfferentSynapseListCacheFriendly() {
	
	int count = afferentSynapses.size();
	
	// Sort on depth and process each segment
	bubble(0, count, &sortOnDepth);
	
	for(int dStart = 0, dEnd = 0;dEnd < count;dEnd++) {
		
		// Process present depth segment [dStart, dEnd] if
		// 1) last element of array, or
		// 2) next element is new depth
		if(dEnd == count - 1 || afferentSynapses[dEnd].preSynapticNeuron->depth != afferentSynapses[dEnd + 1].preSynapticNeuron->depth) {
			
			// Sort on row and process each segment
			int segmentLength = dEnd - dStart + 1;
			
			bubble(dStart, segmentLength, &sortOnRow);
			
			for(int rStart = dStart, rEnd = dStart;rEnd < dStart+segmentLength;rEnd++) {
				
				// Process present depth segment [rStart, rEnd] if
				// 1) last element of array, or
				// 2) next element is new row
				if(rEnd == count - 1 || afferentSynapses[rEnd].preSynapticNeuron->row != afferentSynapses[rEnd + 1].preSynapticNeuron->row) {
					
					// Sort on col
					bubble(rStart, rEnd - rStart + 1, &sortOnCol);
					
					rStart = rEnd + 1;
				}
			}
			
			dStart = dEnd + 1;
		}
	}
	
	
	 //dumpSynapses(0,count);
	 
	 //int a = 0;
	 //a++;
}

void ESNeuron::bubble(int start, int count, bool (*cmp)(Synapse & s1, Synapse & s2)) {
	const Neuron * t;
	float w; // added weight to preserve the old hash!! not neccesary in principle
	
	for(int a=start+1; a<count+start; a++)
		for(int b=count+start-1; b>=a; b--)
			if(cmp(afferentSynapses[b-1], afferentSynapses[b])) {
				t = afferentSynapses[b-1].preSynapticNeuron;
				w = afferentSynapses[b-1].weight;
				
				afferentSynapses[b-1].preSynapticNeuron = afferentSynapses[b].preSynapticNeuron;
				afferentSynapses[b-1].weight = afferentSynapses[b].weight;
				
				afferentSynapses[b].preSynapticNeuron = t;
				afferentSynapses[b].weight = w;
			}
}
*/

/*
 // check that maginitutes are reasonable... there is something
// wrong with libconfig exceptions as well!
if(neuronType == DISCRETE) {
	
	// Not usefull anymore, since there is no training
	// output for discrete case
	
	 stepsPrTransform = timeStepsPrInputFile;
	 
	 if(timeStepsPrInputFile <= 0) {
	 cerr << "timeStepsPrInputFile <= 0." << endl;
	 exit(EXIT_FAILURE);
	 }
	 
	 // Test that we actually are going to get training output
	 // when we are asking for it. 
	 if(learningOn && (outputNeurons || outputWeights)) {
	 
	 if(timeStepsPrInputFile < outputAtTimeStepMultiple) {
	 cerr << "There is never any output due to timeStepsPrInputFile < outputAtTimeStepMultiple." << endl;
	 exit(EXIT_FAILURE);
	 }
	 
	 // This test should be after test for: timeStepsPrInputFile < outputAtTimeStepMultiple
	 if(timeStepsPrInputFile % outputAtTimeStepMultiple != 0) {
	 cerr << "Last timestep will not be saved due to timeStepsPrInputFile % outputAtTimeStepMultiple != 0." << endl;
	 exit(EXIT_FAILURE);
	 }
	 }
	 
}
else
*/

/*
 float ESRegion::findThreshold() {
 
 float localThreshold;
 u_short percentileSize = static_cast<u_short>(depth*dimension*dimension*(1-sparsenessLevel));
 
 if(sparsenessRoutine == HEAP) {
 
 u_short added = 0;
 
 // Clear existing content from last run, there is no clear method, so we have to reset
 priority_queue<float,vector<float>,greater<float> > minimumHeap = priority_queue<float,vector<float> ,greater<float> >();
 
 // Iterate layer, and only replace top of heap if bigger then .top is found
 for(u_short d = 0;d < depth;d++)
 for(u_short i = 0;i < dimension;i++)
 for(u_short j = 0;j < dimension;j++) {
 
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
 localThreshold = minimumHeap.top();
 
 } else if(sparsenessRoutine == QSORT) {
 
 u_short d, i, j;
 
 // Copy activations (post inhibition) into sorting array
 for(d = 0;d < depth;d++)
 for(i = 0;i < dimension;i++)
 for(j = 0;j < dimension;j++)
 minimumVector[d*(dimension*dimension) + i*dimension + j] = Neurons[d][i][j].newInhibitedActivation;
 
 // Sort vector
 sort(minimumVector.begin(), minimumVector.end(), greater<float>());
 
 // Find percentile
 localThreshold = minimumVector[percentileSize - 1];
 
 }
 
 return localThreshold;
 }
 */

/*
 // Save outout in newInhibitedActivation
 // CLASSIC
 void ESRegion::inhibit() {
 
 
 //float averageActivation = 0;
 //for(int i = 0;i < dimension;i++)
 //for(int j = 0;j < dimension;j++)
 //averageActivation += Neurons[0][i][j].newActivation;
 
 
 averageActivation /= dimension*dimension;
 

int i, j, n_i, n_j;		 // neuron being inspected by filter
u_short f_i, f_j;		 // filter iteration
float convolutionResult, tmp = 0;
tmp++; //avoid lousy warning, but still keep in #pragma

// Choose neuron to center filter on
#pragma omp for private(i, j, n_i, n_j, f_i, f_j, convolutionResult, tmp)
for(i = 0;i < dimension;i++)
for(j = 0;j < dimension;j++) {
	
	convolutionResult = 0;
	
	// Iterate over neighberhood of (f_i,f_j)
	for(f_i = 0; f_i < inhibitoryWidth;f_i++)
		for(f_j = 0; f_j < inhibitoryWidth;f_j++) {
			
			n_i = i + f_i - filterCenter;
			n_j = j + f_j - filterCenter;
			
			// Wrap around
			n_i = wrap(n_i, dimension);
			n_j = wrap(n_j, dimension);
			
			convolutionResult += Neurons[0][n_i][n_j].newActivation * spatialFilter[f_i][f_j];
			
			// Check that (f_i,f_j) is not outside layer
			//if(n_i >= 0 && n_j >= 0 && n_i < dimension && n_j < dimension)
			//	tmp = Neurons[0][n_i][n_j].newActivation;
			//else
			//	tmp = averageActivation;
			//	
			//	convolutionResult += tmp * spatialFilter[f_i][f_j];
		}
	
	// Save result convolutionResult
	Neurons[0][i][j].newInhibitedActivation = convolutionResult;
}	
}
*/

/*
 // James' Idea
 // RANDOMIZATION OF PRESENTATION ORDER WITHIN EACH OBJECT
 // did not work...
 #pragma omp single
 {
 // if random setting is set
 if(learningOn) {
 
 unsigned object = s/p.nrOfTransformations;
 unsigned trans = gsl_rng_uniform_int(rngController, p.nrOfTransformations);
 //cout << "RND>>  object #" << object << ", trans #" << trans << endl;
 loadThis = object + trans;
 }
 else
 loadThis = s;
 }
 */

/*
 void ESRegion::setupPartialSpatialFilterSums() {
 
 // Choose neuron to center filter on
 for(u_short i = 0;i < dimension;i++)
 for(u_short j = 0;j < dimension;j++) {
 
 float partialSum = 0;
 
 // Iterate over neighberhood of (i,j)
 for(u_short f_i = 0; f_i < inhibitoryWidth;f_i++)
 for(u_short f_j = 0; f_j < inhibitoryWidth;f_j++) {
 
 // Neuron being inspected by filter
 int n_i = i + f_i - filterCenter;
 int n_j = j + f_j - filterCenter;
 
 // Check that (f_i,f_j) is outside layer and add filter component to sum if it is
 if(n_i >= 0 && n_j >= 0 && n_i < dimension && n_j < dimension)
 partialSum += spatialFilter[f_i][f_j];
 }
 
 // Save result partial sum
 partialSpatialFilterSums[i][j] = partialSum;
 }
 }
 */

/*
 // Save outout in newInhibitedActivation
 // WAS SUPPOSED TO BE FAST
 void ESRegion::inhibit() {
 
 float averageActivation = 0;
 for(int i = 0;i < dimension;i++)
 for(int j = 0;j < dimension;j++)
 averageActivation += Neurons[0][i][j].newActivation;
 
 averageActivation /= dimension*dimension;
 
 int i, j, n_i, n_j;		// neuron being inspected by filter
 u_short i_min, i_max, j_min, j_max; // used to to iterate filter that is not outside region
 u_short f_i, f_j;		// filter iteration
 float convolutionResult;
 
 // Choose neuron to center filter on
 #pragma omp for private(i, j, n_i, n_j, i_min, i_max, j_min, j_max, f_i, f_j, convolutionResult)
 for(i = 0;i < dimension;i++)
 for(j = 0;j < dimension;j++) {
 //cout << "here" << endl;
 //cout.flush();
 convolutionResult = 0;
 
 // Iterate over neighberhood of (f_i,f_j) that is not outside layer [i_min, i_max]x[j_min, j_max]
 i_min = i < filterCenter ? filterCenter - i : 0;
 i_max = i + filterCenter + 1 > dimension ? dimension + filterCenter - i - 1 : 2*filterCenter;
 
 j_min = j < filterCenter ? filterCenter - j : 0;
 j_max = j + filterCenter + 1 > dimension ? dimension + filterCenter - j - 1 : 2*filterCenter;			
 
 for(f_i = i_min; f_i <= i_max/2;f_i++)
 for(f_j = j_min; f_j <= j_max/2;f_j++) {
 //cout << "j_max: " << j_max << endl;
 //cout << "j:" << j << " inhibitoryWidth:" << inhibitoryWidth << " filterCenter:" << filterCenter << endl;
 //cout << "there: " << "f_i " << f_i << "f_j" << f_j << endl;
 n_i = i + f_i - filterCenter;
 n_j = j + f_j - filterCenter;
 
 // Check that (f_i,f_j) is not outside layer
 if(n_i >= 0 && n_j >= 0 && n_i < dimension && n_j < dimension)
 convolutionResult += Neurons[0][n_i][n_j].newActivation * spatialFilter[f_i][f_j];
 }
 
 // Save result convolutionResult
 Neurons[0][i][j].newInhibitedActivation = convolutionResult + averageActivation * partialSpatialFilterSums[i][j];
 }	
 }
 */



/*#include "utilities.h"

#include <sstream>

// should we pass ref back? read up
vector<string> split(const string & s, char delim) {
	
	vector<string> elems;
	stringstream ss(s);
    string item;
	
    while(getline(ss, item, delim))
        elems.push_back(item);
	
    return elems;
}

template <class T>
vector<T> splitAndConvert(char * s) {
	
	vector<string> splitted = split(string(s),',');
	vector<T> converted;
	
	for(unsigned i = 0;i < splitted.size();i++) {
		istringstream ss(splitted[i]);
		T t;
		ss >> t;
		converted.push_back(t);
	}
	
	return converted;
}

// template compile issue requires this
template vector<unsigned int> splitAndConvert<unsigned int>(char * s);
template vector<double> splitAndConvert<double>(char * s);
template vector<bool> splitAndConvert<bool>(char * s);

template <class T>
T convert(char * s) {
	istringstream ss(s);
	T t;
	ss >> t;
	return t;
}

template unsigned int convert<unsigned int>(char * s);
template bool convert<bool>(char * s);

vector<string> directoryListing (const char * directory) {
	DIR *dp;
	struct dirent *ep;
	vector<string> files;
 
	dp = opendir (directory);
	if (dp != NULL) {
		while (ep = readdir (dp))
			files.push_back(ep->d_name);
 
		closedir (dp);
	}
 
	return files;
}
*/

/*
void Network::setupAfferentSynapsesV2() {
    
    unsigned radius = p.radii[0];
    
    // Region sizes
    unsigned preSynapticRegionSize = striateCortex.dimension;
	unsigned postSynapticRegionSize = ESPathway[0].dimension;
	
	// Distance betweene postsynaptic neurons
	double v2Padding = preSynapticRegionSize/postSynapticRegionSize;
    
    // Offset that centers destination region over source region    
    double offset = v2Padding/2;
    
    // Go through postsynaptic neurons
    for(unsigned i = 0;i < postSynapticRegionSize;i++) {
        for(unsigned j = 0;j < postSynapticRegionSize;j++) {
			
			ESNeuron * postSynapticNeuron = &ESPathway[0].Neurons[0][i][j];

			// Subsample each wavelength to the required degree dictated by waveLengthConnections
			for(int wavelengthIndex = 0;wavelengthIndex < p.waveLengthConnections.size();wavelengthIndex++) {
				
				unsigned connectionsMade = 0;
				unsigned connectionsToMake = p.waveLengthConnections[wavelengthIndex];
				
				// Continue to resample bivariate gaussian until we have made enough connections.
				// A sample may miss if it
				// 1) Samples outside the size of the presynaptic layer
				// WARNING: The check on that is actually not 100% correctly checked, since 
				// it will bias against all edge neurons since they loose 
				// half their "hit range"... but who cares!
				// 2) Suggests a connection to an already connected neuron
				while(connectionsMade < connectionsToMake) {
					
					// Sample
					double x,y;
					gsl_ran_bivariate_gaussian(rngController, radius, radius, 0, &x, &y);
					
					double xMean = j*v2Padding + offset;
					double yMean = i*v2Padding + offset;
					
					// DO BETTER ROUNDING BASED ON [0,1] OVERFLOW !!!
					int xSource = static_cast<int>(x + xMean);
					int ySource = static_cast<int>(y + yMean);

					if(xSource > 0 && ySource > 0 && xSource < preSynapticRegionSize && ySource < preSynapticRegionSize) {
						
						// Sample phase and orientation
						unsigned phaseIndex = gsl_rng_uniform_int(rngController,striateCortex.filterPhases.size());
						unsigned orientationIndex = gsl_rng_uniform_int(rngController,striateCortex.filterOrientations.size());
						
						V1Neuron * preSynapticNeuron = striateCortex.getV1Neuron(ySource, xSource, orientationIndex, wavelengthIndex, phaseIndex);
						
						
						// Make sure we dont reconnect
						if(!postSynapticNeuron->areYouConnectedTo(preSynapticNeuron)) {
							
							double weight = 0;
							
							if(p.initialWeight != ZERO) 
								weight = gsl_rng_uniform(rngController);
							
							postSynapticNeuron->addAfferentSynapse(weight, preSynapticNeuron);
							
							connectionsMade++;
						}
					}
					else
						connectionsMade++;
				}
			}
			
			if(p.weightNormalization == CLASSIC)
                postSynapticNeuron->normalize(norm);
        }
    }   
}


// Read input to network
					ostringstream dirStream;
					dirStream << inputDirectory << "/Filtered/" << dirNameBase << ".flt/" << dirNameBase << '.' << p.filterWavelengths[wavelength] << '.' << p.filterOrientations[orientation] << '.' << p.filterPhases[phase] << ".gbo";
					string t = dirStream.str();
					
					// Open&Read gabor filter file
					ifstream gaborStream(t.c_str(), ios::in|ios::binary);
					if(gaborStream.fail()) {
						dirStream << " unable to open";
						string s = dirStream.str();
						throw new NetworkRunException(s.c_str());
					}
					
					gaborStream.read((char *)tmpBuffer, dimension*dimension*sizeof(float));
					
					// Read flat buffer into 2d slice of V1
					unsigned unsigned d = mapToV1Depth(orientation,wavelength,phase);
					for(unsigned i = 0;i < dimension;i++) {
						for(unsigned j = 0;j < dimension;j++) {
							
							float firing = tmpBuffer[i*dimension + j];
							
							buffer[s][d][i][j] = 1/(1+exp(-2 * 30 * (firing - 3)));

						}
					}

*/
        /*
		string s(outputDirectory);
		s.append("synapticWeights.dat");
		fstreamWrapper weightFile;
		
		try {
			weightFile.open(s, std::ios::out | std::ios_base::binary);
		} catch (fstream::failure e) {
			stringstream s;
			s << "Unable to open " << s << " for history output: " << e.what();
			cerr << s.str();
			exit(EXIT_FAILURE);
		}

		weightFile << p.nrOfEpochs;
		weightFile << p.nrOfObjects;
		weightFile << p.nrOfTransformations;
		weightFile << p.timeStepsPrInputFile/p.outputAtTimeStepMultiple;
		
		// Output the size of all layers
        // this is needed for plotting of
		// receptive field structure where you also need sizes of layers
		// that are not them selfs outputted

		weightFile << static_cast<unsigned>(p.dimensions.size());
		
        for(unsigned k = 0;k < p.dimensions.size();k++)
            weightFile << p.dimensions[k];
		*/

	/*
	config_t cfg;
	config_setting_t *setting;

	config_init(&cfg);

	// Read the file. If there is an error, report it and exit.
	if(!config_read_file(&cfg, filename)) {
		fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
				config_error_line(&cfg), config_error_text(&cfg));
		config_destroy(&cfg);
		//return(EXIT_FAILURE);
	}
	*/
	/*

	int tmp;

	lookup(const char * path, &tmp);
	neuronType = static_cast<NEURONTYPE>(tmp);

	if(config_lookup_int(&cfg, "name", &tmp))
		neuronType = static_cast<NEURONTYPE>(tmp);
	else
		fprintf(stderr, "No 'neuronType' setting in configuration file.\n");

	if(config_lookup_int(&cfg, "feedback", &tmp))
		feedback = static_cast<FEEDBACK>(tmp);
	else
		fprintf(stderr, "No 'feedback' setting in configuration file.\n");

	if(config_lookup_int(&cfg, "initialWeight", &tmp))
		initialWeight = static_cast<INITIALWEIGHT>(tmp);
	else
		fprintf(stderr, "No 'initialWeight' setting in configuration file.\n");

	if(config_lookup_int(&cfg, "weightNormalization", &tmp))
		weightNormalization = static_cast<WEIGHTNORMALIZATION>(tmp);
	else
		fprintf(stderr, "No 'weightNormalization' setting in configuration file.\n");

	if(config_lookup_int(&cfg, "sparsenessRoutine", &tmp))
		sparsenessRoutine = static_cast<SPARSENESSROUTINE>(tmp);
	else
		fprintf(stderr, "No 'sparsenessRoutine' setting in configuration file.\n");




	cfg.lookupValue("sparsenessRoutine",tmp);
	sparsenessRoutine = static_cast<SPARSENESSROUTINE>(tmp);
		*/

	/* Output a list of all books in the inventory. */
	/*
	setting = config_lookup(&cfg, "inventory.books");
	if(setting != NULL) {
		int count = config_setting_length(setting);
		unsigned i;

		printf("%-30s  %-30s   %-6s  %s\n", "TITLE", "AUTHOR", "PRICE", "QTY");

		for(i = 0; i < count; ++i) {
			config_setting_t *book = config_setting_get_elem(setting, i);

			// Only output the record if all of the expected fields are present.
			const char *title, *author;
			double price;
			int qty;

			if(!(config_setting_lookup_string(book, "title", &title)
				&& config_setting_lookup_string(book, "author", &author)
				&& config_setting_lookup_float(book, "price", &price)
				&& config_setting_lookup_int(book, "qty", &qty)))
			continue;

			printf("%-30s  %-30s  $%6.2f  %3d\n", title, author, price, qty);
		}
		putchar('\n');
	}

	config_destroy(&cfg);
	*/

/*
void Param::lookup(const char * path, double * value) {

	if(!config_lookup_float(&cfg, path, value)) {

		char buff[100];
		ret = sprintf(buff, "No '%d' setting in configuration file.\n",path);
		fprintf(stderr, buff);
	}
}
*/

		// inlined because of template
		/*
		template <class T>
		void lookup(const char * path, T * value) {

			int tmp;
			if(!config_lookup_int(&cfg, path, &tmp)) {

				char buff[100];
				ret = sprintf(buff, "No '%s' setting in configuration file.\n",path);
				fprintf(stderr, buff);
			}
			else
				*value = static_cast<T>(tmp);
		}*/


        /*
        //vector<bool> outputRegion;			// Not for V1
		//vector<unsigned> regionsToOutput;		// Not for V1, this one is just deduced from outputRegion
	    */

/*
 *  Exception.h
 *  VisBack
 *
 *  Created by Bedeho Mender on 11/30/10.
 *  Copyright 2010 OFTNAI. All rights reserved.
 *
 */

//#ifndef EXCEPTION_H
//#define EXCEPTION_H

/*
class ParamLoadException {
public:
    ParamLoadException(const char * message);
};

class NetworkBuildException {
public:
    NetworkBuildException(const char * message);
};

class NetworkRunException {
public:
    NetworkRunException(const char * message);
};

*/

//#endif // EXCEPTION_H


//#include "Exception.h"

/*
#include <iostream>
#include <string>
#include <stdlib.h>

using namespace std;

ParamLoadException::ParamLoadException(const char * message) {
	cerr << "Param file loading error: " << message;
	exit(EXIT_FAILURE);
}

ParamLoadException::ParamLoadException(const string * message) {
	cerr << "Param file loading error: " << message;
	exit(EXIT_FAILURE);
}

NetworkBuildException::NetworkBuildException(const char * message) {
	cerr << "Network build error: " << message;
	exit(EXIT_FAILURE);
}

NetworkRunException::NetworkRunException(const char * message) {
	cerr << "Network run error: " << message;
	exit(EXIT_FAILURE);
}

*/

