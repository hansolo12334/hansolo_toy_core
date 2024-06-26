#ifndef HANSOLO_SUBSCRIBER_H
#define HANSOLO_SUBSCRIBER_H

#include"hansolo_std_msg.h"
#include"hansolo_tcp_thread.h"

#include<functional>
#include"colormod.h"


template <typename M>
class HansoloSubscriber
{

private:
    hansolo_stdMsg m_data;

    std::string m_node_name;
    std::string m_topic_name;
    int m_port{};
    const char *m_ip{NULL};
    hansolo_tcp_thread<M> *my_tcp{nullptr};

public:
    HansoloSubscriber(std::string topic_name,std::string node_name,int port,const char *ip=NULL)
     : m_topic_name{topic_name}
     , m_node_name{node_name}
     , m_port{port}
     , m_ip{ip}
    {
         my_tcp = new hansolo_tcp_thread<M>{port,node_name,topic_name,m_ip};
    }
    
    void shutdown(){
        if(my_tcp!=nullptr){
            my_tcp->end();
        }
    }

    void init(std::function<void(const M&)> call_back){

        my_tcp->client_start(call_back);

        hDebug(Color::FG_GREEN) << "初始化subscribe 订阅端口为: " <<m_port<<'\n';
        // m_data.data = 1222;
        // call_back(m_data);
    }
};
#endif
