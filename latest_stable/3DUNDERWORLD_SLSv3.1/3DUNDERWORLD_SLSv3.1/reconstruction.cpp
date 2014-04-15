
#include "stdafx.h"
#include <math.h>
#include "cvaux.h"
#include "cv.h"
#include "highgui.h"
#include "cvwimage.h"
#include "cxcore.h"
#include "cxmisc.h"
#include "ml.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <conio.h>

#define COL_GRAY_OFFSET 0
#define ROW_GRAY_OFFSET 0
#define NUM_OF_CAM_IMGS 42
#define COL_IMGS 10
#define ROW_IMGS 10
#define PROJ_H 768
#define PROJ_W 1024
#define PROJ_CAM 60
//#define EXPORT_MASKED_POINTS
#define SCALE 1

IplImage** camImgs;
bool *mask;
int blackThreshold=0;
int whiteThreshold=0;
int colNum;
int rowNum;

CvMat* camMatrix;
CvMat* projMatrix;

CvMat* camDist;
CvMat* projDist;

CvMat* camRotMatrix;
CvMat*	projRotMatrix;

CvMat* camTranMatrix;
CvMat*	projTranMatrix;

CvMat* projRays;

CvScalar projCenter;

CvScalar fcCam, fcProj; //focal distance 
CvScalar ccCam, ccProj; //principal point

IplImage* colorImg;

int projW;

int projH;

double* projColPlanes;
double* projRowPlanes;

double* projColPlanesNormals;
double* projRowPlanesNormals;

int col_gray_offset;
int row_gray_offset;

double projRec[PROJ_W][PROJ_H][4]={0};

std::stringstream dirPath;

void normalize3d(CvScalar &vec)	{
	double mag = sqrt( vec.val[0]*vec.val[0] + vec.val[1]*vec.val[1] + vec.val[2]*vec.val[2]);
	
	vec.val[0] /= std::max(0.000001, mag);
	vec.val[1] /= std::max(0.000001, mag);
	vec.val[2] /= std::max(0.000001, mag);
	
	return;
}

void normalize3dtable(double vec[3])
{
	double mag = sqrt( vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
	
	vec[0] /= std::max(0.000001, mag);
	vec[1] /= std::max(0.000001, mag);
	vec[2] /= std::max(0.000001, mag);
}

//load camera images
void loadCamImages(int n)
{
	camImgs = new IplImage* [n];

	IplImage* tmp;

	std::cout<<"Loading Camera Images...";

	for(int i=0; i<n;i++)
	{
		std::stringstream path;

		if(i+1<10)
			path<<dirPath.str()<<"dataSet/("<<i+1<<").jpg";
		else
			path<<dirPath.str()<<"dataSet/("<<i+1<<").jpg";

		tmp=cvLoadImage(path.str().c_str() );
		camImgs[i]= cvCreateImage(cvSize(tmp->width, tmp->height), IPL_DEPTH_8U,  1);
		cvCvtColor(tmp, camImgs[i], CV_RGB2GRAY);
		cvReleaseImage(&tmp);

		if(!camImgs[i])
		{
			std::cout<<"Error loading cam image "<<i+1<<". Press Any Key to Exit.";
			getch();
			exit(-1);
		}

		if(i==0)
			colorImg = cvLoadImage(path.str().c_str());
	}

	std::cout<<"done!\n";
	
}

//find the shadows in the scene using white and black img
void findShadows()
{

	std::cout<<"Estimating Shadows...";

	int w=camImgs[0]->width;
	int h=camImgs[0]->height;

	mask = new bool [w*h];

	IplImage* maskImg        = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U,  1);

	for(int i=0;i<w;i++)
	{
		for(int j=0; j<h; j++)
		{
			double blackVal,whiteVal;
			
			CvScalar black = cvGet2D(camImgs[1],j,i);
			CvScalar white = cvGet2D(camImgs[0],j,i);

			blackVal= black.val[0];
			whiteVal= white.val[0];

			if(whiteVal-blackVal > blackThreshold)
			{
				mask[i+j*w]=true;
				cvSet2D(maskImg,j,i, cvScalar(255));
			}
			else
			{
				mask[i+j*w]=false;
				cvSet2D(maskImg,j,i, cvScalar(0));
			}
		}
	}

	std::stringstream path;

	path<<dirPath.str()<<"mask.png";

	cvSaveImage(path.str().c_str(),maskImg);
	std::cout<<"done!\n";
}

bool XOR(bool val1, bool val2)
{
	if(val1==val2)
		return 0;
	else
		return 1;
}

//convert a gray code sequence to a decimal number
int grayToDec(bool grayCol[], int size)
{
	int dec=0;
	bool tmp = grayCol[0];
	
	if(tmp)
		dec+=(int) pow((float)2,size-1);

	for(int i=1; i< size; i++)
	{
		tmp=XOR(tmp,grayCol[i]);
		if(tmp)
			dec+= (int) pow((float)2,size-i-1);
	}
	return dec;
}

//for a (x,y) pixel of the camera returns the corresponding projector pixel
CvScalar getProjPixelForCamPixel(int x, int y, bool *error)
{
	bool *grayCol = new bool[colNum];
	bool *grayRow = new bool[rowNum];

	CvScalar pixel;
	double val1, val2;
	int xDec,yDec;


	//prosses column images
	for(int count=0; count<colNum; count++)
	{

		pixel = cvGet2D(camImgs[count*2+2],y,x);
		val1=pixel.val[0];

		pixel = cvGet2D(camImgs[count*2+2+1],y,x);
		val2=pixel.val[0];
		
		if(abs(val1-val2)<whiteThreshold)
			*error=true;

		if(val1>val2)
			grayCol[count]=1;
		else
			grayCol[count]=0;

	}
	xDec=grayToDec(grayCol,colNum) - col_gray_offset;


	if(xDec>projW)
	{
		*error=true;
		std::cout<<"error on dec col "<<xDec<<" \n";		
	}

	//prosses row images
	for(int count=0; count<rowNum; count++)
	{

		pixel = cvGet2D(camImgs[count*2+2+colNum*2],y,x);
		val1=pixel.val[0];

		pixel = cvGet2D(camImgs[count*2+2+colNum*2+1],y,x);
		val2=pixel.val[0];
		
		if(abs(val1-val2)<whiteThreshold)
			*error=true;

		if(val1>val2)
			grayRow[count] = 1;
		else
			grayRow[count] = 0;

	}

	yDec = grayToDec(grayRow,rowNum) - row_gray_offset; //-128 is the datasets offset for rows pls remove

	if(yDec > projH)
	{
		*error=true;
		std::cout<<"error on dec row "<< yDec <<"\n";		
	}

	return cvScalar(xDec,yDec);
}


//undistorPoints modified cvUndistortPoints cos cvUndistortPoints not working!!!
void UndistortPoints( double xi, double yi, const CvMat* cameraMatrix, const CvMat* distCoeffs,double *xd,double *yd)
{

    double  k[5]={0,0,0,0,0}, fx, fy, ifx, ify, cx, cy;
  
    int iters = 1;

	k[0]=distCoeffs->data.fl[0];
	k[1]=distCoeffs->data.fl[1];
	k[2]=distCoeffs->data.fl[2];
	k[3]=distCoeffs->data.fl[3];
	k[4]=0;

    iters = 5;

    fx = cvGet2D(cameraMatrix,0,0).val[0];
    fy = cvGet2D(cameraMatrix,1,1).val[0];
	
    ifx = 1./fx;
    ify = 1./fy;
    cx = cvGet2D(cameraMatrix,0,2).val[0];
    cy = cvGet2D(cameraMatrix,1,2).val[0];


	double x, y, x0, y0;
	x=xi;
	y=yi;

	x0 = x = (x - cx)*ifx;
	y0 = y = (y - cy)*ify;
			
	for(int jj = 0; jj < iters; jj++ )
	{
		double r2 = x*x + y*y;
		double icdist = 1./(1 + ((k[4]*r2 + k[1])*r2 + k[0])*r2);
		double deltaX = 2*k[2]*x*y + k[3]*(r2 + 2*x*x);
		double deltaY = k[2]*(r2 + 2*y*y) + 2*k[3]*x*y;
		x = (x0 - deltaX)*icdist;
		y = (y0 - deltaY)*icdist;
	}
	


	*xd=(x*fx)+cx;
	*yd=(y*fy)+cy;
}

//convert a point from projector to camera space
void proj2cam(double p[3])
{
	
	CvMat *Tmp=cvCreateMat(3,1,CV_32FC1);
	CvMat *tmpPoint=cvCreateMat(3,1,CV_32FC1);

	tmpPoint->data.fl[0]=p[0];
	tmpPoint->data.fl[1]=p[1];
	tmpPoint->data.fl[2]=p[2];

	cvGEMM(projRotMatrix, projTranMatrix, -1, NULL, 0, Tmp,CV_GEMM_A_T); 
	cvGEMM(projRotMatrix, tmpPoint, 1, NULL, 0, tmpPoint,CV_GEMM_A_T);

	Tmp->data.fl[0]=cvGet1D(Tmp,0).val[0]+cvGet1D(tmpPoint,0).val[0];
	Tmp->data.fl[1]=cvGet1D(Tmp,1).val[0]+cvGet1D(tmpPoint,1).val[0];
	Tmp->data.fl[2]=cvGet1D(Tmp,2).val[0]+cvGet1D(tmpPoint,2).val[0];

	//cvGEMM(Rc, Tmp, 1, Tc, 1, Tmp, 0);

	p[0]=Tmp->data.fl[0];
	p[1]=Tmp->data.fl[1];
	p[2]=Tmp->data.fl[2];
	
	cvReleaseMat(&Tmp);
	cvReleaseMat(&tmpPoint);
	
}

//convert image pixel to image 3d space point
void PixelToImage(double p[3], CvScalar fc, CvScalar cc)
{

	p[0]=(p[0]-cc.val[0])/fc.val[0];
	p[1]=(p[1]-cc.val[1])/fc.val[1];
	p[2]=1;

}

void computeProjPlanesNormals()
{
	std::cout<<"Calculating projector's planes' normals..."; 
	double p1[3],p2[3],v1[3],v2[3],n[3];

	projColPlanesNormals = new double[projW*3]; 
	projRowPlanesNormals = new double[projH*3];
	
	//compute row planes 
	for(int i=0; i<projH; i++)
	{

		p1[0]=0;
		p1[1]=i;

		p2[0]=projW;
		p2[1]=i;

		double xUD,yUD;
		UndistortPoints(p1[0],p1[1],projMatrix,projDist,&xUD,&yUD);

		p1[0]=xUD;
		p1[1]=yUD;

		UndistortPoints(p2[0],p2[1],projMatrix,projDist,&xUD,&yUD);

		p2[0]=xUD;
		p2[1]=yUD;

		PixelToImage(p1,fcProj,ccProj);
		PixelToImage(p2,fcProj,ccProj);

		proj2cam(p1);
		proj2cam(p2);

		v1[0]=projCenter.val[0]-p1[0];
		v1[1]=projCenter.val[1]-p1[1];
		v1[2]=projCenter.val[2]-p1[2];

		normalize3dtable(v1);

		v2[0]=projCenter.val[0]-p2[0];
		v2[1]=projCenter.val[1]-p2[1];
		v2[2]=projCenter.val[2]-p2[2];

		normalize3dtable(v2);
		

		n[0]=v1[1]*v2[2] - v1[2]*v2[1];
		n[1]=v1[2]*v2[0] - v1[0]*v2[2];
		n[2]=v1[0]*v2[1] - v1[1]*v2[0];

		normalize3dtable(n);

		projRowPlanesNormals[0+i*3]=n[0];
		projRowPlanesNormals[1+i*3]=n[1];
		projRowPlanesNormals[2+i*3]=n[2];

	}

	//compute col planes
	for(int i=0; i<projW; i++)
	{

		p1[0]=i;
		p1[1]=0;

		p2[0]=i;
		p2[1]=projH;

		double xUD,yUD;
		UndistortPoints(p1[0],p1[1],projMatrix,projDist,&xUD,&yUD);

		p1[0]=xUD;
		p1[1]=yUD;

		UndistortPoints(p2[0],p2[1],projMatrix,projDist,&xUD,&yUD);

		p2[0]=xUD;
		p2[1]=yUD;

		PixelToImage(p1,fcProj,ccProj);
		PixelToImage(p2,fcProj,ccProj);

		proj2cam(p1);
		proj2cam(p2);

		v1[0]=projCenter.val[0]-p1[0];
		v1[1]=projCenter.val[1]-p1[1];
		v1[2]=projCenter.val[2]-p1[2];

		normalize3dtable(v1);

		v2[0]=projCenter.val[0]-p2[0];
		v2[1]=projCenter.val[1]-p2[1];
		v2[2]=projCenter.val[2]-p2[2];

		normalize3dtable(v2);

		n[0]=v1[1]*v2[2] - v1[2]*v2[1];
		n[1]=v1[2]*v2[0] - v1[0]*v2[2];
		n[2]=v1[0]*v2[1] - v1[1]*v2[0];

		normalize3dtable(n);

		projColPlanesNormals[0+i*3]=n[0];
		projColPlanesNormals[1+i*3]=n[1];
		projColPlanesNormals[2+i*3]=n[2];

	}

	std::cout<<"done!\n"; 
}



//calculate the intersection point of a ray and a plane, given the normal and a point of the plane, and a point and the vector of the ray
CvScalar planeRayInter(CvScalar planeNormal,CvScalar planePoint, CvScalar rayVector, CvScalar rayPoint )
{
	double l;
	CvScalar point;

	CvScalar pSub;
	pSub.val[0] = - rayPoint.val[0] + planePoint.val[0];
	pSub.val[1] = - rayPoint.val[1] + planePoint.val[1];
	pSub.val[2] = - rayPoint.val[2] + planePoint.val[2];

	//normalize3d(pSub);

	double dotProd1 = pSub.val[0] * planeNormal.val[0] + pSub.val[1] * planeNormal.val[1] + pSub.val[2] * planeNormal.val[2];
	double dotProd2 = rayVector.val[0] * planeNormal.val[0] + rayVector.val[1] * planeNormal.val[1] + rayVector.val[2] * planeNormal.val[2];

	double temp = std::max(0.000001,fabs(dotProd2));
	if (dotProd2 < 0.0)	temp = -temp;
	l=dotProd1/temp;

	point.val[0] = rayPoint.val[0] + l * rayVector.val[0]; 
	point.val[1] = rayPoint.val[1] + l * rayVector.val[1]; 
	point.val[2] = rayPoint.val[2] + l * rayVector.val[2]; 

	return point;
}



void reconstruction()
{
	
	int w=camImgs[0]->width;
	int h=camImgs[0]->height;

	std::ofstream out1; out1.open("reconstruction/output/3d_cloud.xyz");
	std::ofstream out2; out2.open("reconstruction/output/3d_cloudRows.xyz");
	std::ofstream out3; out3.open("reconstruction/output/3d_cloudCols.xyz");
	std::ofstream out4; out4.open("reconstruction/output/3d_cloudProj.xyz");

	IplImage* decRows = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U,  1);
	IplImage* decCols = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U,  1);
	IplImage* projToCamRays = cvCreateImage(cvSize(PROJ_W, PROJ_H), IPL_DEPTH_8U,  1);

	CvScalar projPixel;
	CvScalar projPlaneNorm;
	CvScalar camVector;
	CvScalar pCol;
	CvScalar pRow;

	CvScalar p;
	CvScalar color;

	int load=0;

	//reconstraction for every camera pixel
	
	for(int i=0; i<w; i++)
	{
		for(int j=0; j<h; j++)
		{
			//if the pixel is not shadow reconstruct
			if(mask[i+j*w])
			{

				if(load != (int) (((j+(float)i*h)/((float)w*h))*100))
				{
					load =  (int) (((j+(float)i*h)/((float)w*h))*100);
					system("cls");
					std::cout<<"Computing 3D Cloud "<<load<< "%";
				}

				//get pixel color
				color = cvGet2D(colorImg,j,i);

				bool error=false;
				//get the projector pixel for camera (i,j) pixel
				projPixel = getProjPixelForCamPixel(i,j,&error);

				if(error)
				{
					#ifdef EXPORT_MASKED_POINTS
					out1<<-1<<" "<<-1<<" "<< -1 << " " << -1 << " "<< -1 << " "<< -1 <<"\n";
					#endif
					continue;
				}

				//camera 3d point p for (i,j) pixel
				double xUD,yUD;
				UndistortPoints(i,j,projMatrix,projDist,&xUD,&yUD);

				double pCam[3];
				pCam[0]=xUD;
				pCam[1]=yUD;

				//convert camera pixel to image space
				PixelToImage(pCam,fcCam,ccCam);

				//compute camera vector 
				camVector.val[0]=-pCam[0];
				camVector.val[1]=-pCam[1];
				camVector.val[2]=-pCam[2];

				//ray - col plane intersection 
				projPlaneNorm.val[0] = projColPlanesNormals[ 0 + (int) projPixel.val[0]*3];
				projPlaneNorm.val[1] = projColPlanesNormals[ 1 + (int) projPixel.val[0]*3];
				projPlaneNorm.val[2] = projColPlanesNormals[ 2 + (int) projPixel.val[0]*3];

				pCol = planeRayInter(projPlaneNorm, projCenter, camVector, cvScalar(0,0));// LinePlaneInterPoint(cvScalar(0,0) ,camVector, projColPlanes, projPixel.val[0],4);
				
				//ray - row plane intersection
				projPlaneNorm.val[0] = projRowPlanesNormals[ 0 + (int) projPixel.val[1]*3];
				projPlaneNorm.val[1] = projRowPlanesNormals[ 1 + (int) projPixel.val[1]*3];
				projPlaneNorm.val[2] = projRowPlanesNormals[ 2 + (int) projPixel.val[1]*3];

				pRow = planeRayInter(projPlaneNorm, projCenter, camVector, cvScalar(0,0));//LinePlaneInterPoint(cvScalar(0,0) ,camVector, projRowPlanes, projPixel.val[1],4);
				
				//find the average point
				p.val[0]=(pCol.val[0]+pRow.val[0])/2;
				p.val[1]=(pCol.val[1]+pRow.val[1])/2;
				p.val[2]=(pCol.val[2]+pRow.val[2])/2;

				
				
				projRec[int (projPixel.val[0])][int (projPixel.val[1])][0]+=p.val[0];
				projRec[int (projPixel.val[0])][int (projPixel.val[1])][1]+=p.val[1];
				projRec[int (projPixel.val[0])][int (projPixel.val[1])][2]+=p.val[2];
				projRec[int (projPixel.val[0])][int (projPixel.val[1])][3]+=1;
				

				cvSet2D(decRows,j,i,cvScalar(projPixel.val[0]/projW*255));
				cvSet2D(decCols,j,i,cvScalar(projPixel.val[1]/projH*255));
				out1<<p.val[0]*SCALE<<" "<<p.val[1]*SCALE<<" "<< p.val[2]*SCALE<< " " <<color.val[2] << " "<< color.val[1] << " "<< color.val[0] <<"\n";//<< "\t proj: "<< projPixel.val[0] << " " <<projPixel.val[1] <<"\t cam: " <<i <<" " << j<<"\n"; 
				out2<<pRow.val[0]<<" "<<pRow.val[1]<<" "<< pRow.val[2] << " " <<color.val[2] << " "<< color.val[1] << " "<< color.val[0] <<"\n";
				out3<<pCol.val[0]<<" "<<pCol.val[1]<<" "<< pCol.val[2] << " " <<color.val[2] << " "<< color.val[1] << " "<< color.val[0] <<"\n";
			}
			#ifdef EXPORT_MASKED_POINTS
			else
			{
				out1<<-1<<" "<<-1<<" "<< -1 << " " << -1 << " "<< -1 << " "<< -1 <<"\n";
			}
			#endif

		}

	}
	system("cls");
	std::cout<<"Computing 3D Cloud  100%";

	double max=0;
	/*
	for(int i=0; i<PROJ_W; i++)
	{
		
		for(int j=0; j<PROJ_H; j++)
		{
			

			if(projRec[i][j][3]>=0.99 )//&& projRec[i][j][3]<= 3 )
			{
				projRec[i][j][0]/= projRec[i][j][3];
				projRec[i][j][1]/= projRec[i][j][3];
				projRec[i][j][2]/= projRec[i][j][3];
				out4<<projRec[i][j][0]<<" "<<projRec[i][j][1]<<" "<< projRec[i][j][2]<<"\n";
			}
			else
			{
				out4<<-1<<" "<<-1<<" "<< -1<<"\n";
			}
			
			//if(projRec[i][j][3]==0)
				//cvSet2D(projToCamRays,j,i,cvScalar(255));
			//else
				//cvSet2D(projToCamRays,j,i,cvScalar(0));
			
			cvSet2D(projToCamRays,j,i,cvScalar(projRec[i][j][3]/170*255));

			if(projRec[i][j][3]>max)
				max=projRec[i][j][3];

			
		}
	} 
	*/
	std::cout<<"\nMaximum number of intersections of camera rays with a projector ray: "<<max;

	cvSaveImage("reconstruction/decRow.png",decRows);
	cvSaveImage("reconstruction/projToCamRays.png",projToCamRays);
	cvSaveImage("reconstruction/decCol.png",decCols);
	out1.close();

	getch();
		
}


int loadMatrix(CvMat **matrix,int s1,int s2 ,std::string file){

	std:: ifstream in1; 
	in1.open(file.c_str());
	
	if(in1==NULL)
	{
		std::cout<<"Error loading file "<<file.c_str()<<"\n";
		return -1;

	}

	*matrix=cvCreateMat(s1,s2,CV_32FC1);

	for(int i=0; i<s1; i++)
		for(int j=0; j<s2; j++)
		{
			float val;
			in1>>val;
			CvScalar v;
			v.val[0]=val; 
			if(s2>1)
				cvSet2D(*matrix,i,j,v);
			else
				(*matrix)->data.fl[i]=val;
				//cvSet1D(matrix,i,v);
		}
	return 1;
}

//find camera's and projector's center
void findCam_ProjC()
{
	//camera center
	CvScalar camCenter;
	camCenter=cvScalar(0,0);
	

	CvMat *Tmp=cvCreateMat(3,1,CV_32FC1);
	CvMat *Tptmp=cvCreateMat(3,1,CV_32FC1);

	cvSet1D(Tptmp,0,cvScalar(-cvGet1D(projTranMatrix,0).val[0]));
	cvSet1D(Tptmp,1,cvScalar(-cvGet1D(projTranMatrix,1).val[0]));
	cvSet1D(Tptmp,2,cvScalar(-cvGet1D(projTranMatrix,2).val[0]));

	cvSolve(projRotMatrix,Tptmp,Tmp);

	//cvGEMM(Rc, Tmp, 1, Tc, 1, Tmp, 0);
	
	//projector center
	projCenter.val[0]= cvGet1D(Tmp,0).val[0];
	projCenter.val[1]= cvGet1D(Tmp,1).val[0];
	projCenter.val[2]= cvGet1D(Tmp,2).val[0];

	cvReleaseMat(&Tmp);
	cvReleaseMat(&Tptmp);
	
}

void loadParams()
{

	//loading files
	
	//rotation matrix tou projector
	loadMatrix(&projRotMatrix,3,3,"reconstruction/input/proj_rotation_matrix.txt");
	//translation vector tou projector
	loadMatrix(&projTranMatrix,3,1,"reconstruction/input/proj_trans_vectror.txt");
	//translation vector tis cameras
	loadMatrix(&camTranMatrix,3,1,"reconstruction/input/cam_trans_vectror.txt");
	//rotation matrix tis cameras
	loadMatrix(&camRotMatrix,3,3,"reconstruction/input/cam_rotation_matrix.txt");
	//camera matrix
	loadMatrix(&camMatrix,3,3,"reconstruction/input/cam_matrix.txt");
	//projector matrix
	loadMatrix(&projMatrix,3,3,"reconstruction/input/proj_matrix.txt");
	//distortion tis cameras
	loadMatrix(&camDist,5,1,"reconstruction/input/cam_distortion.txt");
	//distortion tou projector
	loadMatrix(&projDist,5,1,"reconstruction/input/proj_distortion.txt"); 

	ccCam.val[0]=cvGet2D(camMatrix,0,2).val[0];
	ccCam.val[1]=cvGet2D(camMatrix,1,2).val[0];

	fcCam.val[0]=cvGet2D(camMatrix,0,0).val[0];
	fcCam.val[1]=cvGet2D(camMatrix,1,1).val[0];

	ccProj.val[0]=cvGet2D(projMatrix,0,2).val[0];
	ccProj.val[1]=cvGet2D(projMatrix,1,2).val[0];

	fcProj.val[0]=cvGet2D(projMatrix,0,0).val[0];
	fcProj.val[1]=cvGet2D(projMatrix,1,1).val[0];
}



int reconMain()
{
	dirPath<<"reconstruction/";

	int numOfCamImgs=NUM_OF_CAM_IMGS;

	col_gray_offset=COL_GRAY_OFFSET;
	row_gray_offset=ROW_GRAY_OFFSET;

	colNum=COL_IMGS;
	rowNum=ROW_IMGS;

	projH=PROJ_H;
	projW=PROJ_W;

	std::cout<<"Set Black Threshold (0-255): ";
	std::cin>>blackThreshold;

	std::cout<<"Set White Threshold (0-255): ";
	std::cin>>whiteThreshold;
	
	loadParams();

	findCam_ProjC();

	loadCamImages(numOfCamImgs);
	
	findShadows();
	
	computeProjPlanesNormals();
	

	reconstruction();
	
	return 0;
}

