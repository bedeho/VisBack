
**********
* README *
**********

*************************
* DEPLOYMENT OF VISBACK *
*************************
1. Install libraries:

GSL: http://www.gnu.org/software/gsl/, 
Libconfig: http://www.hyperrealm.com/libconfig/,

Consult readme files to learn how to change install location,
defualt is /usr/local.

OBSERVE: Steps (2-4) may be done in an IDE or with g++ directly

2. Set linker flags: -lconfig++ -lgsl -lgslcblas

3. Set the header file and library search
paths of the two libraries, by default /usr/local/include
and /usr/local/lib respectively.

4. Set flag -fopenmp with compiler and linker.

5. Compile.

6. In "Scripts/Run/myConfig.pm" change 
  $BASE to the top level directory of VisBack.
  $PROGRAM to indicate the folder which has the binary "VisBack" in it, this value is typically a result of where your IDE compiler puts the final binary.
  $MATLAB to indicate the path where the matlab binary is located.

7. In "Scripts/Analysis/declareGlobalVars.m" change variable "base" to top level directory of VisBack.

8. Recompile binaries in folder "Scripts/InfoAnalysis".

***************
* ADD STIMULI *
***************
1. Make a new subfolder of /Stimuli which has the appropriate name
2. Make a new subfolder, /Images, of this folder and put all your stimuli .png images in here: they must be identical square dimension AND HAVE PERFECT HOMOGENOUS BACKGROUND COLOR
4. call the Matlab script filterImageSet.m and pass it the required parameters, in particular the gray scale code of the background mentioned.
