#ifndef HANSOLO_NODE_H
#define HANSOLO_NODE_H

#include<memory>
#include<iostream>
#include<string>
#include<functional>

#include<signal.h>

#include"absl/flags/flag.h"
#include"absl/flags/parse.h"

#include<grpcpp/grpcpp.h>


#include"hansolo_tcp_thread.h"

#include"colormod.h"
#include"hansolo_publisher.h"
#include"hansolo_subscriber.h"

#include"coreConnection.grpc.pb.h"


//msg
#include"hansolo_std_msg.h"

using Color::Code;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using hansolo::RegistReply;
using hansolo::Register;
using hansolo::RegistRequest;

using hansolo::RegistePublisherReply;
using hansolo::RegistePublisherRequest;

using hansolo::RegisteSubscriberReply;
using hansolo::RegisteSubscriberRequest;

using hansolo::OfflineReply;
using hansolo::OfflineRequest;

using hansolo::requestStatus;



// namespace hansolo_rt{
//     class hansolo_node; 
// };

class hansolo_node
{
public:
    hansolo_node(std::string node_name);
    ~hansolo_node();




    bool SayRegist(const std::string &user);


    int RegistePublisher(const std::string topic);

    // void RegistOffline(int signum);

    int RegisteSubscriber(std ::string &subscritbe_topic_name);

    template<typename T>
    HansoloPublisher<T> create_publisher(std::string topic_name)
    {
        int port = RegistePublisher(topic_name);

        HansoloPublisher<T> pub{m_node_name,topic_name,port};
        return pub;
    }
    //todo
    template<typename M>
    HansoloSubscriber<M> create_subscriber(std::string topic_name, void(*fp)(const M&))
    {
        int port = RegisteSubscriber(topic_name);
        HansoloSubscriber<M> sub{topic_name, m_node_name, port};
        sub.init(fp);
        
        return sub;
    }
    
    // static hansolo_node* instancePtr;
// private:
public:
    std::unique_ptr<Register::Stub> stub_;

    bool m_registSucess = false;
    // std::vector<HansoloPublisher> m_pubs{};
    std::string m_node_name{};



};


#endif
