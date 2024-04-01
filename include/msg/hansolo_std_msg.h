#ifndef HANSOLO_STD_MSG_H
#define HANSOLO_STD_MSG_H

#include"stdMsg.pb.h"

#include<google/protobuf/timestamp.pb.h>
#include<time.h>
#include<ctime>
#include<iostream>
#include <unistd.h>

#include"hansolo_time.h"
#include"colormod.h"

using Color::Code;

class hansolo_stdMsg : public hansoloTime
{
public:
    hansolo_stdMsg(){}
    ~hansolo_stdMsg(){}

    hansolo_std::std_msg ss{};


    int data{};
    hansolo_std::std_msg msg{};

    hansolo_std::std_msg get_msg(){
        msg.set_data(data);

        auto nowTime = msg.mutable_timestamp();
        nowTime->set_nanos(0);
        nowTime->set_seconds(time(NULL));
        isEmpty = false;
        return msg;
    }
    void write_msg(){
        data = msg.data();
        seconds = msg.timestamp().seconds();
        isEmpty = false;
    }
    void printMessage() const
    {
        hDebug(Color::FG_DEFAULT) <<
        "---\n"<<
        "std_msg:\n"<<
        "timeStamp:"<<
        ' '<<seconds<<
        "\ndata: " << data;
    }
};

#endif
