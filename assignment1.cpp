#include <stdio.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <set>
#include <vector>

#define THRESHOLD 38
#define IGNORE 50
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480

using namespace cv;
using namespace std;
using namespace chrono;

// ----------------------------------------------------------------------------------
// 159.731 - Computer Vision
// Assignment1
// Name: Dong Huihui
// ID: 17383655
// ----------------------------------------------------------------------------------
// compile with
// g++ -std=c++11 assignment1.cpp -o assignment1 `pkg-config --cflags --libs opencv`
// ----------------------------------------------------------------------------------

Mat image_source, image_filter_out, image_blob_out, frame, frame_grey;
//declare a vector of sets SET[]
vector < set<  pair< int, int> > > SET;

int object_counts;

int dynamic = 0;

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

// Calculate threshold
int calThreshold(const Mat& image_in)
{

    if (dynamic == 1)
    {
        // histogram
        int histogram[256] = {0};
        for (int y=0; y<image_in.cols; y++)
        {
            for (int x=0; x<image_in.rows; x++)
            {
                histogram[image_in.at<uchar>(y, x)]++;
            }
        }

//        for(int i=0; i<256; i++)
//        {
//            cout << "histogram[" << i << "] is: " << histogram[i] << endl;
//        }

        // total pixeles
        int pixel_total = image_in.cols * image_in.rows;

        // the calculation
        int sumB = 0;
        int sum1 = 0;
        float wB = 0.0f;
        float wF = 0.0f;
        float mF = 0.0f;
        float max_var = 0.0f;
        float inter_var = 0.0f;
        int threshold = 0;
        int index = 0;

        for (index=0; index<256; index++)
        {
            sum1 += index * histogram[index];
        }

        for (index=0; index<256;index++)
        {
            wB = wB + histogram[index];
            wF = pixel_total - wB;
            if( wB == 0 || wF == 0)
            {
                continue;
            }
            sumB = sumB + index * histogram[index];
            mF = (sum1 - sumB) / wF;
            inter_var = wB * wF * ((sumB / wB) - mF) * ((sumB / wB) - mF);

            if (inter_var >= max_var)
            {
                threshold = index;
                max_var = inter_var;
            }
        }

        return threshold;

    }
    else
    {
        return THRESHOLD;
    }
}


//
//int blobCounter(const Mat& image_in, Mat& image_out, vector < set<  pair< int, int> > > SET)
int blobCounter(const Mat& image_in)
{
    SET.clear();

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

    int thre = calThreshold(image_in);

    //4:for y = 1 to i:height do
    for(int y=1; y<height; y++)
    {
        //5: for x = 1 to i:width do
        for(int x=1; x<width; x++)
        {

            //cout << "y="<< y << "x=" << x<< endl;
            //6: if ( i(x; y) != 0 ) (white) then

            if((image_in.at<uchar>(y, x)) > thre)
//            if((image_in.at<uchar>(y, x)) != 0)
            {
                //7: if (i(x - 1; y) != 0 (left) OR i(x; y - 1) != 0)(up) then
                if((image_in.at<uchar>(y, x-1)) > thre || (image_in.at<uchar>(y-1, x)) > thre)
//                if((image_in.at<uchar>(y, x-1)) != 0 || (image_in.at<uchar>(y-1, x)) != 0)
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

    int x1, y1;
    int objects=0;

    for( int l=0; l<SET.size(); l++)
    {
        if (!SET[l].empty() && SET[l].size()>IGNORE)
        {
            objects++;
        }
    }

    return objects;
}


// Color objects in SETs
void colorObjects(Mat& image_out)
{
//    image_out.create(image_in.size(), CV_8UC3);
    int x1, y1;

    for( int l=0; l<SET.size(); l++)
    {
        if (!SET[l].empty() && SET[l].size()>IGNORE)
        {
            int b=rand()%256;
            int g=rand()%256;
            int r=rand()%256;
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
}

// Input - greyscale, Output - greyscale
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


int main(int argc, char** argv)
{

// Check arguments
    if (argc==1)
    {
        dynamic = 1;

        //Open camera
        // ---------------  Video Capture Code Start  -------
        VideoCapture cap;
        cap.open(0);
        if (!cap.isOpened())
        {
            cout << "Failed to open camera" << endl;
            return 0;
        }
        cout << "Opened camera" << endl;
        namedWindow("WebCam", 1);
        cap.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
        cap >> frame;
        printf("frame size %d %d \n",frame.rows, frame.cols);
        int key=0;

        double fps=0.0;
        while (1){
            system_clock::time_point start = system_clock::now();

            cap >> frame;
            if( frame.empty() )
            break;

            char printit[100];
            sprintf(printit,"%2.1f",fps);
            putText(image_filter_out, printit, cvPoint(30, 50), FONT_HERSHEY_PLAIN, 2, cvScalar(255,255,255), 2, 8);

            // Count starts
            cvtColor(frame, frame_grey, CV_BGR2GRAY);
            medianFilter(frame_grey, image_filter_out, 3);
            object_counts = blobCounter(image_filter_out);
            stringstream ss;
            ss << object_counts;
            string objects_string = ss.str();
            putText(image_filter_out, objects_string, cvPoint(30, 50), FONT_HERSHEY_PLAIN, 4, Scalar(255,255,255), 2, 8, 0);
            // Count ends

            //imshow("WebCam", frame);
            imshow("WebCam", image_filter_out);
            key=waitKey(1);
            if(key==113 || key==27) return 0;//either esc or 'q'

            system_clock::time_point end = system_clock::now();
            double seconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            //fps = 1000000*10.0/seconds;
            fps = 1000000/seconds;
            cout << "frames " << fps << " seconds " << seconds << endl;
        }

// ---------------  Video Capture Code End    -------

    }
    else if (argc==2)
    {
        dynamic = 0;
        //Read the Image File
        image_source = imread(argv[1], 0);
        if (image_source.empty())
        {
            cout << "Load image file failed." << endl;
            return -1;
        }

        namedWindow("image_source", 1);
        namedWindow("image_filter_out", 1);
        namedWindow("image_blob_out", 1);

        medianFilter(image_source, image_filter_out, 5);
//        medianFilter(image_filter_out, image_filter_out, 3);

        // use threshold to cover to binary image.
//        threshold(image_filter_out, image_binary, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

        object_counts = blobCounter(image_filter_out);
        image_blob_out.create(image_source.size(), CV_8UC3);
        colorObjects(image_blob_out);
        cout<<"There are " << object_counts << " objects in this image" << endl;

        stringstream ss;
        ss << object_counts;
        string objects_string = ss.str();
        putText(image_filter_out, objects_string, cvPoint(30, 50), FONT_HERSHEY_PLAIN, 4, Scalar(255,255,255), 2, 8, 0);
        putText(image_blob_out, objects_string, cvPoint(30, 50), FONT_HERSHEY_PLAIN, 4, Scalar(255,255,255), 2, 8, 0);


        imshow("image_source", image_source);
        imshow("image_filter_out", image_filter_out);
        imshow("image_blob_out", image_blob_out);

        waitKey(0);
        exit(0);
    }
    else
    {
        cout << "The program take 1 or 0 argument" << endl;
        cout << argv[0] << " image_file_path" << endl;
        cout << argv[0] << endl;
        exit(0);
    }

}
