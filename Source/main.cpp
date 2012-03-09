
#include "Network.h"
#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>
//#include "utilities.h"

#ifndef OMP_DISABLE
	#include <omp.h>
#endif

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::string;

void usage();

int main (int argc, char *argv[]) {

	// Iterate command line options
	bool verbose = false;
	bool xgrid = false;
	
	#ifndef OMP_DISABLE
		int numberOfThreads = (3 * omp_get_num_procs())/4; // Ben's advice, use 75% of cores
	#else
		int numberOfThreads = 1;
	#endif
	
	//dumping stuff for xgrid
	//for(int j = 0;j < argc;j++)
	//	cout << j << ": " << argv[j] << endl;
	//cout << endl;
	//dumping stuff

	int i = 1;
	for(;i < argc;i++) {

		if(argv[i][0] != '-' || argv[i][1] != '-')	// break on first non-option token
			break;
		if(strcmp("--help", argv[i]) == 0) {
			usage();
			return 0;
		}
		else if(strcmp("--verbose", argv[i]) == 0)
			verbose = true;
		else if(strcmp("--xgrid", argv[i]) == 0)
			xgrid = true;
		else if(strcmp("--singlethreaded", argv[i]) == 0)
			numberOfThreads = 1;
		else {
			cout << "Unknown option: " << argv[i] << endl;
			usage();
			return 1;
		}
	}
	
	// Iterate command line arguments
	if(argc - i < 3)
		cout << "Expected atleast three arguments." << endl;
	else {

		if(strcmp("build", argv[i]) == 0) {

			if(argc - i != 3)
				cout << "Expected three arguments: build <parameter file> <output directory>" << endl;
			else {

				cout << "Building network..." << endl;
				Network n(argv[i + 1], verbose);

				cout << "Saving network..." << endl;
				string s(argv[i + 2]);
				s.append("BlankNetwork.txt");
				n.saveNetwork(s.c_str());
			}

		} else if(strcmp("train", argv[i]) == 0) {
			
			const char * inputDir = "./Filtered/", * outputDir = "./", * fileList = "FileList.txt";
			
			if(xgrid) {
				
				if(argc - i != 3) {
					cout << "Expected three arguments: train <parameter file> <untrained network file>" << endl;
					return 1;
				}
				#ifndef OS_WIN
				// On linux/mac we must untar result when xgrid is used
				else {
					system("tar -xjf xgridPayload.tbz");
				}
				#endif
			} else {
				
				if(argc - i != 6) {
					cout << "Expected five arguments: train <parameter file> <untrained network file> <file list> <input directory> <output directory>" << endl;
					return 1;
				}
				else {
					//param = argv[i + 1];
					//weight = argv[i + 2];
					fileList = argv[i + 3];
					inputDir = argv[i + 4];
					outputDir = argv[i + 5];
				}
			}
			
			cout << "Loading network..." << endl;
			Network n(fileList, argv[i + 1], verbose, argv[i + 2], true);
			
			cout << "Training network..." << endl;
			n.run(inputDir, outputDir, true, numberOfThreads, xgrid);

			cout << "Saving network..." << endl;
			string s(outputDir);
			s.append("TrainedNetwork.txt");
			n.saveNetwork(s.c_str());
			
			#ifndef OS_WIN
			// On linux/mac we tar and cleanup when xgrid is used
			if(xgrid) {
				system("tar -cjf result.tbz *.dat");
				system("rm *.dat"); // delete all results
				system("rm -R Filtered"); // delete all untared input stimuli
			}			
			#endif
			
		} else if(strcmp("test", argv[i]) == 0) {
			
			if(xgrid) {
				cerr << "No support for testing on grid..." << endl;
				return 1;
			} else if(argc - i != 6) {
				cout << "Expected five arguments: test <parameter file> <trained network file> <file list> <input directory> <output directory>" << endl;
				return 1;
			}

			cout << "Loading network..." << endl;
			Network n(argv[i + 3], argv[i + 1], verbose, argv[i + 2], false);

			cout << "Testing network..." << endl;
			n.run(argv[i + 4], argv[i + 5], false, numberOfThreads, xgrid);

		} else if(strcmp("loadtest", argv[i]) == 0) {

			cout << "Loading network..." << endl;
			Network n(NULL, argv[i + 1], verbose, argv[i + 2], false);

			cout << "Saving network..." << endl;
			string s(argv[i + 3]);
			s.append("LOADTEST.txt");
			n.saveNetwork(s.c_str()); 
		}
		else
			cout << "Unknown command." << endl;
	}
}

void usage() {

	cout << endl;
	cout << "usage: visback [--help] [--verbose]  [--xgrid] [--multicore] " << endl; // [--silent]
    cout << "               COMMAND ARGS" << endl;
    cout << endl;
    cout << "The command list for visback is:" << endl;

	cout << "\t build\t Build new network." << endl;
	cout << "\t\t\t build <parameter file> <output directory>" << endl;

	cout << "\t run\t Train built network." << endl;
	cout << "\t\t\t  train <parameter file> <untrained network file> <input directory> <output directory>" << endl;

	cout << "\t run\t Test trained network." << endl;
	cout << "\t\t\t  test <parameter file> <untrained network file> <input directory> <output directory>" << endl;
}