#include<iostream>
#include"hansolo_node_new.h"

#include"hansolo_std_msg.h"
#include"stdMsg.pb.h"

#include"hansolo_publisher.h"

void sub_callback(const std::shared_ptr<hansolo_stdMsg const> &msg)
{
    hDebug(Color::FG_DEFAULT) <<"time: "<<msg->seconds<<" node2 recevie data "<< msg->data;
    sleep(1);
}


int main(int argc,char **argv)
{
    // absl::ParseCommandLine(argc, argv);

    hansolo_node client{"hansolo node2"};

    auto sub = client.create_subscriber<hansolo_stdMsg>("/test_node", sub_callback);

    while(1){

    }


    return 0;
}
