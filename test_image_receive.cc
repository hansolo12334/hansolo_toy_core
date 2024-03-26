#include<iostream>
#include"hansolo_node_new.h"


#include"hansolo_publisher.h"

#include <opencv2/opencv.hpp>
#include"image_tools.h"
#include"hansolo_image_msg.h"


void sub_callback(const std::shared_ptr<hansolo_imageMsg const> &msg)
{

  hansoloImageTools tool;

  cv::Mat img = tool.binaryImage2Mat(msg->data);
  if(img.empty()){
    return;
  }
  hDebug(Color::BG_DEFAULT) << msg->frameId;
  // cv::resize(img, img, cv::Size(img.size[1]/6,img.size[0]/6));
  cv::imshow("Image", img);

  if(cv::waitKey(30)>=0){
    return;
  }   
}



int main(int argc,char **argv)
{
  

    hansolo_node client{"hansolo video1"};

    auto sub = client.create_subscriber<hansolo_imageMsg>("/image_pub", sub_callback);

    cv::namedWindow("Image",cv::WINDOW_NORMAL);
    while(1){
 
    }


    return 0;
}
