#ifndef HANSOLO_MASTER_H
#define HANSOLO_MASTER_H

#include"absl/flags/flag.h"
#include"absl/flags/parse.h"
#include"absl/strings/str_format.h"

#include<grpcpp/ext/proto_server_reflection_plugin.h>
#include<grpcpp/grpcpp.h>
#include<grpcpp/health_check_service_interface.h>


#include<iostream>
#include<string>
#include<map>
#include<memory>
#include<vector>
#include<unordered_map>
#include<algorithm>

#include"colormod.h"

#include"coreConnection.grpc.pb.h"

using grpc::CallbackServerContext;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerUnaryReactor;
using grpc::Status;
// using hansolo_thread::Greeter;
// using hansolo_thread::HelloReply;
// using hansolo_thread::HelloRequest;

using hansolo::Register;
using hansolo::RegistReply;
using hansolo::RegistRequest;


using hansolo::RegistePublisherReply;
using hansolo::RegistePublisherRequest;

using hansolo::RegisteSubscriberReply;
using hansolo::RegisteSubscriberRequest;

using hansolo::OfflineReply;
using hansolo::OfflineRequest;

using hansolo::requestStatus;
using hansolo::singleTopic;

using hansolo::replyTopics;
using hansolo::requestTopics;

class hansolo_master final : public Register::CallbackService
{

private:
    int m_port{4321};
    std::unordered_map<std::string, int> m_node_names{};


    struct node_publish_item
    {
        std::string published_topic_name{};
        int self_port{};
    };
    struct node_subscribe_item
    {
        std::string subscribed_topic_name{};
        std::string subscribed_node_name{};
    };

    enum nodeStatues
    {
        OFFLINE,
        ONLINE,
        RESERVE,
    };
    struct node_items
    {
        std::string node_name;
        nodeStatues status{OFFLINE};
        //  pair 自己发布的 话题名称  端口名称
        std::vector<node_publish_item> publish_topics{};
        //  pair : 订阅的话题名称 发布该话题名称的节点名字
        std::vector<node_subscribe_item> subscribe_topics{};
    };
    std::vector<node_items> m_all_nodes{};


public:
    ServerUnaryReactor *SayRegist(CallbackServerContext *context,
                                  const RegistRequest *request,
                                  RegistReply *reply) override;


    ServerUnaryReactor *RegistePublisher(CallbackServerContext *context,
                                         const RegistePublisherRequest *request,
                                         RegistePublisherReply *reply) override;

    ServerUnaryReactor *RegisteSubscriber(CallbackServerContext *context,
                                          const RegisteSubscriberRequest *request,
                                          RegisteSubscriberReply *reply) override;

    ServerUnaryReactor *RegistOffline(CallbackServerContext *context,
                                         const OfflineRequest *request,
                                         OfflineReply *reply) override;

    ServerUnaryReactor *GetTopics(CallbackServerContext *context,
                                                  const requestTopics *request,
                                                  replyTopics *reply) override;
};

#endif
