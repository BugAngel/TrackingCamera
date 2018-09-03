#include "mythread.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QDebug>
#include "mainwindow.h"

mythread::mythread()
{

}

//顺时针旋转图像
void myRotateClockWise90(cv::Mat& img)
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

//逆时针旋转图像
void myRotateAntiClockWise90(cv::Mat& img)
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

/**
* @brief  用向量来做COSα=两向量之积/两向量模的乘积求两条线段夹角
* @param  线段3个点坐标pt1,pt2,pt0,最后一个参数为公共点
* @retval 线段夹角，单位为角度
* @author  BugAngel
* @attention
*/
double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    double angle_line = (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);//余弦值
    return acos(angle_line) * 180 / 3.141592653;
}

/**
* @brief  判断角度是否在范围内
* @param  angle为输入角度，rangeMin，rangeMax分别为范围的下限和上限
* @retval 在范围内返回true，不在返回false
* @author  BugAngel
* @attention
*/
bool angleRange(double angle,int rangeMin,int rangeMax)
{
    if(angle > rangeMin && angle < rangeMax)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void mythread::run()
{
    cv::VideoCapture capture(0);
    double minArea=1000;//最小面积
    double maxArea=120000;//最大面积
    unsigned int shape;//多边形边数
    cv::RNG g_rng(12345);
    cv::Mat grayImage;
    cv::Mat cannyMat_output;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    int thresh = 100;
    cv::Mat srcImage;
    cv::Mat dstImage;

    while(1)
    {
        if(capture.isOpened())
        {
            capture>>srcImage;
        }
        else
        {
            msleep(10); //延时，防止卡死
            continue;
        }


        try
        {
            cv::cvtColor(srcImage,grayImage,CV_RGB2GRAY);

            // 用Canny算子检测边缘
            cv::Canny( grayImage, cannyMat_output, thresh, thresh*2, 3 );

            // 寻找轮廓
            cv::findContours( cannyMat_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

            //多边形逼近轮廓
            std::vector<std::vector<cv::Point>> contours_poly(contours.size());

            for(unsigned int i=0;i<contours.size();i++)
            {
                cv::approxPolyDP(contours[i], contours_poly[i], 3, true);//多边形逼近
                double tempArea=cv::contourArea(contours_poly[i]);
                //如果是凸多边形并且面积在范围内,并且是凸的
                if((contours_poly[i].size() == shape &&  tempArea > minArea  && tempArea < maxArea &&  cv::isContourConvex(contours_poly[i])))
                {
                    if(shape==3)
                    {
                        double angel_1 = fabs(angle(contours_poly[i][0], contours_poly[i][1], contours_poly[i][2]));
                        double angel_2 = fabs(angle(contours_poly[i][0], contours_poly[i][2], contours_poly[i][1]));
                        double angel_3 = fabs(angle(contours_poly[i][1], contours_poly[i][2], contours_poly[i][0]));
                        if (angleRange(angel_1,50,70) && angleRange(angel_2,50,70) && angleRange(angel_3,50,70))//判断角度范围
                        {
                            cv::Scalar color = cv::Scalar( g_rng.uniform(0, 255), g_rng.uniform(0,255), g_rng.uniform(0,255) );//随机生成颜色值
                            cv::polylines(dstImage,contours_poly[i],true,color,5, CV_AA, 0);//画出多边形轮廓
                            x=(contours_poly[i][0].x+contours_poly[i][1].x+contours_poly[i][2].x)/(3*4);
                            y=(contours_poly[i][0].y+contours_poly[i][1].y+contours_poly[i][2].y)/(3*4);
                        }
                    }
                    else if(shape==4)
                    {
                        double angel_1 = fabs(angle(contours_poly[i][1], contours_poly[i][3], contours_poly[i][0]));
                        double angel_2 = fabs(angle(contours_poly[i][0], contours_poly[i][2], contours_poly[i][1]));
                        double angel_3 = fabs(angle(contours_poly[i][1], contours_poly[i][3], contours_poly[i][2]));
                        double angel_4 = fabs(angle(contours_poly[i][2], contours_poly[i][0], contours_poly[i][3]));
                        if (angleRange(angel_1,80,100)&&angleRange(angel_2,80,100)&&angleRange(angel_3,80,100)&&angleRange(angel_4,80,100))//判断角度范围
                        {
                            cv::Scalar color = cv::Scalar( g_rng.uniform(0, 255), g_rng.uniform(0,255), g_rng.uniform(0,255) );//随机生成颜色值
                            cv::polylines(dstImage,contours_poly[i],true,color,5, CV_AA, 0);//画出多边形轮廓
                            x=(contours_poly[i][0].x+contours_poly[i][1].x+contours_poly[i][2].x+contours_poly[i][3].x)/(4*4);
                            y=(contours_poly[i][0].y+contours_poly[i][1].y+contours_poly[i][2].y+contours_poly[i][3].y)/(4*4);
                        }
                    }
                }
            }
            cv::imshow("多边形检测",dstImage);

        }
        catch(std::exception& e){
            qDebug()<<e.what();
        }
        msleep(10); //延时，防止卡死
    }
}
