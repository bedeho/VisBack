
**********
* README *
**********


*************************
* DEPLOYMENT OF VISBACK *
*************************
1. Make a designated directory <DIR>
2. Make the following directory structure
  <DIR>
	|-Experiments
	|-Results
	|-Scripts
	|-Stimuli
	|-Writing
	|-Xgrid
3. Clone down git@github.com:bedeho/VisBack.git into <DIR> and rename to "Source"
4. Clone down git@github.com:bedeho/VisBackMatlabScripts.git into <DIR/Scripts> and rename to "Analysis"
5. Clone down git@github.com:bedeho/RunScripts.git into <DIR/Scripts> and rename to "Run"
6. Make folder 
  InfoAnalysis 
    |-infoanalysis
    |-infoplot
    |-inform_visnet
    |-infors_visnet
7. Make folder "Scripts/DataTables-1.8.2" with contents from http://www.datatables.net/extras/tabletools/plug-ins
8. Manually get myConfig.pm and put in "Scripts/Run/myConfig.pm" change $BASE to <DIR>, remember to add trailing slash, also change
$PROGRAM to indicate the folder which has the binary "VisBack" in it, this value is typically a result of where your IDE compiler puts the final binary.
9. Manually get declareGlobalVars.m and put in "Scripts/Analysis/declareGlobalVars.m" change variable "base" to <DIR>, and remember to have trailing slash
10. Download and install libconfig from http://www.hyperrealm.com/libconfig/
11. Download and install GSL
12. Setup xcode/IDE project which sets header and library paths to GSL and libconfig
13. Add linker flag -lconfig++ -gsl
14. Make sure openMP is enabled in IDE settings/compiler flags
15. Compile project and make sure to chose same bit depth (32,64) as the compiled libraries (GSL,libconfig)

***************
* ADD STIMULI *
***************
1. Make a new subfolder of /Stimuli which has the appropriate name
2. Make a new subfolder, /Images, of this folder and put all your stimuli .png images in here
3. Make sure all your stimuli pictures are of identical dimension AND HAVE PERFECT HOMOGENOUS BACKROUNG COLOR,
4. call the Matlab script filterImageSet.m and pass it the required parameters, in particular the gray scale code of the background mentioned.
