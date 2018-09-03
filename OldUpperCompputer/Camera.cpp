/**
******************************************************************************
* @file    shape.cpp
* @author  WangRong
* @version V1.0
* @date    2017.7.26
* @note    opencv�������λ�������
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
	//����ת��
	transpose(img,img);
	//0:X�޷�ת  >0:Y�ᷭת
	flip(img,img,1);
}

void myRotateAntiClockWise90(Mat& img)
{
	if(img.empty())
	{
		return;
	}
	//����ת��
	transpose(img,img);
	//0:X�޷�ת  >0:Y�ᷭת
	flip(img,img,0);
}

//////////////////////////////////////////////////////////////////
//�������ܣ�����������COS��=������֮��/������ģ�ĳ˻��������߶μн�
//���룺   �߶�3��������pt1,pt2,pt0,���һ������Ϊ������
//�����   �߶μнǣ���λΪ�Ƕ�
//////////////////////////////////////////////////////////////////
double angle(CvPoint* pt1, CvPoint* pt2, CvPoint* pt0)
{
	double dx1 = pt1->x - pt0->x;
	double dy1 = pt1->y - pt0->y;
	double dx2 = pt2->x - pt0->x;
	double dy2 = pt2->y - pt0->y;
	double angle_line = (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);//����ֵ
	return acos(angle_line) * 180 / 3.141592653;
}
//////////////////////////////////////////////////////////////////
//�������ܣ����ö���αƽ���⣬ͨ��Լ������Ѱ�Ҷ����
//���룺   img ԭͼ��
//          storage �洢
//          minarea��maxarea ������ε���С/������
//          minangle,maxangle ������α߼нǷ�Χ����λΪ�Ƕ�  
//�����   ���������
//////////////////////////////////////////////////////////////////
CvSeq* findSquares4(IplImage* img, CvMemStorage* storage, int minarea, int maxarea, int minangle, int maxangle,int shape)
{
	CvSeq* contours;//��Ե
	int N = 6;  //��ֵ�ּ�
	CvSize sz = cvSize(img->width & -2, img->height & -2);
	IplImage* timg = cvCloneImage(img);//����һ��img
	IplImage* gray = cvCreateImage(sz, 8, 1); //img�Ҷ�ͼ
	IplImage* pyr = cvCreateImage(cvSize(sz.width / 2, sz.height / 2), 8, 3);  //�������˲�3ͨ��ͼ���м����
	IplImage* tgray = cvCreateImage(sz, 8, 1);;
	CvSeq* result;
	//double s[SHAPE];
	double t;
	double s;
	CvSeq* squares = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage);

	cvSetImageROI(timg, cvRect(0, 0, sz.width, sz.height));
	//�������˲� 
	cvPyrDown(timg, pyr, 7);
	cvPyrUp(pyr, timg, 7);

	//cvShowImage("�˲����", timg);
	//��3��ͨ����Ѱ�Ҷ���� 
	for (int c = 0; c < 3; c++) //��3��ͨ���ֱ���д��� 
	{
		cvSetImageCOI(timg, c + 1);
		cvCopy(timg, tgray, 0);  //���ν�BGRͨ������tgray         
		//for (int l = 0; l < N; l++)
		//{
			////��ͬ��ֵ�¶�ֵ��
			//cvThreshold(tgray, gray, (l + 1) * 255 / N, 255, CV_THRESH_BINARY);
		    cvThreshold(tgray, gray, 119, 255, CV_THRESH_BINARY);
			cvShowImage("��ֵ�����", gray);
			cvFindContours(gray, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
			while (contours)
			{ //����αƽ�             
				result = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0);
				//�����͹����β�������ڷ�Χ��
				if (result->total == shape && fabs(cvContourArea(result, CV_WHOLE_SEQ)) > minarea  && fabs(cvContourArea(result, CV_WHOLE_SEQ)) < maxarea &&  cvCheckContourConvexity(result))
				{
					//memset(s, 0, sizeof(s));
					s = 0;
					//�ж�ÿһ����
					for (int i = 0; i < shape + 1; i++)
					{
						if (i >= 2)
						{   //�Ƕ�            
							t = fabs(angle((CvPoint*)cvGetSeqElem(result, i), (CvPoint*)cvGetSeqElem(result, i - 2), (CvPoint*)cvGetSeqElem(result, i - 1)));
							s = s > t ? s : t;
							//s[i-2] = t;
						}
					}
					//�����SΪֱ���ж����� ��λΪ�Ƕ�
					if (s > minangle && s < maxangle)
						////�ڽǺ��ж�
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
//�������ܣ��������о���
//���룺   img ԭͼ��
//          squares ���������
//          wndname ��������
//          shape  ͼ����״
//�����   ͼ���б�Ƕ����
//ע�⣺  CvPoint pt[5];  ��������������С������ҪΪ������ΪֻҪ�����κ�
//�����Σ���������д����5�����������;����ͱ���Ҫ����
//////////////////////////////////////////////////////////////////
//int lenghtFlag=1;
//int count=0;
void drawSquares(IplImage* img, CvSeq* squares, const char* wndname, int shape)
{
	CvSeqReader reader;
	IplImage* cpy = cvCloneImage(img);
	CvPoint pt[5];  //ע����������С��Ҫ�ڶ���ε�ʱ�����
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
		cvPolyLine(cpy, &rect, &count, 1, 1, CV_RGB( 255, 255, 255), 5, CV_AA, 0);//��ɫ����
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
			std::cout << ":" << center_x << "��" << center_y << "\r\n";
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
			//640*480,������֮����160*120
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
			//		//������
			//		fp=fopen("E:\\data.txt","a+");
			//		fprintf(fp,"%d  %d  %d\r\n",(int)center.x/4,(int)center.y/4,length);//x���꣬y���꣬�߶γ���
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
	int shape = 3;//����
	CvCapture* capture = cvCreateCameraCapture(0);
	IplImage* img0 = 0;
	CvMemStorage* storage = 0;
	int c;
	const char* wndname = "����μ��"; //��������
	storage = cvCreateMemStorage(0);
	cvNamedWindow(wndname, 1);
	while (true)
	{
		img0 = cvQueryFrame(capture);
		Mat M(img0,true);
		myRotateAntiClockWise90(M);//��ʱ��ת90
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

		cvClearMemStorage(storage);  //��մ洢

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