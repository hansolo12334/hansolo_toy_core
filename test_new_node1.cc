#include<iostream>
#include"hansolo_node_new.h"

#include"hansolo_std_msg.h"
#include"stdMsg.pb.h"

#include"hansolo_publisher.h"

void sub_callback(const hansolo_stdMsg &msg)
{
    hDebug(Color::FG_DEFAULT) <<"node1 recevie data "<< msg.data;
    sleep(2);
}


int main(int argc,char **argv)
{
    // absl::ParseCommandLine(argc, argv);

    hansolo_node client{"hansolo node1"};

    auto sub = client.create_subscriber<hansolo_stdMsg>("/test_node", sub_callback);

    HansoloPublisher<hansolo_stdMsg> pub= client.create_publisher<hansolo_stdMsg>("/test_node1");

    while(1){
        hansolo_stdMsg msg;
        msg.data = 1222;
        // sleep(1);
        pub.pub(msg);
    }


    return 0;
}
