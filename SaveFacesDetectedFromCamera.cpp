/*
* crop images detected from camera:
* useful for creating dataset
*/
#include <openbr/openbr_plugin.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <unistd.h>
#include<iostream>
#include<string>

using namespace std;
using namespace cv;


int main(int argc, char** argv)
{
    int count1 = 1;
    br::Context::initialize(argc, argv);
    br::Globals->enrollAll = true; // Enroll 0 or more faces per image

    cv::VideoCapture cap("rtsp://admin:password@ip/h264/ch1/main/av_stream?tcp");//for ip camera

    QSharedPointer<br::Transform> transform_fd = br::Transform::fromAlgorithm("Open+Cascade(FrontalFace)+ASEFEyes");//+ASEFEyes

    br::Globals->enrollAll = true; // Enroll 0 or more faces per image

    cv::namedWindow( "camera", WINDOW_NORMAL /*WINDOW_AUTOSIZE */);
    if(!cap.isOpened())
        return -1;
    char cont = 'Y';

    do{
        Mat frame;
        cap >> frame;

        if( frame.empty() )
            break; // end of video stream

        br::Template queryB(frame);

        br::TemplateList querylist;
        querylist.push_back(queryB);//inserting queryB i.e. template in template list
        querylist >> *transform_fd;

        cv::Mat vis = frame;

        for ( auto query : querylist) {//to display faces
            const QPoint firstEye = query.file.get<QPoint>("First_Eye");
            const QPoint secondEye = query.file.get<QPoint>("Second_Eye");
            const QRect faceroi = query.file.get<QRect>("FrontalFace");

            cv::Point fe(firstEye.x(), firstEye.y());
            cv::Point se(secondEye.x(), secondEye.y());

//             cv::circle(vis, fe, 5.0, cv::Scalar(255, 0, 0), 5);
//             cv::circle(vis, se, 5.0, cv::Scalar(0, 0, 255),5);
//             cv::rectangle(vis, Point (faceroi.x(),faceroi.y()), Point (faceroi.x()+faceroi.width(),faceroi.y()+faceroi.height()), Scalar(0x0,0xff,0xff),10,8,0);


            cv::Mat mat_face ;//= vis(Rect(faceroi.x()-100, faceroi.y()-100 , faceroi.width()+100, faceroi.height()+200));
            mat_face = vis(Rect(faceroi.x(), faceroi.y() , faceroi.width(), faceroi.height()));

            imwrite(cv::format( "/home/openBRex/detect_faces/Image%d.jpg", ++count1), mat_face);

            cv::rectangle(vis, Point (faceroi.x(),faceroi.y()), Point (faceroi.x()+faceroi.width(),faceroi.y()+faceroi.height()), Scalar(0x0,0xff,0xff),10,8,0);

        }

        imshow("camera", vis);
        cv::waitKey(1);
//         if( waitKey(1) == 27 ) break; // stop capturing by pressing ESC
//         cout<<"\n\nWant to continue : Y/N "<<endl;
//         cin>>cont;
//         sleep(3);
    }while(cont = 'Y' || cont == 'y');//intentionally kept 'Y' for taking one by one use cont == Y so that it will stop for input

    // the camera will be closed automatically upon exit
//       cap.close();

    br::Context::finalize();
    return 0;
}
