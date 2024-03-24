#ifndef MSG_TYPE_MAP_H
#define MSG_TYPE_MAP_H

#include <functional>
#include <google/protobuf/any.pb.h>
#include <hansolo_std_msg.h>
#include <string>
// namespace::msg_func_map{

static std::map<std::string, std::function<void(google::protobuf::Any &)>> topic_type_map{
    {"type.googleapis.com/hansolo_std.std_msg", [](google::protobuf::Any &any)
     {
       hansolo_stdMsg data_msg;
       any.UnpackTo(&data_msg.msg);
       data_msg.write_msg();
       data_msg.printMessage();
     }
    }


};



#endif