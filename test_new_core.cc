#include<iostream>
#include"hansolo_master_new.h"

#include"colormod.h"
//          参数类型，参数名， 默认值， 备注。
ABSL_FLAG(uint16_t, port, 50051, "用于服务端的端口");

void RunServer(uint16_t port)
{
    std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
    hansolo_master master;


    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;

    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    builder.RegisterService(&master);

    std::unique_ptr<Server> server(builder.BuildAndStart());

    hDebug(Color::FG_BLUE) << "服务端 在端口 " << server_address << " 监听";

    server->Wait();
}

int main(int argc,char **argv)
{
    RunServer(absl::GetFlag(FLAGS_port));
}
