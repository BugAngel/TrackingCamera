/**
******************************************************************************
* @file    shape.cpp
* @author  WangRong
* @version V1.0
* @date    2017.7.26
* @note    opencv找三角形或正方形
******************************************************************************
*/

#include <cv.h>
#include <highgui.h>
#include <cstdio>
#include <cmath>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <vector>
#include <cmath>
using namespace cv;

void myRotateClockWise90(Mat& img)
{
	if(img.empty())
	{
		return;
	}
	//矩阵转置
	transpose(img,img);
	//0:X洲翻转  >0:Y轴翻转
	flip(img,img,1);
}

void myRotateAntiClockWise90(Mat& img)
{
	if(img.empty())
	{
		return;
	}
	//矩阵转置
	transpose(img,img);
	//0:X洲翻转  >0:Y轴翻转
	flip(img,img,0);
}

//////////////////////////////////////////////////////////////////
//函数功能：用向量来做COSα=两向量之积/两向量模的乘积求两条线段夹角
//输入：   线段3个点坐标pt1,pt2,pt0,最后一个参数为公共点
//输出：   线段夹角，单位为角度
//////////////////////////////////////////////////////////////////
double angle(CvPoint* pt1, CvPoint* pt2, CvPoint* pt0)
{
	double dx1 = pt1->x - pt0->x;
	double dy1 = pt1->y - pt0->y;
	double dx2 = pt2->x - pt0->x;
	double dy2 = pt2->y - pt0->y;
	double angle_line = (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);//余弦值
	return acos(angle_line) * 180 / 3.141592653;
}
//////////////////////////////////////////////////////////////////
//函数功能：采用多边形逼近检测，通过约束条件寻找多边形
//输入：   img 原图像
//          storage 存储
//          minarea，maxarea 检测多边形的最小/最大面积
//          minangle,maxangle 检测多边形边夹角范围，单位为角度  
//输出：   多边形序列
//////////////////////////////////////////////////////////////////
CvSeq* findSquares4(IplImage* img, CvMemStorage* storage, int minarea, int maxarea, int minangle, int maxangle,int shape)
{
	CvSeq* contours;//边缘
	int N = 6;  //阈值分级
	CvSize sz = cvSize(img->width & -2, img->height & -2);
	IplImage* timg = cvCloneImage(img);//拷贝一次img
	IplImage* gray = cvCreateImage(sz, 8, 1); //img灰度图
	IplImage* pyr = cvCreateImage(cvSize(sz.width / 2, sz.height / 2), 8, 3);  //金字塔滤波3通道图像中间变量
	IplImage* tgray = cvCreateImage(sz, 8, 1);;
	CvSeq* result;
	//double s[SHAPE];
	double t;
	double s;
	CvSeq* squares = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage);

	cvSetImageROI(timg, cvRect(0, 0, sz.width, sz.height));
	//金字塔滤波 
	cvPyrDown(timg, pyr, 7);
	cvPyrUp(pyr, timg, 7);

	//cvShowImage("滤波结果", timg);
	//在3个通道中寻找多边形 
	for (int c = 0; c < 3; c++) //对3个通道分别进行处理 
	{
		cvSetImageCOI(timg, c + 1);
		cvCopy(timg, tgray, 0);  //依次将BGR通道送入tgray         
		//for (int l = 0; l < N; l++)
		//{
			////不同阈值下二值化
			//cvThreshold(tgray, gray, (l + 1) * 255 / N, 255, CV_THRESH_BINARY);
		    cvThreshold(tgray, gray, 119, 255, CV_THRESH_BINARY);
			cvShowImage("二值化结果", gray);
			cvFindContours(gray, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
			while (contours)
			{ //多边形逼近             
				result = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0);
				//如果是凸多边形并且面积在范围内
				if (result->total == shape && fabs(cvContourArea(result, CV_WHOLE_SEQ)) > minarea  && fabs(cvContourArea(result, CV_WHOLE_SEQ)) < maxarea &&  cvCheckContourConvexity(result))
				{
					//memset(s, 0, sizeof(s));
					s = 0;
					//判断每一条边
					for (int i = 0; i < shape + 1; i++)
					{
						if (i >= 2)
						{   //角度            
							t = fabs(angle((CvPoint*)cvGetSeqElem(result, i), (CvPoint*)cvGetSeqElem(result, i - 2), (CvPoint*)cvGetSeqElem(result, i - 1)));
							s = s > t ? s : t;
							//s[i-2] = t;
						}
					}
					//这里的S为直角判定条件 单位为角度
					if (s > minangle && s < maxangle)
						////内角和判定
						//double sum=0.0;
						//for (int i = 0; i<SHAPE; i++)
						//{
						//	sum += s[i];
						//}
						//if ((sum>150 * (SHAPE - 2)) && (sum<210 * (SHAPE - 2)))
					for (int i = 0; i < shape; i++)
						cvSeqPush(squares, (CvPoint*)cvGetSeqElem(result, i));
				}
				contours = contours->h_next;
			}
		//}
	}
	cvReleaseImage(&gray);
	cvReleaseImage(&pyr);
	cvReleaseImage(&tgray);
	cvReleaseImage(&timg);
	return squares;
}
//////////////////////////////////////////////////////////////////
//函数功能：画出所有矩形
//输入：   img 原图像
//          squares 多边形序列
//          wndname 窗口名称
//          shape  图形形状
//输出：   图像中标记多边形
//注意：  CvPoint pt[5];  几边形这个数组大小就至少要为几，因为只要正方形和
//三角形，所以这里写的是5，如果其他用途这个就必须要更改
//////////////////////////////////////////////////////////////////
//int lenghtFlag=1;
//int count=0;
void drawSquares(IplImage* img, CvSeq* squares, const char* wndname, int shape)
{
	CvSeqReader reader;
	IplImage* cpy = cvCloneImage(img);
	CvPoint pt[5];  //注意这个数组大小需要在多边形的时候更改
	int i;
	cvStartReadSeq(squares, &reader, 0);
	for (i = 0; i < squares->total; i += shape)
	{
		CvPoint* rect = pt;
		int count = shape;
		for (int j = 0; j < count; j++)
		{
			memcpy(pt + j, reader.ptr, squares-> elem_size);
			CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
		}
		cvPolyLine(cpy, &rect, &count, 1, 1, CV_RGB( 255, 255, 255), 5, CV_AA, 0);//彩色绘制
		double center_x = 0.0, center_y = 0.0;
		if (shape == 4)
		{
			center_x = (pt[0].x + pt[1].x + pt[2].x + pt[3].x) / 4;
			center_y = (pt[0].y + pt[1].y + pt[2].y + pt[3].y) / 4;
		}
		else if (shape == 3)
		{
			center_x = (pt[0].x + pt[1].x + pt[2].x) / 3;
			center_y = (pt[0].y + pt[1].y + pt[2].y) / 3;
		}

		if (center_x - 0.0 > fabs(1.0e-6) && (center_y - 0.0 > fabs(1.0e-6)))
		{
			std::cout << ":" << center_x << "，" << center_y << "\r\n";
			CvPoint center;
			center.x = int(center_x);
			center.y = int(center_y);
			CvPoint temp;
			temp.x=240;
			temp.y=320;
			cvCircle(cpy, center, 3, CV_RGB( 255, 255,255), CV_FILLED);
			cvLine(cpy,center,temp,CV_RGB( 0, 255,0),3);

			FILE *fp;
			while((fp=fopen("E:\\test.txt","w"))==NULL);
			//640*480,除以四之后是160*120
			fprintf(fp,"%d\r\n%d\r\n",(int)center.x/4,(int)center.y/4);
			fclose(fp);

			//if(lenghtFlag)
			//{
			//	count++;
			//	//if(count>10)
			//	//{
			//	//	count=0;
			//		int length=(int)sqrt((double)((center.x/4-60)*(center.x/4-60))+(double)((center.y/4-80)*(center.y/4-80)));
			//		if(length<10) lenghtFlag=0;
			//		//出数据
			//		fp=fopen("E:\\data.txt","a+");
			//		fprintf(fp,"%d  %d  %d\r\n",(int)center.x/4,(int)center.y/4,length);//x坐标，y坐标，线段长度
			//		fclose(fp);
			//	//}
			//}
			/*Sleep(50);*/
;		}
		
	}
	cvShowImage(wndname, cpy);
	cvReleaseImage(&cpy);
}

int main()
{
	int shape = 3;//边数
	CvCapture* capture = cvCreateCameraCapture(0);
	IplImage* img0 = 0;
	CvMemStorage* storage = 0;
	int c;
	const char* wndname = "多边形检测"; //窗口名称
	storage = cvCreateMemStorage(0);
	cvNamedWindow(wndname, 1);
	while (true)
	{
		img0 = cvQueryFrame(capture);
		Mat M(img0,true);
		myRotateAntiClockWise90(M);//逆时针转90
		IplImage imgTmp=M;

		/**img0=cvCloneImage(&imgTmp);*/
		if (shape == 4)
		{
			drawSquares(&imgTmp, findSquares4(&imgTmp, storage, 1000, 120000, 70, 110, shape), wndname,shape);
		}

		else if (shape == 3)
		{
			drawSquares(&imgTmp, findSquares4(&imgTmp, storage, 1000, 120000, 40, 80, shape), wndname,shape);
		}

		cvClearMemStorage(storage);  //清空存储

		c = cvWaitKey(10);
		if (c == 27)
			break;
	}

	cvReleaseImage(&img0);
	//cvReleaseImage(&imgTmp);
	cvClearMemStorage(storage);
	cvDestroyWindow(wndname);

	return 0;
}