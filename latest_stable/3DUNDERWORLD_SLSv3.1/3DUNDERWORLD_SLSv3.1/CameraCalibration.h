//------------------------------------------------------------------------------------------------------------
//* Copyright © 2010-2013 Immersive and Creative Technologies Lab, Cyprus University of Technology           *
//* Link: http://ict.cut.ac.cy                                                                               *
//* Software developer(s): Kyriakos Herakleous                                                               *
//* Researcher(s): Kyriakos Herakleous, Charalambos Poullis                                                  *
//*                                                                                                          *
//* This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.*
//* Link: http://creativecommons.org/licenses/by-nc-sa/3.0/deed.en_US                                        *
//------------------------------------------------------------------------------------------------------------

#pragma once

#include <conio.h>
#include <iostream>
#include "cv.h"
#include "highgui.h"
#include "Utilities.h"
#include <direct.h>



class CameraCalibration
{
	 
public:

	#define CAMCALIB_OUT_MATRIX 1
	#define CAMCALIB_OUT_DISTORTION 2
	#define CAMCALIB_OUT_ROTATION 3
	#define CAMCALIB_OUT_TRANSLATION 4

	CameraCalibration(void);
	~CameraCalibration(void);

	int calibrateCamera();

	void loadCameraImgs(const char *path);
	void unloadCameraImgs();

	void saveCalibData(const char *pathNfilename);
	void loadCalibData(const char *pathNfilename);
	bool findCameraExtrisics();
	//set data
	void	setSquareSize(cv::Size size);
	cv::Size getSquareSize();

	void	setNumberOfCameraImgs(int num);
	int		getNumberOfCameraImgs();

	void	exportTxtFiles(const char *pathNfileName, int CAMCALIB_OUT_PARAM);

	void	printData();

	int		extractImageCorners();

	cv::Mat camMatrix;
	cv::Mat distortion;
	cv::Mat rotationMatrix;
	cv::Mat translationVector;

private:

	//functions
	bool						findCornersInCamImg(cv::Mat camImg,cv::vector<cv::Point2f> *camCorners,cv::vector<cv::Point3f> *objCorners);
	void						drawOutsideOfRectangle(cv::Mat img,cv::vector<cv::Point2f> rectanglePoints, float color);

	cv::vector<cv::Point2f>		manualMarkCheckBoard(cv::Mat img);
	float						markWhite(cv::Mat img);
	void						manualMarkCalibBoardCorners(cv::Mat img,cv::vector<cv::Point2f> &imgPoints_out, cv::vector<cv::Point2f> &objPoints_out);

	void						perspectiveTransformation(cv::vector<cv::Point2f> corners_in,cv::Mat homoMatrix, cv::vector<cv::Point3f> &points_out);
	void						undistortCameraImgPoints(cv::vector<cv::Point2f> points_in,cv::vector<cv::Point2f> &points_out);
	
	cv::vector<cv::vector<cv::Point2f>> imgBoardCornersCam;
	cv::vector<cv::vector<cv::Point3f>> objBoardCornersCam;
	
	//images
	cv::Vector<cv::Mat> calibImgs;
	cv::Mat				extrImg;

	cv::Size	squareSize;
	int			numOfCamImgs;

	cv::Size	camImageSize;

	bool		camCalibrated;

};

