#include "hansolo_tcp.h"


hansolo_tcp::hansolo_tcp()
{
}
hansolo_tcp::~hansolo_tcp()
{
}


bool hansolo_tcp::init_server_tcp(int port)
{
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
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
bool hansolo_tcp::init_client_tcp(int port)
{
    serverfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(struct sockaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    while(!Connect(serverfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr))){

    }
    // {
    return  true;
    // }

    // return false;
}
