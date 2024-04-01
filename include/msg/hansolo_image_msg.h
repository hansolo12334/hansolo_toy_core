#ifndef HANSOLO_IMAGE_MSG_H
#define HANSOLO_IMAGE_MSG_H

#include"imageMsg.pb.h"

#include<google/protobuf/timestamp.pb.h>
#include<time.h>
#include<ctime>
#include<iostream>
#include <unistd.h>

#include"hansolo_time.h"
#include"colormod.h"

using Color::Code;

class hansolo_imageMsg : public hansoloTime
{
public:
    hansolo_imageMsg(){}
    ~hansolo_imageMsg(){}

   


    std::string data{};
    hansolo_image::Image  msg{};
    int frameId{};
    int height{};
    int width{};
    hansolo_image::Image get_msg()
    {

      msg.set_image_data(data);
      msg.set_frame(frameId);
      msg.set_height(height);
      msg.set_width(width);

      auto nowTime = msg.mutable_timestamp();
      nowTime->set_nanos(0);
      nowTime->set_seconds(time(NULL));
      isEmpty = false;
      return msg;
    }
    void write_msg(){
      data = msg.image_data();
      frameId = msg.frame();
      width = msg.width();
      height = msg.height();
      
      seconds = msg.timestamp().seconds();
      isEmpty = false;
    }
    void printMessage() const
    {
      hDebug(Color::FG_DEFAULT) << "---\nimage data: \ntimeStamp: "<<seconds<< "\nframeId: " << frameId << "\nwidth: " << width << "\nheight: " << height;
    }
};

#endif
