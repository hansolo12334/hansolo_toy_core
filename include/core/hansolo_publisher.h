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
    const char *m_ip{NULL};

    hansolo_tcp_thread<T> *my_tcp{nullptr};



public:
    HansoloPublisher(std::string node_name, std::string topic_name, int port,const char *ip=NULL)
        : m_node_name{node_name}
        , m_topic_name{topic_name}
        , m_port{port}
        , m_ip{ip}
    {
        my_tcp = new hansolo_tcp_thread<T>{port,node_name,topic_name,m_ip};
        my_tcp->server_start();
    }
    ~HansoloPublisher(){}

    google::protobuf::Any any;

    void publish(T &msg)
    {
        if(!my_tcp->already){
            return;
        }
        std::string temp;
        //测试 any
        
        any.PackFrom(msg.get_msg());
        // msg.get_msg().SerializeToString(&temp);
        // std::cout << msg << std::endl;
        any.SerializeToString(&temp);
     
        my_tcp->sendData = temp;
        any.Clear();
    }
};


#endif
