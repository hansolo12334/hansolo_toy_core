#include "hansolo_tcp.h"


hansolo_tcp::hansolo_tcp()
{
}
hansolo_tcp::~hansolo_tcp()
{
}


bool hansolo_tcp::init_server_tcp(int port,const char* ip)
{
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    if(ip==NULL){
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else{
        servaddr.sin_addr.s_addr = inet_addr(ip);
    }
    servaddr.sin_port = htons(port);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    Listen(listenfd, 20);


    maxfd = listenfd; /* initialize */
    maxi = -1;        /* index into client[] array */

    for (int i = 0; i < FD_SETSIZE; i++)
    {
        client[i] = -1; /* -1 indicates available entry */
    }

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    return true;
}


/// @brief 使用select模型的tcp服务端
/// @param message
/// @return
bool hansolo_tcp::tcp_server_update_once_send_msg(std::string &message)
{

    int i = 0;
    rset = allset; /* structure assignment */
    nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
    if (nready < 0)
    {
        perr_exit("select error");
    }
    if (FD_ISSET(listenfd, &rset))
    { /* new client connection */
        cliaddr_len = sizeof(cliaddr);
        connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);

        printf("received from %s at PORT %d\n",
               inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
               ntohs(cliaddr.sin_port));

        for (i = 0; i < FD_SETSIZE; i++){
            if (client[i] < 0)
            {
                client[i] = connfd; /* save descriptor */
                break;
            }
        }

        if (i == FD_SETSIZE)
        {
            fputs("too many clients\n", stderr);
            exit(1);
        }

        FD_SET(connfd, &allset); /* add new descriptor to set */
        if (connfd > maxfd){
            maxfd = connfd; /* for select */
        }

        if (i > maxi){
            maxi = i; /* max index in client[] array */
        }


        if (--nready == 0){
            return 1; /* no more readable descriptors */
        }

    }

    for (i = 0; i <= maxi; i++)
    { /* check all clients for data */
        if ((sockfd = client[i]) < 0){
             continue;
        }
        if (FD_ISSET(sockfd, &rset))
        {
            if ((data_size = Read(sockfd, buf, MAXLINE)) == 0)
            {
                /* connection closed by client */
                Close(sockfd);
                FD_CLR(sockfd, &allset);
                client[i] = -1;
            }
            else
            {
                // std::cout << message.length() << std::endl;
                Write(sockfd, message.data(), message.length());
                // Write(sockfd, buf, data_size);
            }

            if (--nready == 0){
                return 1; /* no more readable descriptors */
            }
        }

    }
    message.clear();
    return true;
}



bool hansolo_tcp::tcp_server_update_once_send_msg_big_data(std::string &message)
{

    int i = 0;
    bool re = true;

    rset = allset; /* structure assignment */
    nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
    if (nready < 0)
    {
        perr_exit("select error");
    }
    if (FD_ISSET(listenfd, &rset))
    { /* new client connection */
        cliaddr_len = sizeof(cliaddr);
        connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);

        printf("received from %s at PORT %d\n",
               inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
               ntohs(cliaddr.sin_port));

        for (i = 0; i < FD_SETSIZE; i++){
            if (client[i] < 0)
            {
                client[i] = connfd; /* save descriptor */
                break;
            }
        }

        if (i == FD_SETSIZE)
        {
            fputs("too many clients\n", stderr);
            exit(1);
        }

        FD_SET(connfd, &allset); /* add new descriptor to set */
        if (connfd > maxfd){
            maxfd = connfd; /* for select */
        }

        if (i > maxi){
            maxi = i; /* max index in client[] array */
        }


        if (--nready == 0){
            return 1; /* no more readable descriptors */
        }

    }

    for (i = 0; i <= maxi; i++)
    { /* check all clients for data */
        if ((sockfd = client[i]) < 0){
             continue;
        }
        if (FD_ISSET(sockfd, &rset))
        {
            if ((data_size = Read(sockfd, buf, MAXLINE)) == 0)
            {
                /* connection closed by client */
                Close(sockfd);
                FD_CLR(sockfd, &allset);
                client[i] = -1;
                re = false;
            }
            else
            {
                // std::cout << "-------" << std::endl;
                //先发送数据的大小 再发送实际数据 分次发送
                size_t msg_size = message.length();
                char header[sizeof(size_t)];
                memset(header,'\0',sizeof(size_t));// 八位 全部填充为 0
                memcpy(header, (char *)&msg_size, sizeof(size_t));
                //先发送包大小
                size_t error_n=Write(sockfd, header, sizeof(size_t));

                if(error_n==-2){
                    std::cout << "尝试写入关闭的管道\n";
                     /* connection closed by client */
                    Close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                    re = false;
                    continue;
                }
                //切割数据 分次发送
                size_t n = 0;
                size_t offset = 0;
                while (msg_size > 0)
                {
                    n = Write(sockfd, message.data()+offset, msg_size);
                    if(n==-2){
                        std::cout << "尝试写入关闭的管道\n";
                        /* connection closed by client */
                        Close(sockfd);
                        FD_CLR(sockfd, &allset);
                        client[i] = -1;
                        re = false;
                        break;
                    }
                    msg_size -= n;
                    offset += n;
                    re = true;
                    // std::cout << n << std::endl;
                }
            }

            if (--nready == 0){
                return re; /* no more readable descriptors */
            }
        }

    }
    message.clear();
    return re;
}




bool hansolo_tcp::tcp_server_update_once()
{
    int i = 0;
    rset = allset; /* structure assignment */
    nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
    if (nready < 0)
    {
        perr_exit("select error");
    }
    if (FD_ISSET(listenfd, &rset))
    { /* new client connection */
        cliaddr_len = sizeof(cliaddr);
        connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);

        printf("received from %s at PORT %d\n",
               inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
               ntohs(cliaddr.sin_port));

        for (i = 0; i < FD_SETSIZE; i++){
            if (client[i] < 0)
            {
                client[i] = connfd; /* save descriptor */
                break;
            }
        }

        if (i == FD_SETSIZE)
        {
            fputs("too many clients\n", stderr);
            exit(1);
        }

        FD_SET(connfd, &allset); /* add new descriptor to set */
        if (connfd > maxfd){
            maxfd = connfd; /* for select */
        }

        if (i > maxi){
            maxi = i; /* max index in client[] array */
        }


        if (--nready == 0){
            return 1; /* no more readable descriptors */
        }

    }

    for (i = 0; i <= maxi; i++)
    { /* check all clients for data */
        if ((sockfd = client[i]) < 0){
             continue;
        }
        if (FD_ISSET(sockfd, &rset))
        {
            if ((data_size = Read(sockfd, buf, MAXLINE)) == 0)
            {
                /* connection closed by client */
                Close(sockfd);
                FD_CLR(sockfd, &allset);
                client[i] = -1;
            }
            else
            {
                std::cout << "收到来自 " << ntohs(cliaddr.sin_port) << " 的消息 " << std::string(buf,data_size) << std::endl;
                int j;
                for (j = 0; j < data_size; j++){
                    buf[j] = toupper(buf[j]);
                }
                Write(sockfd, buf, data_size);
            }

            if (--nready == 0){
                return 1; /* no more readable descriptors */
            }
        }

    }

    return true;
}


/// @brief tcp客户端实现
/// @param port
/// @return
bool hansolo_tcp::init_client_tcp(int port,const char* ip)
{
    serverfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(struct sockaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if(ip==NULL){
        servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    }
    else{
        servaddr.sin_addr.s_addr = inet_addr(ip);
    }
    

    while(!Connect(serverfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr))){

    }
    // {
    return  true;
    // }

    // return false;
}
