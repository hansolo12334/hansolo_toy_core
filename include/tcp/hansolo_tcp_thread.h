#ifndef HANSOLO_TCP_THREAD_H
#define HANSOLO_TCP_THREAD_H

#include"hansolo_tcp.h"
#include"hansolo_std_msg.h"

#include<thread>
#include<iostream>
#include<vector>
#include<chrono>
#include<future>

#include"colormod.h"
#include"hansolo_msg_base.h"

#include"baseMsg.pb.h"

#include"msg_type_map.h"

// #include"protobuf/any.pb.h"

class hansolo_tcp_thread
{
private:
    int m_a;


    std::mutex mu;
    bool m_stopE = false;
    bool m_pause = false;
    std::condition_variable m_cv;

    hansolo_tcp *my_tcp{nullptr};

    int m_port{};
    std::string m_node_name;
    std::string m_topic_name;
    google::protobuf::Any any;
    bool findAnyRealType = false;
    std::function<void(google::protobuf::Any &)> func;

public:
    hansolo_tcp_thread(int port,std::string node_name,std::string topic_name);
    ~hansolo_tcp_thread();

    std::string sendData{};
    bool already{false};
    void server_update(int port);
    void server_start();

    template<typename T>
    void topic_echo_start(void(*fp)(const T&))
    {
       int sucess= my_tcp->init_client_tcp(m_port);
       hDebug(Color::FG_BLUE) << m_node_name <<' '<<m_topic_name << " hansoloTopic启动";
        while (1)
        {
            if(!m_stopE)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                std::unique_lock<std::mutex> lock(mu);
                m_cv.wait(lock, [this] { return !m_pause; });
            }
            if(m_stopE)
            {
                break;
            }

            char buf[100] = {'h'};
            int send_len=send(my_tcp->serverfd,(uint8_t *)buf, strlen(buf),0);
            if (send_len <= 0)
            {
                Close(my_tcp->serverfd);
                int sucess= my_tcp->init_client_tcp(m_port);
                continue;
                // printf("tcp_send error!\n");
                // close(my_tcp->serverfd);
                // exit(EXIT_FAILURE);
            }
            bzero(buf, sizeof(buf));
            // std::cout << sizeof(buf) << ' ' << strlen(buf) << std::endl;
            int recv_len = recv(my_tcp->serverfd, buf, sizeof(buf), 0);
            if(recv_len<=0){
                Close(my_tcp->serverfd);
                int sucess= my_tcp->init_client_tcp(m_port);
                continue;
                // printf("tcp_receive error!\n");
                // close(my_tcp->serverfd);
                // exit(EXIT_FAILURE);
            }
            std::string temp = std::string(buf, recv_len);


            
            // data_msg.msg.ParseFromString(temp);
            // 从字节流中恢复数据结构 存入类中
            // data_msg.write_msg();

            // call_back(data_msg);
            any.ParseFromString(temp);
            if(!findAnyRealType){
                hDebug(Color::BG_DEFAULT) << any.type_url();
                findAnyRealType = true;
                func=topic_type_map[any.type_url()];
            }

            if(findAnyRealType){
                func(any);
            }
        }
    }


    template<typename M>
    void client_update(std::function<void(const M&)> call_back)
    {
        //如果初始化失败 重复
        int sucess= my_tcp->init_client_tcp(m_port);


        hDebug(Color::FG_BLUE) << m_node_name <<' '<<m_topic_name << " subscriber启动";
        while (1)
        {
            if(!m_stopE)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                std::unique_lock<std::mutex> lock(mu);
                m_cv.wait(lock, [this] { return !m_pause; });
            }
            if(m_stopE)
            {
                break;
            }

            char buf[100] = {'h'};
            int send_len=send(my_tcp->serverfd,(uint8_t *)buf, strlen(buf),0);
            if (send_len <= 0)
            {
                Close(my_tcp->serverfd);
                int sucess= my_tcp->init_client_tcp(m_port);
                continue;
                // printf("tcp_send error!\n");
                // close(my_tcp->serverfd);
                // exit(EXIT_FAILURE);
            }
            bzero(buf, sizeof(buf));
            // std::cout << sizeof(buf) << ' ' << strlen(buf) << std::endl;
            int recv_len = recv(my_tcp->serverfd, buf, sizeof(buf), 0);
            if(recv_len<=0){
                Close(my_tcp->serverfd);
                int sucess= my_tcp->init_client_tcp(m_port);
                continue;
                // printf("tcp_receive error!\n");
                // close(my_tcp->serverfd);
                // exit(EXIT_FAILURE);
            }
            std::string temp = std::string(buf, recv_len);

            
            
            any.ParseFromString(temp);

            M data_msg;
            any.UnpackTo(&data_msg.msg);
            any.Clear();
            // data_msg.msg.ParseFromString(temp);
            // 从字节流中恢复数据结构 存入类中
            data_msg.write_msg();
            // hDebug(Color::FG_BLUE) << any.type_url();
            call_back(data_msg);
        }
    }

    template<typename M>
    void client_start(std::function<void(const M&)> call_back)
    {
        std::thread t(&hansolo_tcp_thread::client_update<M>,this,call_back);
        t.detach();
    }

    void end();
    void pause();
    void resume();
};


#endif
