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

public:
    hansolo_tcp_thread(int port,std::string node_name,std::string topic_name);
    ~hansolo_tcp_thread();

    std::string sendData{};
    bool already{false};
    void server_update(int port);
    void server_start();

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
            char buf[50] = "h";
            int send_len=send(my_tcp->serverfd,(uint8_t *)buf, strlen(buf),0);
            if (send_len <= 0)
            {
                printf("tcp_send error!\n");
                close(my_tcp->serverfd);
                exit(EXIT_FAILURE);
            }
            bzero(buf, sizeof(buf));
            // std::cout << sizeof(buf) << ' ' << strlen(buf) << std::endl;
            int recv_len = recv(my_tcp->serverfd, buf, sizeof(buf), 0);
            if(recv_len<=0){
                printf("tcp_receive error!\n");
                close(my_tcp->serverfd);
                exit(EXIT_FAILURE);
            }
            std::string temp = std::string(buf, recv_len);
            M data_msg;
            data_msg.msg.ParseFromString(temp);
            data_msg.write_msg();

            // hansolo_stdMsg msg;
            // msg.data = 23333;
            // msg.write_msg();

            call_back(data_msg);
            // my_tcp->tcp_server_update_once();
            // if(sendData.length()>0){
            //     my_tcp->tcp_server_update_once_send_msg(sendData);
            // }
            // sendData.clear();
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
