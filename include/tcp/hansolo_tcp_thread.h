#ifndef HANSOLO_TCP_THREAD_H
#define HANSOLO_TCP_THREAD_H

#include "hansolo_tcp.h"
#include "hansolo_std_msg.h"

#include <thread>
#include <iostream>
#include <vector>
#include <chrono>
#include <future>
#include<queue>

#include "colormod.h"
#include "hansolo_msg_base.h"

#include "baseMsg.pb.h"

#include "msg_type_map.h"

// #include"protobuf/any.pb.h"

template <typename M>
class hansolo_tcp_thread
{
private:
    int m_a;

    std::mutex mu;
    std::mutex data_mu;

    

    bool m_stopE = false;
    bool m_pause = false;
    std::condition_variable m_cv;

    hansolo_tcp *my_tcp{nullptr};

    int m_port{};
    const char *m_ip{NULL};
    std::string m_node_name;
    std::string m_topic_name;
    google::protobuf::Any any;
    bool findAnyRealType = false;
    std::function<void(google::protobuf::Any &)> func;

    M msg_data;
    std::queue<M> data_queue{};

    

public:
  

    // std::string sendData{};
    bool already{true};
    std::mutex send_data_mu;
    bool connectReady{true};
    std::queue<std::string> send_data_queue{};

    hansolo_tcp_thread(int port, std::string node_name, std::string topic_name,const char *ip)
        : m_port{port}, m_node_name{node_name}, m_topic_name{topic_name}, 
        m_ip{ip},
        my_tcp{new hansolo_tcp{}}
    {
    }
    hansolo_tcp_thread(int port, std::string node_name, std::string topic_name)
    : m_port{port}, m_node_name{node_name}, m_topic_name{topic_name}, 
    my_tcp{new hansolo_tcp{}}
    {
    }

    ~hansolo_tcp_thread()
    {
    }

    void server_start()
    {
        std::thread t(&hansolo_tcp_thread::server_update, this, m_port,m_ip);
        t.detach();
    }

    void end()
    {
        std::cout << "sssssssssssssss end \n";
        m_stopE = true;
    }

    void pause()
    {
        std::unique_lock<std::mutex> lock(mu);
        m_pause = true;
        m_cv.notify_one();
    }

    void resume()
    {
        std::unique_lock<std::mutex> lock(mu);
        m_pause = false;
        m_cv.notify_one();
    }

    void server_update(int port,const char *ip)
    {   
     
        my_tcp->init_server_tcp(port,ip);

        hDebug(Color::FG_BLUE) << m_node_name << ' ' << m_topic_name << " publisher启动";
        while (1)
        {
            if (!m_stopE)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                std::unique_lock<std::mutex> lock(mu);
                m_cv.wait(lock, [this]
                          { return !m_pause; });
            }
            if (m_stopE)
            {
                break;
            }
     
            // if (sendData.length() > 0)
            // {
                already = false;
                std::lock_guard<std::mutex> lock_data(send_data_mu);
      
                while(!send_data_queue.empty())
                {
                    connectReady=my_tcp->tcp_server_update_once_send_msg_big_data(send_data_queue.front());
                  
                    send_data_queue.pop();
                    // hDebug() << "connectReady " << connectReady;
                }
                // sendData.clear();
                already = true;
            // }
           
        }
    }

   
    void topic_echo_start(void (*fp)(const M &))
    {
        int sucess = my_tcp->init_client_tcp(m_port,m_ip);
        hDebug(Color::FG_BLUE) << m_node_name << ' ' << m_topic_name << " hansoloTopic启动";
        while (1)
        {
            if (!m_stopE)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                std::unique_lock<std::mutex> lock(mu);
                m_cv.wait(lock, [this]
                          { return !m_pause; });
            }
            if (m_stopE)
            {
                break;
            }

            char buf[100] = {'h'};
            int send_len = send(my_tcp->serverfd, (uint8_t *)buf, strlen(buf), 0);
            if (send_len <= 0)
            {
                Close(my_tcp->serverfd);
                int sucess = my_tcp->init_client_tcp(m_port,m_ip);
                continue;
                // printf("tcp_send error!\n");
                // close(my_tcp->serverfd);
                // exit(EXIT_FAILURE);
            }
            bzero(buf, sizeof(buf));
            // 第一次接收发来的数据大小
            char buffer_header[sizeof(size_t)] = {0};
            int data_size = recv(my_tcp->serverfd, buffer_header, sizeof(size_t), 0);
            // std::cout << data_size << std::endl;

            if (data_size <= 0)
            {
                Close(my_tcp->serverfd);
                int sucess = my_tcp->init_client_tcp(m_port,m_ip);
                continue;
                // printf("tcp_receive error!\n");
                // close(my_tcp->serverfd);
                // exit(EXIT_FAILURE);
            }
            // 开始接收发来的数据包
            size_t msg_size;
            memcpy(&msg_size, buffer_header, sizeof(size_t)); // sizeof(size_t) => 8

            char *buffer_msg = (char *)malloc(msg_size * sizeof(char));
            memset(buffer_msg, '\0', sizeof(char) * msg_size);
            int length = 0;
            for (length = 0; msg_size != 0; length += data_size)
            {
                // 客户端第二次发送来的才是数据
                data_size = recv(my_tcp->serverfd, buffer_msg + length, msg_size, 0);
                msg_size -= data_size;
                // std::cout << data_size << std::endl;
            }
            std::string temp = std::string(buffer_msg, length);
            free(buffer_msg);

            // data_msg.msg.ParseFromString(temp);
            // 从字节流中恢复数据结构 存入类中
            // data_msg.write_msg();

            // call_back(data_msg);
            {
                std::lock_guard<std::mutex> data_lock(data_mu);
                any.ParseFromString(temp);
                if (!findAnyRealType)
                {
                    hDebug(Color::BG_DEFAULT) << any.type_url();
                    findAnyRealType = true;
                    func = topic_type_map[any.type_url()];
                }

                if (findAnyRealType)
                {
                    func(any);
                }
            }
        }
        //关闭节点
        Close(my_tcp->serverfd);
    }

  
    // void client_update(std::function<void(const M &)> call_back)
    // {
    //     // 如果初始化失败 重复
    //     int sucess = my_tcp->init_client_tcp(m_port);

    //     hDebug(Color::FG_BLUE) << m_node_name << ' ' << m_topic_name << " subscriber启动";
    //     while (1)
    //     {
    //         if (!m_stopE)
    //         {
    //             std::this_thread::sleep_for(std::chrono::milliseconds(1));
    //             std::unique_lock<std::mutex> lock(mu);
    //             m_cv.wait(lock, [this]
    //                       { return !m_pause; });
    //         }
    //         if (m_stopE)
    //         {
    //             break;
    //         }

    //         char buf[1024] = {'h'};
    //         int send_len = send(my_tcp->serverfd, (uint8_t *)buf, strlen(buf), 0);
    //         if (send_len <= 0)
    //         {
    //             Close(my_tcp->serverfd);
    //             int sucess = my_tcp->init_client_tcp(m_port);
    //             continue;
    //             // printf("tcp_send error!\n");
    //             // close(my_tcp->serverfd);
    //             // exit(EXIT_FAILURE);
    //         }
    //         bzero(buf, sizeof(buf));
    //         // std::cout << sizeof(buf) << ' ' << strlen(buf) << std::endl;

    //         int recv_len = recv(my_tcp->serverfd, buf, sizeof(buf), 0);
    //         // std::cout << recv_len << std::endl;
    //         if (recv_len <= 0)
    //         {
    //             Close(my_tcp->serverfd);
    //             int sucess = my_tcp->init_client_tcp(m_port);
    //             continue;
    //             // printf("tcp_receive error!\n");
    //             // close(my_tcp->serverfd);
    //             // exit(EXIT_FAILURE);
    //         }
    //         std::string temp = std::string(buf, recv_len);

    //         any.ParseFromString(temp);

    //         M data_msg;
    //         any.UnpackTo(&data_msg.msg);
    //         any.Clear();
    //         // data_msg.msg.ParseFromString(temp);
    //         // 从字节流中恢复数据结构 存入类中
    //         data_msg.write_msg();
    //         // hDebug(Color::FG_BLUE) << any.type_url();
    //         call_back(data_msg);
    //     }
    // }

    
    void client_update_data_big(std::function<void(const M &)> call_back)
    {
        // 如果初始化失败 重复
        int sucess = my_tcp->init_client_tcp(m_port,m_ip);

        hDebug(Color::FG_BLUE) << m_node_name << ' ' << m_topic_name << " subscriber启动";
     
        std::thread t(&hansolo_tcp_thread::update_callback, this, call_back);
        t.detach();

        while (1)
        {
            if (!m_stopE)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                std::unique_lock<std::mutex> lock(mu);
                m_cv.wait(lock, [this]
                          { return !m_pause; });
            }
            if (m_stopE)
            {
                break;
            }

            char buf[1024] = {'h'};
            int send_len = send(my_tcp->serverfd, (uint8_t *)buf, strlen(buf), 0);
            if (send_len <= 0)
            {
                Close(my_tcp->serverfd);
                int sucess = my_tcp->init_client_tcp(m_port,m_ip);
                continue;
                // printf("tcp_send error!\n");
                // close(my_tcp->serverfd);
                // exit(EXIT_FAILURE);
            }
            bzero(buf, sizeof(buf));

            // 第一次接收发来的数据大小
            char buffer_header[sizeof(size_t)] = {0};
            int data_size = recv(my_tcp->serverfd, buffer_header, sizeof(size_t), 0);
            // std::cout << data_size << std::endl;

            if (data_size <= 0)
            {
                Close(my_tcp->serverfd);
                int sucess = my_tcp->init_client_tcp(m_port,m_ip);
                continue;
                // printf("tcp_receive error!\n");
                // close(my_tcp->serverfd);
                // exit(EXIT_FAILURE);
            }
            // 开始接收发来的数据包
            size_t msg_size;
            memcpy(&msg_size, buffer_header, sizeof(size_t)); // sizeof(size_t) => 8

            char *buffer_msg = (char *)malloc(msg_size * sizeof(char));
            memset(buffer_msg, '\0', sizeof(char) * msg_size);
            int length = 0;
            for (length = 0; msg_size != 0; length += data_size)
            {
                // 客户端第二次发送来的才是数据
                data_size = recv(my_tcp->serverfd, buffer_msg + length, msg_size, 0);
                msg_size -= data_size;
                // std::cout << data_size << std::endl;
            }
            std::string temp = std::string(buffer_msg, length);
            free(buffer_msg);

            {
                any.ParseFromString(temp);
                any.UnpackTo(&msg_data.msg);
                // 从字节流中恢复数据结构 存入类中
                any.Clear();
                msg_data.write_msg();
                std::lock_guard<std::mutex> data_lock(data_mu);
                data_queue.push(msg_data);
                // hDebug(Color::FG_BLUE) << "写data "<<msg_data.getSecond();
            }
            // call_back(msg_data);
        }
        Close(my_tcp->serverfd);
    }

  
    void client_start(std::function<void(const M &)> call_back)
    {
        std::thread t(&hansolo_tcp_thread::client_update_data_big, this, call_back);
        t.detach();
    }


 
    void update_callback(std::function<void(const M &)> call_back)
    {
        while (1)
        {
            if (!m_stopE)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                std::unique_lock<std::mutex> lock(mu);
                m_cv.wait(lock, [this]
                          { return !m_pause; });

                if (m_stopE)
                {
                    break;
                }
                // M data;
                {
                    
                    // data = msg_data;
                    // hDebug(Color::FG_BLUE) << "读data ";
                    // if(!msg_data.isEmpty){
                        if(!data_queue.empty()){
                            // call_back(msg_data);
                            std::lock_guard<std::mutex> data_lock(data_mu);
                            call_back(data_queue.front());
                            data_queue.pop();
                        }

                        // msg_data.isEmpty = true;
                    // }
                }
                
            }
            
            
        }
    }

    
};

#endif
