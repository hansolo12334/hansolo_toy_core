#include<iostream>
#include"hansolo_node_new.h"

#include"hansolo_std_msg.h"


void sub_callback(const std::shared_ptr<hansolo_stdMsg const> &msg)
{

    hDebug(Color::FG_DEFAULT) <<"time: "<<msg->seconds<< " node recevie data "<< msg->data;
    // std::cout << "node recevie data " << msg.data << std::endl;
    sleep(2);
}

int main(int argc,char **argv)
{
    // absl::ParseCommandLine(argc, argv);
    hansolo_node client{"hansolo node"};

    HansoloPublisher<hansolo_stdMsg> pub= client.create_publisher<hansolo_stdMsg>("/test_node");
    HansoloPublisher<hansolo_stdMsg> pub1= client.create_publisher<hansolo_stdMsg>("/test_node11");

    // auto sub = client.create_subscriber<hansolo_stdMsg>("/test_node1", sub_callback);

    while (1)
    {
        hansolo_stdMsg msg;
        msg.data = 233;
        // sleep(1);
        pub.publish(msg);
        msg.data = 23333;
        pub1.publish(msg);
    }

    return 0;
}
