#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/core/matx.hpp"
#include "avatar/Avatar.hpp"
using namespace std;
 static const std::string OPENCV_WINDOW = "Image window";
  static const std::string NEW_WINDOW = "mask window";
 class ImageConverter
{
     ros::NodeHandle nh_;
     image_transport::ImageTransport it_;
     image_transport::Subscriber image_sub_;
     image_transport::Publisher image_pub_;

   public:
     ImageConverter()
       : it_(nh_)
   {
	   
	   //roslaunch ros2opencv usb_cam.launch 
       //roslaunch pi_face_tracker face_tracker_usb_cam.launch
       // Subscrive to input video feed and publish output video feed
       image_sub_ = it_.subscribe("/camera/pi_face_tracker/image", 1,
         &ImageConverter::imageCb, this);
         
       image_pub_ = it_.advertise("/image_converter/output_video", 1);

       cv::namedWindow(OPENCV_WINDOW);
       cv::namedWindow(NEW_WINDOW);
     }

     ~ImageConverter()
     {
       cv::destroyWindow(OPENCV_WINDOW);
       cv::destroyWindow(NEW_WINDOW);
     }

     void imageCb(const sensor_msgs::ImageConstPtr& msg)
     {
       cv_bridge::CvImagePtr cv_ptr;
       cv_bridge::CvImagePtr pi_ptr;
       try
       {
         cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
         pi_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
       }
       catch (cv_bridge::Exception& e)
       {
         ROS_ERROR("cv_bridge exception: %s", e.what());
         return;
       }

      // Draw an example circle on the video stream
       if (cv_ptr->image.rows > 60 && cv_ptr->image.cols > 60)
         cv::circle(cv_ptr->image, cv::Point(50, 50), 10, CV_RGB(255,0,0));
           //Mat source = imread("chessOrange.png",1);
          //  imshow("source", source);
          
       // Update GUI Window
       cv::imshow(OPENCV_WINDOW, cv_ptr->image);
       cv::waitKey(3);
       

       // Output modified video stream
        image_pub_.publish(cv_ptr->toImageMsg());
        // cv::destroyWindow(OPENCV_WINDOW);
        std::cout<<"reached here";
         if (pi_ptr->image.rows > 60 && pi_ptr->image.cols > 60)
         cv::circle(pi_ptr->image, cv::Point(50, 50), 10, CV_RGB(255,0,0));
         cv::Mat mask;
      cv::Scalar upperb = cv::Scalar(153, 250, 153);
      cv::Scalar lowerb = cv::Scalar(0, 204, 0);
      cv::inRange(pi_ptr->image, lowerb, upperb, mask);
      cv::imshow(NEW_WINDOW, mask);
       //     cv::waitKey(3);
     }
   };

   int main(int argc, char** argv)
   {
	//std::cout<<"reached here 1";
    ros::init(argc, argv, "image_converter");
    ImageConverter ic;
   // std::cout<<"reached here 2";
    ros::spin();
    //std::cout<<"reached here 3";
     return 0;
   }
