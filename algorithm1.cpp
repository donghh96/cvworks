#include  "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

Mat image_in, image_out;

//declare a vector of sets SET[]
vector < set<  pair< int, int> > > SET;
set< pair < int, int> > tmp;

int main(int argc, char** argv)
{
    //read image_in file
    image_in = imread(argv[1], 0);

//    cout << image_in.channels() << endl;
//    cout << "image_in.step[0]:"<<image_in.step[0]<< endl;
//    cout << "image_in.step[1]:"<< image_in.step[1]<< endl;
//    cout << "dims:" << image_in.dims << endl;
//    cout << "rows:" << image_in.rows << endl;
//    cout << "cols:" << image_in.cols << endl;
//    cout << "channels:" << image_in.channels() << endl;
//    cout << "type:" << image_in.type() << endl;
//    cout << "depth:" << image_in.depth() << endl;
//    cout << "elemSize:" << image_in.elemSize() << endl;
//    cout << "elemSize1:" << image_in.elemSize1() << endl;

    //namewindow
    namedWindow("image_in", 1);
    namedWindow("image_out", 1);


    //cout << "Total count for not black or white:" << count << endl;

    //convert

    //1: declare a vector of sets SET[ ]
    //2: declare integers counter = -1, s1, s2
    int counter = -1, s1, s2;
    //3: declare A[i:width][i:height] {a 2D vector or mat initialised to -1}
    //int A[image_in.rows][image_in.cols];
    int width = image_in.cols;
    int hight = image_in.rows;

//    int black = 0;
//    int white = 0;
    //find any pixels not 255 or 0;
//    for(int x=0; x<width; x++)
//    {
//        for(int y=0; y<hight; y++)
//        {
//            int pixel = image_in.at<uchar>(y, x);
//            if(pixel != 0 &&pixel!=255)
//            {
//                cout<<pixel<<endl;
//            }
//        }
//    }
//    cout << "white is:" << white << endl;
//    cout << "black is:" << black << endl;
    //vector<vector<int> >A(image_in.cols,vector<int>(image_in.rows));//定义二维数组
    //vector<vector<int> >A(width,vector<int>hight);
    int A[width][hight];
    //initialize
    for(int i=0; i<width; i++) {
        for(int j=0; j<hight; j++) {
            A[i][j] = -1;
        }
    }
    //4:for y = 1 to i:height do
    for(int y=1; y<hight; y++)
    {
        //5: for x = 1 to i:width do
        for(int x=1; x<width; x++)
        {

            //cout << "y="<< y << "x=" << x<< endl;
            //6: if ( i(x; y) != 0 ) (white) then
            if((image_in.at<uchar>(y, x)) > 60)
            {
                //7: if (i(x - 1; y) != 0 (left) OR i(x; y - 1) != 0)(up) then
                if((image_in.at<uchar>(y, x-1)) > 60 || (image_in.at<uchar>(y-1, x))>60)
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


    //print A
//    for(int i=0; i<width; i++) {
//        for(int j=0; j<hight; j++) {
//            cout << A[i][j] << " ";
//        }
//        cout << endl;
//    }
    //print SET[0]
    //cout<<"SET.size() is:"<<SET.size()<<endl;

    image_out.create(image_in.size(), CV_8UC3);
    int x1, y1;
    int objects=0;

    for( int l=0; l<SET.size(); l++)
    {
        if (!SET[l].empty())
        {
            int b=rand()%256;
            int g=rand()%256;
            int r=rand()%256;
            objects++;
            //cout <<b<<g<<r<<endl;
            for(set< pair<int,int> >::iterator it=SET[l].begin(); it!=SET[l].end(); it++)
            {
                //cout << "SET[0].size():" < SET[0].size() << endl;
                // << (*it).first << "," << (*it).second << endl;
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

    cout<<"There are " << objects << " objects in this image" << endl;


    imshow("image_in", image_in);
    imshow("image_out", image_out);

    waitKey(0);
    exit(0);
}
