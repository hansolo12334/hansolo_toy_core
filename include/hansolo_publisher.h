#ifndef HANSOLO_PUBLISHER_H
#define HANSOLO_PUBLISHER_H

#include<memory>
#include<iostream>
#include<string>

#include"absl/flags/flag.h"
#include"absl/flags/parse.h"

#include<grpcpp/grpcpp.h>


#include"hansolo_tcp_thread.h"
#include"hansolo_std_msg.h"
#include"colormod.h"

template<typename T>
class HansoloPublisher
{
private:
    std::string m_node_name;
    std::string m_topic_name;
    int m_port{};

    hansolo_tcp_thread *my_tcp{nullptr};



public:
    HansoloPublisher(std::string node_name, std::string topic_name, int port)
        : m_node_name{node_name}
        , m_topic_name{topic_name}
        , m_port{port}
    {
        my_tcp = new hansolo_tcp_thread{port,node_name,topic_name};
        my_tcp->server_start();
    }
    ~HansoloPublisher(){}

    void pub(T &msg)
    {
        if(!my_tcp->already){
            return;
        }
        std::string temp;
        msg.get_msg().SerializeToString(&temp);
        my_tcp->sendData = temp;
    }
};


#endif
