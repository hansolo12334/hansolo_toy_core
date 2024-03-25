#include<iostream>
#include"hansolo_node_new.h"


#include"hansolo_publisher.h"

#include <opencv2/opencv.hpp>
#include"image_tools.h"
#include"hansolo_image_msg.h"


void sub_callback(const hansolo_imageMsg &msg)
{
  std::string decode{};
  hansoloImageTools tool;
  // decode=tool.base64_decode(msg.data);
  // decode = msg.data;
  if(msg.data.length()<=0){
    return;
  }
  std::vector<unsigned char> imageDataVec(msg.data.begin(), msg.data.end());
  cv::Mat img = cv::imdecode(imageDataVec, cv::IMREAD_COLOR);
   if (img.empty()) {
    std::cerr << "无法加载图片。" << std::endl;
    return;
   }
  hDebug(Color::BG_DEFAULT) << msg.frameId;
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
