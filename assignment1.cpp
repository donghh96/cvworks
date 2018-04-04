#include <stdio.h>
//#include <chrono>
#include <ctime>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;
//using namespace chrono;

Mat image_source, image_filter_out, image_blob_out;

void bubbleSort(int arr[], int n)
{
	for(int i = 0;i < n;i++){
		for(int j = 0;j < n-i-1;j++){
            if(arr[j] > arr[j+1]){
                int t = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = t;
            }
        }
    }
}

int blobCounter(const Mat& image_in, Mat& image_out, vector < set<  pair< int, int> > > SET)
{
    set< pair < int, int> > tmp;
    //1: declare a vector of sets SET[ ]
    //2: declare integers counter = -1, s1, s2
    int counter = -1, s1, s2;
    //3: declare A[i:width][i:height] {a 2D vector or mat initialised to -1}
    //int A[image_in.rows][image_in.cols];
    int width = image_in.cols;
    int height = image_in.rows;

    int A[width][height];
    //initialize
    for(int i=0; i<width; i++) {
        for(int j=0; j<height; j++) {
            A[i][j] = -1;
        }
    }
    //4:for y = 1 to i:height do
    for(int y=1; y<height; y++)
    {
        //5: for x = 1 to i:width do
        for(int x=1; x<width; x++)
        {

            //cout << "y="<< y << "x=" << x<< endl;
            //6: if ( i(x; y) != 0 ) (white) then
            if((image_in.at<uchar>(y, x)) > 48)
            {
                //7: if (i(x - 1; y) != 0 (left) OR i(x; y - 1) != 0)(up) then
                if((image_in.at<uchar>(y, x-1)) > 48 || (image_in.at<uchar>(y-1, x))>48)
                {
                    //8: s1 = A[x - 1][y]
                    s1=A[x][y-1];
                    //9: s2 = A[x][y - 1]
                    s2=A[x-1][y];
                    //10: if (s1 != -1) then
                    if(s1!=-1)
                    {
                        //11: i(x; y) ! SET[s1] {insert point i(x; y) into SET[s1]}
                        SET[s1].insert(make_pair(x,y));
                        //12: A[x][y] = s1
                        A[x][y]=s1;
                    }
                    //14: if (s2 != -1) then
                    //15: i(x; y) ! SET[s2]
                    //16: A[x][y] = s2
                    if(s2!=-1)
                    {
                        SET[s2].insert(make_pair(x,y));
                        A[x][y]=s2;
                    }
                    //18: if ((s1 != s2) AND (s1 != -1) AND (s2 != -1)) then
                    //19: SET[s1] U SET[s2] {Union, keep set SET[s1] and empty the other}
                    //20: Reset all points of A(x; y) belonging to SET[s2] to s1
                    //21: empty SET[s2]
                    //22: end if
                    if(s1!=s2 && s1!=-1 && s2!=-1)
                    {
                        //cout<<"s1 is "<<s1<<" s2 is "<<s2<<endl;
                        set_union(SET[s1].begin(), SET[s1].end(), SET[s2].begin(), SET[s2].end(), inserter(tmp, tmp.begin()));
                        SET[s1]=tmp;
                        for(set< pair<int,int> >::iterator it=SET[s2].begin(); it!=SET[s2].end(); it++)
                        {
                            A[(*it).first][(*it).second] = s1;

                        }

                        SET[s2].clear();
                        tmp.clear();
                    }

                }
                //23: else
                //24: counter = counter + 1
                //25: Create new set SET[counter]
                //26: i(x; y) ! SET[counter]
                //27: A[x][y] = counter
                else
                {
                    counter++;
                    //cout<<"counter is:"<<counter<<endl;
                    tmp.insert(make_pair(x,y));
                    SET.push_back(tmp);
                    A[x][y]=counter;
                    tmp.clear();
                }
            }
        }
    }

    image_out.create(image_in.size(), CV_8UC3);
    int x1, y1;
    int objects=0;

    for( int l=0; l<SET.size(); l++)
    {
        if (!SET[l].empty() && SET[l].size()>40)
        {
            int b=rand()%256;
            int g=rand()%256;
            int r=rand()%256;
            objects++;
            for(set< pair<int,int> >::iterator it=SET[l].begin(); it!=SET[l].end(); it++)
            {
                x1 = (*it).first;
                y1 = (*it).second;
                Vec3b pixel=image_out.at<Vec3b>(y1, x1);
                pixel[0] = b;
                pixel[1] = r;
                pixel[2] = g;
                image_out.at<Vec3b>(y1, x1) = pixel;
            }
        }
    }

    return objects;
}

void medianFilter(const Mat& image_in, Mat& image_out, int masksize)
{

    image_out.create(image_in.size(), CV_8UC1);

    int mask[masksize*masksize];

    for(int y=1; y<image_in.rows; y++)
    {
        for(int x=1; x<image_in.cols; x++)
        {
            for(int i=0; i<masksize; i++)
            {
                for(int j=0; j<masksize; j++)
                {
                    mask[i*masksize+j] = image_in.at<uchar>(y+j-2, x+i-2);
                }
            }

            bubbleSort(mask, masksize*masksize);
            image_out.at<uchar>(y,x) = mask[masksize*masksize/2];
        }
    }
}



//declare a vector of sets SET[]
vector < set<  pair< int, int> > > SET;

// Dynamic, the final counts shows on the screen. Don't print images.



int main(int argc, char** argv)
{

// ---------------  Video Capture Code Start  -------


// ---------------  Video Capture Code End    -------

    // Check parameter

    image_source = imread(argv[1], 0);

    namedWindow("image_source", 1);
    namedWindow("image_filter_out", 1);
    namedWindow("image_blob_out", 1);

    medianFilter(image_source, image_filter_out, 5);
//    medianFilter(image_filter_out, image_filter_out, 3);

    int objects = blobCounter(image_filter_out, image_blob_out, SET);
    cout<<"There are " << objects << " objects in this image" << endl;

    stringstream ss;
    ss << objects;
    string objects_string = ss.str();
    putText(image_blob_out, objects_string, cvPoint(30, 50), FONT_HERSHEY_PLAIN, 4, Scalar(0,255,255), 2, 8, 0);


    imshow("image_source", image_source);
    imshow("image_filter_out", image_filter_out);
    imshow("image_blob_out", image_blob_out);

    waitKey(0);
    exit(0);
}
