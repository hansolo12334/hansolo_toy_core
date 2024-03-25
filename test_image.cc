#include "hansolo_node_new.h"


#include "imageMsg.pb.h"

#include <iostream>
#include <string>
#include <fstream>
// #include <cstring>
// #include <openssl/bio.h>
// #include <openssl/evp.h>
// #include <openssl/buffer.h>

#include <opencv2/opencv.hpp>
#include"image_tools.h"
#include"hansolo_image_msg.h"

int main()
{
  // std::string imagePath{"/home/hansolo/CplusplusLearn/hansolo_toy_core/test/cat2.jpg"};
  // std::ifstream  input(imagePath, std::ios::binary);
  // if (!input) {
  //   std::cerr << "无法打开图片文件 " << imagePath << std::endl;
  //   return 1;
  // }
  // std::string imageData((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  // hansoloImageTools imgTool{};

  std::string videoPath{"/home/hansolo/CplusplusLearn/hansolo_toy_core/test/2.mp4"};
  cv::VideoCapture cap(videoPath);
  if (!cap.isOpened())
  {
      std::cerr << "无法打开视频文件 " << videoPath << std::endl;
      return 1;
  }

  hansolo_node node{"image_node"};
  auto pub = node.create_publisher<hansolo_imageMsg>("/image_pub");

  cv::Mat frame;
  int frameId = 0;
  while (cap.read(frame))
  {
    std::vector<uchar> buf;
    cv::imencode(".jpg", frame, buf);
    std::string binaryImage(buf.begin(), buf.end());

    hansolo_imageMsg img;
    img.data = binaryImage;
    img.frameId = frameId;
    frameId++;
    pub.publish(img);
  }
}