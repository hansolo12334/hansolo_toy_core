#ifndef HANSOLO_TOPIC_H
#define HANSOLO_TOPIC_H

#include"absl/flags/flag.h"
#include"absl/flags/parse.h"
#include"absl/strings/str_format.h"
#include "absl/flags/usage.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_cat.h"

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

using Color::Code;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;


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


using hansolo::replyTopics;
using hansolo::requestTopics;

class hansolo_topic 
{
public:
  hansolo_topic();
  ~hansolo_topic();

  void GetTopics();

private:
    std::unique_ptr<Register::Stub> stub_;
};

#endif