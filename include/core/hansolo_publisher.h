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
        // hDebug() << msg.frameId << "-++++++--";
        //如果 tcp没发送完数据 或者 没连接客户端 就返回
        // 第一次发送时 already初始值是true 所以跳过
        // if(!my_tcp->already ){
        //     return;
        // }
        std::string temp;
     
        any.PackFrom(msg.get_msg());
  
        any.SerializeToString(&temp);

        //如果还没发完 且 tcp以及连接了客户端 则先将数据填入队列
        
        // my_tcp->sendData = temp;
        if(my_tcp->connectReady ) {
            std::lock_guard<std::mutex> data_lock(my_tcp->send_data_mu);
            my_tcp->send_data_queue.push(temp);
        }
        // 
        any.Clear();
    }
};


#endif
