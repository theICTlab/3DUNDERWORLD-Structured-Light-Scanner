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

#include "PointCloudImage.h"

class MeshCreator
{

public:

	MeshCreator(PointCloudImage *in);
	~MeshCreator(void);
	void exportObjMesh(std::string path);
	void exportPlyMesh(std::string path);

private:

	int *pixelNum;
	PointCloudImage *cloud;
	int MeshCreator::access(int i,int j);
	int MeshCreator::access(int i,int j, int z);
	int w;
	int h;

};

