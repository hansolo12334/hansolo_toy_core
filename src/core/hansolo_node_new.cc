#include"hansolo_node_new.h"

ABSL_FLAG(std::string, target, "localhost:50051", "服务端地址");
hansolo_node *g_node = nullptr;
void RegistOffline(int signum);

hansolo_node::hansolo_node(std::string node_name)
     :  m_node_name{node_name}
{

    stub_ = Register::NewStub(grpc::CreateChannel(absl::GetFlag(FLAGS_target), grpc::InsecureChannelCredentials()));

    bool reply =SayRegist(node_name);
    if(reply==true &&m_registSucess==false){
        hDebug(Code::FG_RED) << "注册失败 已有相同名称的节点!\n";
        exit(-1);
    }
    if(reply==true && m_registSucess==true){
        hDebug(Code::FG_GREEN) << "注册成功!\n";
    }

    g_node=this;
    signal(SIGINT, RegistOffline);
}

hansolo_node::~hansolo_node()
{

}
bool hansolo_node::SayRegist(const std::string &user)
{
    RegistRequest request;
    request.set_nodename(user);

    RegistReply reply;

    ClientContext context;

    Status status = stub_->SayRegist(&context, request, &reply);

    if(status.ok()){
        if(reply.registstatus()==requestStatus::RegistNodeAlreadyHaveSameNodeName){
            m_registSucess = false;
        }
        else{
            m_registSucess = true;
        }
        return true;

    }
    else{
        hDebug(Code::FG_RED) << status.error_code() << ": " << status.error_message();
        return false;
    }
}

int hansolo_node::RegistePublisher(const std::string topic)
{
    RegistePublisherRequest request;
    request.set_nodename(m_node_name);
    request.set_topicname(topic);

    RegistePublisherReply reply;

    ClientContext context;

    Status status = stub_->RegistePublisher(&context, request, &reply);

    if(status.ok()){
        std::string re{"从master注册 publiher成功! 端口为: "};
        re = re + std::to_string(reply.port());
        hDebug(Code::FG_GREEN) << re;
        return reply.port();
    }
    else{
        hDebug(Code::FG_RED) << status.error_code() << ": " << status.error_message();
        return -1;
    }
}

void RegistOffline(int signum)
{
    if(!g_node){
        return;
    }
    hDebug(Code::FG_RED) << "中断信号 " << signum;

    OfflineRequest request;
  
    request.set_nodename(g_node->m_node_name);
// 
    OfflineReply reply;

    ClientContext context;

    Status status = g_node->stub_->RegistOffline(&context, request, &reply);

    if(status.ok()){

        hDebug(Code::FG_RED) << "下线 " << signum;
        exit(-1);
    }
}

int hansolo_node::RegisteSubscriber(std ::string &subscritbe_topic_name)
{
    RegisteSubscriberRequest request;
    request.set_nodename(m_node_name);
    request.set_topicname(subscritbe_topic_name);

    RegisteSubscriberReply reply;

    ClientContext context;

    Status status = stub_->RegisteSubscriber(&context, request, &reply);


    if(status.ok()){

        hDebug(Code::FG_GREEN) << "订阅节点\n";
        return reply.port();
    }
    return -1;
}
