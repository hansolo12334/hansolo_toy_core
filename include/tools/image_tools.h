#ifndef IMAGE_TOOLS_H
#define IMAGE_TOOLS_H

// #include <openssl/bio.h>
// #include <openssl/evp.h>
// #include <openssl/buffer.h>
#include<string>
#include<cstring>
#include<vector>
#include<opencv2/opencv.hpp>
#include"hansolo_image_msg.h"
#include"colormod.h"

using Color::Code;

class hansoloImageTools
{
private:
  std::string m_image_path{};
  std::string m_video_path{};
  cv::VideoCapture m_cap;

public:
  std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

  std::string base64_encode(const std::string &in);

  std::string base64_decode(const std::string &in);

  hansoloImageTools(std::string video_path)
    : m_video_path{video_path}
  {
    m_cap.open(m_video_path);
    if(!m_cap.isOpened()){
      std::cerr << "无法打开视频文件 " << m_video_path << std::endl;
      exit(-1);
    }
  }
  hansoloImageTools()
  {

  }

  hansolo_imageMsg getImage(std::string imagePath){
    cv::Mat img;
    hansolo_imageMsg h_img;
    h_img.data.clear();
    img = cv::imread(imagePath);
    if(img.empty()){
      hDebug(Color::FG_RED) << "打开失败";
      return h_img;
    }
    std::vector<uchar> buf;
    cv::imencode(".jpg", img, buf);
    std::string binaryImage(buf.begin(), buf.end());
    h_img.data = binaryImage;
    h_img.frameId = 0;
    h_img.height = img.size().height;
    h_img.width = img.size().width;
    return h_img;
  }
  hansolo_imageMsg getImage(){
    cv::Mat img;
    hansolo_imageMsg h_img;

    if(!m_cap.read(img)){
      hDebug(Color::FG_RED) << "视频读取完毕";
      h_img.data.clear();
      return h_img;
    }

    std::vector<uchar> buf;
    cv::imencode(".jpg", img, buf);
    std::string binaryImage(buf.begin(), buf.end());
    h_img.data = binaryImage;
    h_img.frameId = 0;
    h_img.height = img.size().height;
    h_img.width = img.size().width;
    return h_img;
  }

  cv::Mat binaryImage2Mat(const std::string &image){
    cv::Mat img;
    if (image.length() <= 0)
    {
      hDebug(Color::FG_RED) << "二进制图片格式非法";
      return img;
    }
    std::vector<unsigned char> imageDataVec(image.begin(), image.end());
    img = cv::imdecode(imageDataVec, cv::IMREAD_COLOR);
    if (img.empty()) {
      hDebug(Color::FG_RED) << "无法加载图片";
    }
    return img;
  }

};

#endif