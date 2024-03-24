#ifndef HANSOLO_STD_MSG_H
#define HANSOLO_STD_MSG_H

#include"stdMsg.pb.h"

#include<google/protobuf/timestamp.pb.h>
#include<time.h>
#include<ctime>
#include<iostream>
#include <unistd.h>

#include"hansolo_time.h"


class hansolo_stdMsg : public hansoloTime
{
public:
    hansolo_stdMsg(){}
    ~hansolo_stdMsg(){}
    
    int data{};
    hansolo_std::std_msg msg{};

    hansolo_std::std_msg get_msg(){
        msg.set_data(data);

        auto nowTime = msg.mutable_timestamp();
        nowTime->set_nanos(0);
        nowTime->set_seconds(time(NULL));
  
        return msg;
    }
    void write_msg(){
        data = msg.data();
        seconds = msg.timestamp().seconds();
    }

    
};

#endif
