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
    hansolo_image::Image get_msg()
    {

      msg.set_image_data(data);
      msg.set_frame(frameId);

      auto nowTime = msg.mutable_timestamp();
      nowTime->set_nanos(0);
      nowTime->set_seconds(time(NULL));

      return msg;
    }
    void write_msg(){
      data = msg.image_data();
      frameId = msg.frame();
      seconds = msg.timestamp().seconds();
    }
    void printMessage() const
    {
        hDebug(Color::FG_DEFAULT) << "image data";
    }
};

#endif
