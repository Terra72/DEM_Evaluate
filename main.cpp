/*
DEM_Evaluate/main.cpp
Author: Daniel Bowden BWDDAN001

DEM_Evaluate contains the functionality for testing generated DEMs created by the DEM_Generation program
and the resultant DEMs produced by external image processing methods to estimate ground and trees.

Tree Segmentation:
The results from tree segmentation are compared against the original generated DEMs to see if the correct tree masks were created by the watershed method
Ground Estimation:
The results from ground estimation ate compared against the landscapes used to create the generated DEMs

*/

#include <iostream>
#include <iomanip> 
#include <list>
#include <algorithm>
#include <fstream>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

using namespace cv;
using namespace std;

vector<int*> getTrueBoxes(int width, String filePath);
vector<int*> getWatershedBoxes(String filePath);
void compareBoxes(vector<int*> trueBoxes,vector<int*> estimatedBoxes);
float calculateIOU(int*boxA, int*boxB);
void evaluateGround(int treeWidth, String filePath);
vector<vector<Point>> getTrees(int treeWidth, string filePath);
bool insideCircle(Point center, Point tile, float radius);

int main(int, char**) {
//map list:
//gentle,gentleEasy,gentleSpread,gentleSmall,gentleSmallEasy
//steep,steepEasy,steepSpread,steepSmall,steepSmallEasy
//contourHillJoin,contourHillJoinEasy,contourHillJoinSpread,contourHillJoinSmall,contourHillJoinSmallEasy
//contourHillSmallSpread,contourHill,contourHillEasy,contourHillSpread,contourHillSmall,contourHillSmallEasy
//flat,flatSmallSpread,flatSpreadEasy,flatEasy,flatSpread,flatSmall,flatSmallEasy
//hillsSmooth,hills,hillsEasy,hillsSpread,hillsSmall,hillsSmallEasy
 
    String resultName = "contourHillJoin"; 
    
    int treeWidth = 15; //set width = 15 for small, 30 by default
    cout << "Map List: " << endl;
    cout << "gentle,gentleEasy,gentleSpread,gentleSmall,gentleSmallEasy" << endl;
    cout << "steep,steepEasy,steepSpread,steepSmall,steepSmallEasy" << endl;
    cout << "contourHillJoin,contourHillJoinEasy,contourHillJoinSpread,contourHillJoinSmall,contourHillJoinSmallEasy" << endl;
    cout << "contourHillSmallSpread,contourHill,contourHillEasy,contourHillSpread,contourHillSmall,contourHillSmallEasy" << endl;
    cout << "flat,flatSmallSpread,flatSpreadEasy,flatEasy,flatSpread,flatSmall,flatSmallEasy" << endl;
    cout << "hillsSmooth,hills,hillsEasy,hillsSpread,hillsSmall,hillsSmallEasy" << endl;
    cout << "gentle,gentleEasy,gentleSpread,gentleSmall,gentleSmallEasy" << endl;
    cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
    cout << "Input map choice: (Ignore estimated ground results for 'gentle' maps!)" << endl;
    cin >> resultName;
    cout << "Set the correct tree width/diameter: [15 for small, 30 for normal]" << endl;
    cin >> treeWidth;
    String path = "input/results/" + resultName;
    cout << "'" << resultName << "' DEM evaluation:" << endl;
    
    //required functionality to test ground estimation
    evaluateGround(treeWidth, path);

    //required functionality to test tree segmentation
    vector<int*> trueBoxes = getTrueBoxes(treeWidth , path);
    vector<int*> waterShedBoxes = getWatershedBoxes(path);
    compareBoxes(trueBoxes,waterShedBoxes);

    return 0;
}

/*
getTrueBoxes return function, returning a vector<int*> that contains the bounding box values for each true tree based on centres.tif
These are bounding boxes for each tree in final.tif as well
*/
vector<int*>  getTrueBoxes(int width, String filePath){
    vector<int*> tempBoxes;
    String tifFile = filePath + "/centres.tif";
    Mat treeCentres = imread(tifFile, IMREAD_LOAD_GDAL |IMREAD_UNCHANGED);
    for(int i = 0 ; i < treeCentres.rows ; i++){
        for(int j = 0 ; j < treeCentres.cols ; j++){
            if(treeCentres.at<float>(j,i) != 0){              
                tempBoxes.push_back(new int[4]{i - width/2, j-width/2, i +width/2, j + width/2});
            }
        }
    }
    return tempBoxes;
}
/*
getWatershedBoxes return function, returning a vector<int*> that groups each watershed estimated tree based on pixel colours

Each estimated tree is given a unique colour by the watershed estimated methods, using each unqiue colour we create a new group of pixels
*/
vector<int*>  getWatershedBoxes(String filePath){
    vector<int*> tempBoxes;
    //all non-black pixels divided up into groups of positions based on their colour
    vector<vector<Point>> colourPosList;
    //each non-black colour gets added to the colourList
    //index corresponds with colourPosList vector's index , ie colourPosList[i] and colourList[i] refer to data about the same colour
    vector<Vec3b> colourList;
    String tifFile = filePath + "/watershed.tif";
    Mat watershedImage = imread(tifFile, IMREAD_LOAD_GDAL |IMREAD_UNCHANGED);
   //find each new colour in the image add it to the list of colours
    //store each pixel location for each colour together
    for(int i = 0 ; i < watershedImage.rows ; i++){
        for(int j = 0 ; j < watershedImage.cols ; j++){
            Vec3b pixelColour = watershedImage.at<Vec3b>(j,i);
            if(pixelColour != Vec3b(0,0,0)){
                bool colourFound = false;
                int k = 0;
                for (Vec3b const &col: colourList ){
                    if(pixelColour == col){                        
                        colourFound = true;                        
                        colourPosList[k].push_back(Point(i,j));                        
                    }
                    k++;
                }
                if(!colourFound){ 
                    //add a new pixel colour group to the list
                    colourList.push_back(pixelColour);
                    colourPosList.push_back({Point(i,j)});
                }
            }
        }
    }
    
    //search through the colourPositions finding the max and min position values for each colour, these act as the boundix box
    int listIndex = 0;
    for (vector<Point> const &listItem: colourPosList ){
        int minX = 500;
        int maxX = 0;   
        int minY = 500;
        int maxY = 0;
        for (Point const &pos: listItem ){
            if(pos.x < minX){
            minX = pos.x;
            }
            if(pos.y < minY){
            minY = pos.y;
            }
            if(pos.x > maxX){
            maxX = pos.x;
            }
            if(pos.y > maxY){
            maxY = pos.y;
            }
        }
        //add a new bounding box for each colour        
        tempBoxes.push_back(new int[4]{minX,minY,maxX,maxY});
        listIndex++; 
    }    

    namedWindow("display",WINDOW_NORMAL);	
	resizeWindow("display", 1000,1000);
    imshow("display", watershedImage);
    waitKey();
    return tempBoxes;
}

//compare each estimated bounding box to each true bounding box findin the highest IOU
//output the average IOU and standard deviation and the number of trees found versus actual nuber of trees
void compareBoxes(vector<int*> trueBoxes, vector<int*> estimatedBoxes){
    //list with highest IOU value for each estimated box
    vector<float> IOUValues; 
    float totalIOU = 0;
    //calculate the IOU for each estimated tree box by the watershed against each true box in thei image
    
    for( int i = 0 ; i < estimatedBoxes.size() ; i ++){
        float maxIOU = 0;
        for ( int j = 0 ; j < trueBoxes.size() ; j ++){

            float tempIOU = calculateIOU(trueBoxes[j], estimatedBoxes[i]);
            if(tempIOU > maxIOU){
                maxIOU = tempIOU;
            }
        }
        //store the highest found IOU as the closest match
        IOUValues.push_back(maxIOU);
        totalIOU += maxIOU;
    }
    //calculate average IOU for whole Image
    float avgIOU = totalIOU/estimatedBoxes.size();
    //calculate standard deviation for avgIOU
    float diffSum = 0;
    for( int i = 0 ; i < IOUValues.size() ; i ++){
        diffSum += (IOUValues[i] - avgIOU) * (IOUValues[i] - avgIOU);
    }
    float stdDev = sqrt(diffSum/estimatedBoxes.size());
    //output tree segmentationresults
    cout << "No. of trees found: " << estimatedBoxes.size() << " True no. of trees: " << trueBoxes.size() << endl;
    cout << "mean IOU: " << avgIOU << " stdDev: " << stdDev << endl;
}

//intersection over union calculation for two bounding boxes
float calculateIOU(int*boxA, int*boxB){
    // determine the position of the intersection rectangle between the two boxes
    int xA = max(boxA[0], boxB[0]);
    int yA = max(boxA[1], boxB[1]);
    int xB = min(boxA[2], boxB[2]);
    int yB = min(boxA[3], boxB[3]);
    // compute the area of intersection rectangle
    float interArea = max(0, xB - xA + 1) * max(0, yB - yA + 1); //add 1 to intersection and box areas to prevent divide by zero
    // compute the area of both the prediction and ground-truth rectangles
    float boxAArea = (boxA[2] - boxA[0] + 1) * (boxA[3] - boxA[1] + 1);
    float boxBArea = (boxB[2] - boxB[0] + 1) * (boxB[3] - boxB[1] + 1);
    // compute IOU by dividing area by prediction + ground-truth area - intersection area
    float iou = interArea / float(boxAArea + boxBArea - interArea);
    return iou;
}
//read in the ground truth map and compare it to the estimated height values, 
//output a map of the grove region, split up into cells of average height differences across that area
void evaluateGround(int treeWidth, String filePath){
    //find all the patches of pixel positions based on where the trees occluded the ground
    vector<vector<Point>> trees = getTrees(treeWidth,filePath);

    //load the underlying landscape DEM and the estimated ground DEM
    Mat outImage(500, 500, CV_8UC3); 
    String trueTifFile = filePath + "/landscape.tif";
    Mat trueLand = imread(trueTifFile, IMREAD_LOAD_GDAL |IMREAD_UNCHANGED);
    String estTifFile = filePath + "/estimate.tif";
    Mat estimatedLand = imread(estTifFile, IMREAD_LOAD_GDAL |IMREAD_UNCHANGED);

    float tempTotalEst = 0;
    float tempTotalTrue = 0;
    float tempAvgEst = 0;
    float tempAvgTrue = 0;
    float avgDiff = 0;
    
    vector<float> estAverages;
    vector<float> trueAverages;
    //for each tree in the true ground DEM and the estimated DEM compute the average height
    for(int  i = 0; i < trees.size(); i++){
        for(int  j = 0; j < trees[i].size(); j++){
            tempTotalEst += estimatedLand.at<float>(trees[i][j].y,trees[i][j].x);
            tempTotalTrue += trueLand.at<float>(trees[i][j].y,trees[i][j].x);
        }
        //compute the averages
        tempAvgEst = tempTotalEst/trees[i].size();
        tempAvgTrue = tempTotalTrue/trees[i].size();
        avgDiff = abs(tempAvgTrue - tempAvgEst);
        //add these averages to temporary vectors
        estAverages.push_back(tempAvgEst);
        trueAverages.push_back(tempAvgTrue);

        tempTotalEst = 0;
        tempTotalTrue = 0;
    }

    //initialize colour group
    Vec3b green = {0,100,0};
    Vec3b orange = {0,50,100};
    Vec3b red = {0,0,100};
    float tempTotalDiff = 0;
    //for each pixel group compute the difference between the average heights
    for(int  i = 0; i < trees.size(); i++){
        float tempDiff = abs(estAverages[i] - trueAverages[i]);
        tempTotalDiff += tempDiff;
        for(int  j = 0; j < trees[i].size(); j++){
            if(tempDiff < 0.5){
                outImage.at<Vec3b>(Point(trees[i][j].x,trees[i][j].y)) = green;
            }
            else if(tempDiff < 2){
                outImage.at<Vec3b>(Point(trees[i][j].x,trees[i][j].y)) = orange;
            }
            else{
                outImage.at<Vec3b>(Point(trees[i][j].x,trees[i][j].y)) = red;
            }            
        }
    }
    //calculate average height difference across all estimated ground values
    avgDiff = tempTotalDiff/trees.size();
    //calculate standard deviation for avgDiff
    float diffSum = 0;
    for(int  i = 0; i < trees.size(); i++){
        float tempDiff = abs(estAverages[i] - trueAverages[i]);
        diffSum += abs(tempDiff - avgDiff) * abs(tempDiff - avgDiff);
    }
    float stdDev = sqrt(diffSum/trees.size());
    //output ground comparison results
    cout << "ground Avg diff: " << avgDiff << " stdDev: " << stdDev << endl;

    //display visual comparison of estimated ground areas
    namedWindow("display",WINDOW_NORMAL);	
	resizeWindow("display", 1000,1000);
    imshow("display", outImage);
    waitKey();
    imwrite("output/regionAvg.tif",outImage);
    //output each height difference for each tree to a textfile
    ofstream averagesFile ("output/averages.txt");
    if (averagesFile.is_open())
    {
        for(int i = 0 ; i < estAverages.size() ; i++){
            averagesFile << format("%2.2f",estAverages[i]) << " " << format("%2.2f",trueAverages[i])<< "\n"; //<<"," << format("%0.2f",avgDifferences[i]) 
        }
        averagesFile.close();
    }
}


/*
getTrees return function, returning a vector<vector<Point>> data structure
groups pixels into trees based on the centre pixel found in centres.tif

Each pixel group vector<Point> represents a tree and all of these trees are grouped together in the vector<vector<Point>> data structure
Used in evaluating ground as the true tree pixel positions that need to be compared against the estimated ground results

*/
vector<vector<Point>> getTrees(int treeWidth, string filePath){
    vector<vector<Point>> tempTrees;
    String tifFile = filePath + "/centres.tif";
    Mat treeCentres = imread(tifFile, IMREAD_LOAD_GDAL |IMREAD_UNCHANGED);
    for(int i = 0 ; i < treeCentres.rows ; i++){
        for(int j = 0 ; j < treeCentres.cols ; j++){
            if(treeCentres.at<float>(j,i) != 0){              
                vector<Point> tempPixels;
                for(int k = 0; k < treeWidth ; k++){
                    for(int p = 0; p < treeWidth ; p++){
                        if(insideCircle({treeWidth/2,treeWidth/2},{k,p},treeWidth/2)){                        
                            tempPixels.push_back({i + k - treeWidth/2, j + p - treeWidth/2}); //tree position + pixel in the circle offset - radius                                                                                                
                        }                                                                       //to get the correctly placed pixel in the tree circle
                    }
                }      
                tempTrees.push_back(tempPixels);
            }
        }
    }
    return tempTrees;
}

/*
insideCircle return function returning true for a point inside a circle of a given radius
*/
bool insideCircle(Point center, Point tile, float radius) {
    float dx = center.x - tile.x;
    float dy = center.y - tile.y;
    float distance_squared = dx*dx + dy*dy;
    return distance_squared <= radius*radius;
}
