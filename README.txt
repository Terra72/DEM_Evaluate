author: Daniel Bowden BWDDAN001

DEM_Evaluate Layer

Requires openCV 4 to run: 
https://opencv.org/
https://www.learnopencv.com/install-opencv-4-on-ubuntu-18-04/

-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
To Build: 
while in DEM_Generation/build directory:
	make

To run:
while in DEM_Generation directory:
	./build/quickTest

-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Map list:

gentle,gentleEasy,gentleSpread,gentleSmall,gentleSmallEasy
steep,steepEasy,steepSpread,steepSmall,steepSmallEasy
contourHillJoin,contourHillJoinEasy,contourHillJoinSpread,contourHillJoinSmall,contourHillJoinSmallEasy
contourHillSmallSpread,contourHill,contourHillEasy,contourHillSpread,contourHillSmall,contourHillSmallEasy
flat,flatSmallSpread,flatSpreadEasy,flatEasy,flatSpread,flatSmall,flatSmallEasy
hillsSmooth,hills,hillsEasy,hillsSpread,hillsSmall,hillsSmallEasy
-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Description of input choices:

Input map choice: (Ignore estimated ground results for 'gentle' maps!)
	choose a map from the map list to see the compared results
Set the correct tree width/diameter: [15 for small, 30 for normal]
	choose the diamater appropriately for the map choice, 15 for small, 30 for all others

-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
EXAMPLE INPUT:

Input map choice: (Ignore estimated ground results for 'gentle' maps!)
gentleEasy
Set the correct tree width/diameter: [15 for small, 30 for normal]
30


-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
