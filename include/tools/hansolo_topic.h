#ifndef HANSOLO_TOPIC_H
#define HANSOLO_TOPIC_H

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"
#include "absl/flags/usage.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_cat.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "colormod.h"

#include "coreConnection.grpc.pb.h"

#include "hansolo_tcp_thread.h"

#include "hansolo_msg_base.h"

#include"baseMsg.pb.h"


using Color::Code;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using hansolo::Register;

using hansolo::replyTopics;
using hansolo::requestTopics;

using hansolo::replyEchoTopic;
using hansolo::requestEchoTopic;

class hansolo_topic
{

private:
  hansolo_tcp_thread *my_tcp{nullptr};

  
  static void echo_callback(const google::protobuf::Any &msg)
  {
    // msg.printMessage();
  }

  void init_tcp(int port, const std::string &topic_name, const std::string &node_name)
  {
    my_tcp = new hansolo_tcp_thread(port, node_name, topic_name);
    my_tcp->topic_echo_start<google::protobuf::Any>(echo_callback);
  }

public:
  hansolo_topic();
  ~hansolo_topic();


  

  void GetTopics();
  void EchoTopic(const std::string &topic_name);

private:
  std::unique_ptr<Register::Stub> stub_;
};

#endif