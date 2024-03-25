#include "hansolo_node_new.h"

#include <iostream>
#include <string>
#include <fstream>


#include <opencv2/opencv.hpp>
#include"image_tools.h"
#include"hansolo_image_msg.h"

int main()
{

  std::string videoPath{"/home/hansolo/CplusplusLearn/hansolo_toy_core/test/2.mp4"};
  hansoloImageTools imgTool{videoPath};
  

  hansolo_node node{"image_node"};
  auto pub = node.create_publisher<hansolo_imageMsg>("/image_pub");

  hansolo_imageMsg img;
  int frameId = 0;
  img = imgTool.getImage();
  img.frameId = frameId;
  while (img.data.length()>0)
  {
    pub.publish(img);
    img = imgTool.getImage();
    frameId++;
    img.frameId = frameId;
  }
}