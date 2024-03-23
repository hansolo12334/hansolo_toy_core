#ifndef HANSOLO_STD_MSG_H
#define HANSOLO_STD_MSG_H

#include"stdMsg.pb.h"

#include<iostream>




class hansolo_stdMsg
{
public:
    hansolo_stdMsg(){}
    ~hansolo_stdMsg(){}
    int data{};
    hansolo_std::std_msg msg{};

    hansolo_std::std_msg get_msg(){
        msg.set_data(data);
        return msg;
    }
    void write_msg(){
        data = msg.data();
        // msg.set_data(data);
    }
};

#endif
