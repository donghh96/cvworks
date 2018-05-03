//#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "decoding.h"

using namespace cv;
using namespace std;


#define Mpixel(image, x, y)((uchar*)(((image).data) + (y)*((image).step)))[(x)]
#define MpixelB(image, x, y)((uchar*)(((image).data) + (y)*((image).step)))[(x)*((image).channels())]
#define MpixelG(image, x, y)((uchar*)(((image).data) + (y)*((image).step)))[(x)*((image).channels())+1]
#define MpixelR(image, x, y)((uchar*)(((image).data) + (y)*((image).step)))[(x)*((image).channels())+2]

#define StandardLegLength 824


string readBarcode(const Mat& image) {

    vector <vector <int> > bitsArray;

    vector <int> tmpBit;
    bool firstBlock = true;
    int totalBlocks = 0;


    for (int i=40; i<=960; i=i+20) {
        for ( int j=40; j<=960; j=j+20) {

            if ( (i <= 150 && j <= 150) || (i > 850 && j <= 150) || (i > 850 && j > 850)) {
                continue;
            } else {

                if (firstBlock) {

                    tmpBit.clear();

                    if (MpixelR(image, j, i) > 200) {
                        tmpBit.push_back(1);
                    } else {
                        tmpBit.push_back(0);
                    }

                    if (MpixelG(image, j, i) > 200) {
                        tmpBit.push_back(1);
                    } else {
                        tmpBit.push_back(0);
                    }

                    if (MpixelB(image, j, i) > 200) {
                        tmpBit.push_back(1);
                    } else {
                        tmpBit.push_back(0);
                    }

                    firstBlock = false;
                } else {
                    if (MpixelR(image, j, i) > 200) {
                        tmpBit.push_back(1);
                    } else {
                        tmpBit.push_back(0);
                    }

                    if (MpixelG(image, j, i) > 200) {
                        tmpBit.push_back(1);
                    } else {
                        tmpBit.push_back(0);
                    }

                    if (MpixelB(image, j, i) > 200) {
                        tmpBit.push_back(1);
                    } else {
                        tmpBit.push_back(0);
                    }

                    bitsArray.push_back(tmpBit);

                    firstBlock = true;

                    totalBlocks++;
                }
            }
        }
    }

    cout << "total blocks is: " << totalBlocks << endl;

    stringstream ss;

    for ( int i = 0; i < bitsArray.size(); i++) {
        int value = 32 * bitsArray[i][0] + 16 * bitsArray[i][1] + 8 * bitsArray[i][2] + 4 * bitsArray[i][3] + 2 * bitsArray[i][4] + bitsArray[i][5];

        ss << encodingarray[value];
    }

    string output = ss.str();

    return output;
}


float rotateAngle(Mat& image, Point &pLeftTop, Point &pLeftBottom, Point &pRightBottom, float &legLength) {

    Mat image_grey;
    cvtColor(image, image_grey, CV_BGR2GRAY);
    vector<Vec3f> circles;
    HoughCircles(image_grey, circles, CV_HOUGH_GRADIENT, 1, 100, 100, 30, 1, 30);

    for (size_t i=0; i<circles.size(); i++) {
        Vec3i c = circles[i];
        circle(image, Point(c[0], c[1]), c[2], Scalar(0, 0, 255), 3, 1);
        cout << "c[0]: " << c[0] << "  c[1]: " << c[1] << "  c[2]: " << c[2] << endl;
    }

    // calculate the length, find the two points
    float length01 = sqrt( pow((circles[0][0] - circles[1][0]), 2) + pow((circles[0][1] - circles[1][1]), 2));
    float length02 = sqrt( pow((circles[0][0] - circles[2][0]), 2) + pow((circles[0][1] - circles[2][1]), 2));

    cout << "length01 " << length01 << endl;
    cout << "length02 " << length02 << endl;

    if (abs(length01 - length02) < 5 ) {
        cout << "3" << endl;
        pLeftTop = Point(circles[1][0], circles[1][1]);
        pRightBottom = Point(circles[2][0], circles[2][1]);
        pLeftBottom = Point(circles[0][0], circles[0][1]);

        legLength = length01;
    } else if (length01 > length02) {
        cout << "1" << endl;
        pLeftTop = Point(circles[0][0], circles[0][1]);
        pRightBottom = Point(circles[1][0], circles[1][1]);
        pLeftBottom = Point(circles[2][0], circles[2][1]);

        legLength = length02;
    } else if (length01 < length02) {
        cout << "2" << endl;
        pLeftTop = Point(circles[0][0], circles[0][1]);
        pRightBottom = Point(circles[2][0], circles[2][1]);
        pLeftBottom = Point(circles[1][0], circles[1][1]);

        legLength = length01;
    }


    // rotate pLeftTop-pLeftBottom 90 degree, got another point and check if it is pRightBottom

    int x = pLeftTop.x - pLeftBottom.x;
    int y = pLeftTop.y - pLeftBottom.y;
    Point p = Point((pLeftBottom.x - y), (x + pLeftBottom.y));

    // if pRightBottom and p are not the same point
    if(abs(pRightBottom.x-p.x)>5 || abs(pRightBottom.y-p.y)>5) {

        Point tmp = pLeftTop;
        pLeftTop = pRightBottom;
        pRightBottom = tmp;
    }


    // We need to cauculate the angle between vector (P1 P2), and vector (1, 1)
    float theta = atan2((pRightBottom.y - pLeftTop.y), (pRightBottom.x - pLeftTop.x)) - atan2(1,1);

    theta = theta * 180 / CV_PI;

    cout << "theta is: " << theta << endl;

    return theta;
}


int main(int argc, char** argv) {

    Mat image, image_rotated, image_final;

    if(argc != 2) {
        cout << "The program takes 1 argument" << endl;
        cout << argv[0] << " image_file_path" << endl;
        exit(0);
    }

    image = imread(argv[1], 1);

    if (image.empty())
    {
        cout << "Load image file failed." << endl;
        exit(0);
    }

    image_rotated.create(image.size(), CV_8UC3);
    image_final.create(1000, 1000, CV_8UC3);

    // Rotate and Sale
    Point pLeftTop, pLeftBottom, pRightBottom;
    float legLength;
    float theta;
    int rotateAndScale = 0;

    // Calculate the angle that the image needs to rotate
    theta = rotateAngle(image, pLeftTop, pLeftBottom, pRightBottom, legLength);

    // The center of
    Point center = Point((pLeftTop.x + pRightBottom.x) / 2, (pLeftTop.y + pRightBottom.y) / 2);

    if( abs(theta) < 1 ) {
        cout << "No need to rotate" << endl;
        image.copyTo(image_rotated);
    } else {

        float scale;
        if( abs (legLength - StandardLegLength) > 6) {
            scale = StandardLegLength / legLength;
        } else {
            scale = 1;
            cout << "no need to scale" << endl;
        }

        cout << "scale is: " << scale <<endl;
        // calculate the rotate matrix
        Mat rotateMatrix = getRotationMatrix2D(center, theta, scale);

        // rotate it
        warpAffine(image, image_rotated, rotateMatrix, image.size());
        imshow("image_rotated", image_rotated);

        rotateAndScale = 1;
    }

    image_final.create(1000, 1000, CV_8UC3);

    if(rotateAndScale == 1) {
        if(image_rotated.cols > 1000) {

            int x0 = center.x - 499;
            int y0 = center.y - 499;
            for (int i=0; i<1000; i++) {
                for (int j=0; j<1000; j++) {
                    MpixelB(image_final, j, i) = MpixelB(image_rotated, y0 + j, x0 + i);
                    MpixelG(image_final, j, i) = MpixelG(image_rotated, y0 + j, x0 + i);
                    MpixelR(image_final, j, i) = MpixelR(image_rotated, y0 + j, x0 + i);
                }
            }
        }
    } else {
        if (image_rotated.cols != 1000) {
            resize(image, image_final, image_final.size(), INTER_LINEAR);
        } else {
            image_rotated.copyTo(image_final);
        }
    }

    string output = readBarcode(image_final);
    cout << "This color 2D barcode says" << endl;
    cout << output << endl;

    imshow("image", image);
    imshow("image_final", image_final);

    waitKey(0);
    exit(0);
}
