#ifndef MSG_TYPE_MAP_H
#define MSG_TYPE_MAP_H
#include<functional>
#include <google/protobuf/any.pb.h>
#include "hansolo_std_msg.h"
#include "hansolo_image_msg.h"
#include <string>
#include"hansolo_std_msg.h"
#include"hansolo_image_msg.h"




static std::map<std::string, std::function<void(google::protobuf::Any &)>> topic_type_map{
    {"hansolo_std.std_msg", [](google::protobuf::Any &any)
      {    
        hansolo_stdMsg data_msg;
        any.UnpackTo(&data_msg.msg);
        data_msg.write_msg();
        data_msg.printMessage();
      }
   },
    {"hansolo_image.Image", [](google::protobuf::Any &any)
      {    
        hansolo_imageMsg data_msg;
        any.UnpackTo(&data_msg.msg);
        data_msg.write_msg();
        data_msg.printMessage();
      }
   },
};




#endif