#include"hansolo_topic.h"

ABSL_FLAG(std::string, target, "localhost:50051", "服务端地址");
hansolo_topic *g_topic = nullptr;
void shutdown(int signum);

hansolo_topic::hansolo_topic()
{
  stub_ = Register::NewStub(grpc::CreateChannel(absl::GetFlag(FLAGS_target), grpc::InsecureChannelCredentials()));
  g_topic = this;
  signal(SIGINT, shutdown);
}

hansolo_topic::~hansolo_topic()
{

}

void shutdown(int signum)
{
  hDebug(Color::FG_RED) << "正在关闭";
  g_topic->Close();
}

void hansolo_topic::GetTopics()
{
  requestTopics request;
  request.set_status(true);

  replyTopics reply;

  ClientContext context;

  Status status = stub_->GetTopics(&context, request, &reply);

  if(status.ok()){
    for (auto topic:reply.alltopics()){
      hDebug(Color::FG_DEFAULT) << topic.topicname();
    }
  }
  else{
    hDebug(Color::FG_RED) << "hansolo core未启动";
  }
}

void hansolo_topic::EchoTopic(const std::string &topic_name)
{
  requestEchoTopic request;
  request.set_topicname(topic_name);

  replyEchoTopic reply;
  ClientContext context;

  Status status = stub_->EchoTopic(&context, request, &reply);

  if(status.ok()){
    if(reply.status()){
      init_tcp(reply.port(),request.topicname(),"hansoloTopic");
    }
    
    // hDebug(Color::FG_DEFAULT) << reply.port();
  }
  //
  
}



// inline void hansolo_topic::echo_callback(const hansolo_msg_base &msg)
// {
//   msg.printMessage();
// }

// void hansolo_topic::init_tcp(int port,const std::string &topic_name,const std::string &node_name)
// {
//   my_tcp = new hansolo_tcp_thread(port,node_name,topic_name);


//   my_tcp->topic_echo_start<hansolo_msg_base>(echo_callback);
// }