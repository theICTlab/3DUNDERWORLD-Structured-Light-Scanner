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

#include "cv.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

class VirtualCamera
{

public:

	VirtualCamera(void);
	~VirtualCamera(void);


	void loadDistortion(std::string path);
	void loadCameraMatrix(std::string path);
	void loadRotationMatrix(std::string path);
	void loadTranslationVector(std::string path);
	void computeProjectionMatrix();

	
	cv::Mat distortion;
	cv::Mat rotationMatrix;
	cv::Mat translationVector;
	cv::Mat projectionMatrix;
	cv::Mat cameraMatrix;

	cv::Point3f position;

	cv::Point2f fc; 
	cv::Point2f cc; 

	int width;
	int height;

private:

	int loadMatrix(cv::Mat &matrix,int s1,int s2 ,std::string file);

};

