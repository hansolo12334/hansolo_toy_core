#include"hansolo_topic.h"

ABSL_FLAG(std::string, target, "localhost:50051", "服务端地址");

hansolo_topic::hansolo_topic()
{
  stub_ = Register::NewStub(grpc::CreateChannel(absl::GetFlag(FLAGS_target), grpc::InsecureChannelCredentials()));
}



hansolo_topic::~hansolo_topic()
{

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