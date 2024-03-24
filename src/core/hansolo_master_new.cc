#include"hansolo_master_new.h"
#include<iostream>



ServerUnaryReactor* hansolo_master::SayRegist(CallbackServerContext*context,const RegistRequest*request,RegistReply*reply)
{

    hDebug(Color::FG_GREEN)  << "节点 " << request->nodename() << " 申请注册";

    reply->set_nodename(request->nodename());

    //记录
    bool alreadyHasSameNodeName = false;
    bool isReserve = false;
    for (int i = 0; i < m_all_nodes.size(); i++)
    {
        //目前已经有注册相同名字的节点
        if(m_all_nodes[i].node_name==request->nodename()&&m_all_nodes[i].status==ONLINE){
            alreadyHasSameNodeName = true;
            break;
        }
        //节点数据 是否被保留
        else if(m_all_nodes[i].node_name==request->nodename()&&m_all_nodes[i].status==RESERVE){
            isReserve = true;
        }
    }
    if(alreadyHasSameNodeName){
        reply->set_registstatus(requestStatus::RegistNodeAlreadyHaveSameNodeName);
        hDebug(Color::FG_RED) << "已存在相同名称的节点! " << request->nodename() << '\n';
    }
    else if(isReserve){
        hDebug(Color::FG_RED) << "恢复保留的节点! " << request->nodename() << '\n';
        reply->set_registstatus(requestStatus::RegistNodeSuccess);
    }
    else{
        node_items node;
        node.status = ONLINE;
        node.node_name = request->nodename();
        m_all_nodes.push_back(node);
        reply->set_registstatus(requestStatus::RegistNodeSuccess);
        hDebug(Color::FG_GREEN) << "节点注册成功";
    }


    ServerUnaryReactor *reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
}

ServerUnaryReactor* hansolo_master::RegistePublisher(CallbackServerContext*context,const RegistePublisherRequest *request, RegistePublisherReply *reply){
    reply->set_nodename(request->nodename());
    reply->set_topicname(request->topicname());
    

    //上线 首先查找 是否离线时 有否订阅者为自己注册了端口
    bool preRegist = false;
    for (int i = 0; i < m_all_nodes.size(); i++)
    {
        //发现有节点名字长度为0（之前订阅者干的）
        std::vector<hansolo_master::node_items>::iterator find;
        if (m_all_nodes[i].node_name.length() <= 0)
        {
            for(auto pb_tps: m_all_nodes[i].publish_topics){
                //发现存在话题名
                if(pb_tps.published_topic_name==request->topicname() ){
                     //先将自己之前注册过的节点名字删了
                    find=std::find_if(m_all_nodes.begin(), m_all_nodes.end(), [=](const hansolo_master::node_items &node) {
                        return node.node_name == request->nodename();
                    });
                    reply->set_port(pb_tps.self_port);
                    hDebug(Color::FG_GREEN) << "申请端口为: " << pb_tps.self_port;
                    //填充之前的记录
                    m_all_nodes[i].node_name = request->nodename();
                    m_all_nodes[i].status = ONLINE;
                    preRegist = true;
                    if(find!=m_all_nodes.end()){
                        hDebug(Color::FG_RED)
                        << "发现之前有subscriber帮自己注册过了 删除自己注册过的，填充完成之前帮忙的注册的记录 ";
                        m_all_nodes.erase(find);
                    }
                    hDebug(Color::FG_GREEN) << "发布者: " << request->nodename() << "<-->"<< " 订阅者 " << "???" << " 话题名称: " << request->topicname();
                    break;
                }
            }

            break;
        }
    }

    //记录节点的申请发布的话题名称和端口
    if(!preRegist){
        for (int i = 0; i < m_all_nodes.size(); i++)
        {
            if (m_all_nodes[i].node_name == request->nodename()&&m_all_nodes[i].status==ONLINE)
            {
                node_publish_item pr{request->topicname(), m_port};
                m_all_nodes[i].publish_topics.push_back(pr);
                reply->set_port(m_port);
                m_port++;
                break;
            }
            else if(m_all_nodes[i].node_name == request->nodename()&&m_all_nodes[i].status==RESERVE)
            {
                // m_all_nodes[i].status = ONLINE;
                auto find = std::find_if(m_all_nodes[i].publish_topics.begin(), m_all_nodes[i].publish_topics.end(), [=](const node_publish_item &item) {
                    return item.published_topic_name==request->topicname();
                });
                if(find!=m_all_nodes[i].publish_topics.end()){
                    reply->set_port(find->self_port);
                }
                break;
            }
        }
        
        hDebug(Color::FG_GREEN) << "节点 " << request->nodename() << " 申请注册 publisher端口";
    }

    ServerUnaryReactor *reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
}


ServerUnaryReactor *hansolo_master::RegisteSubscriber(CallbackServerContext *context,
                                          const RegisteSubscriberRequest *request,
                                          RegisteSubscriberReply *reply)
{
    bool isFoundTopic = false;
    //查询当前已注册的publisher是否有 这个请求订阅的话题名称
    for (int i = 0; i < m_all_nodes.size();i++){
        for (int j = 0; j < m_all_nodes[i].publish_topics.size();j++){
            if(m_all_nodes[i].publish_topics[j].published_topic_name==request->topicname()){
                reply->set_port(m_all_nodes[i].publish_topics[j].self_port);
                reply->set_targetnodename(m_all_nodes[i].node_name);
                reply->set_topicname(m_all_nodes[i].publish_topics[j].published_topic_name);

                //将订阅的话题 存入申请者的列表中
                node_subscribe_item newItem;
                for (int m = 0; m < m_all_nodes.size();m++){
                    if(m_all_nodes[m].node_name==request->nodename()){
                        newItem.subscribed_topic_name = m_all_nodes[i].publish_topics[j].published_topic_name;
                        newItem.subscribed_node_name = m_all_nodes[i].node_name;
                        m_all_nodes[m].subscribe_topics.push_back(newItem);
                        break;
                    }
                }
                hDebug(Color::FG_GREEN) << "发布者: " << newItem.subscribed_node_name << "<-->"
                                        << " 订阅者 " << request->nodename() << " 话题名称: " << request->topicname();
                isFoundTopic = true;
                break;
            }
        }
    }
    if(!isFoundTopic){
        reply->set_registstatus(requestStatus::RegisteSubscribeTargetOffline);
        //给未来上线的server 提前注册一个端口ip
        node_subscribe_item newItem;
        newItem.subscribed_node_name = "";
        newItem.subscribed_topic_name = request->topicname();
        for (int i = 0; i < m_all_nodes.size();i++){
            if(m_all_nodes[i].node_name==request->nodename()){
                m_all_nodes[i].subscribe_topics.push_back(newItem);
                break;
            }
        }

        reply->set_port(m_port);
        node_publish_item pr{request->topicname(), m_port};
        node_items node;
        //名字设为空
        node.node_name ={};
        node.status = OFFLINE;
        node.publish_topics.push_back(pr);
        m_all_nodes.push_back(node);
        hDebug(Color::FG_RED) << "不存在该话题名称 现为未来的该话题发布者建立档案 发布者 '' 端口: " << m_port;
        m_port++;
    }
    else{
        reply->set_registstatus(requestStatus::RegisteSubscribeSuccess);
    }
    reply->set_nodename(request->nodename());


    ServerUnaryReactor *reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
}


ServerUnaryReactor *hansolo_master::RegistOffline(CallbackServerContext *context,
                                         const OfflineRequest *request,
                                         OfflineReply *reply)
{
    reply->set_nodename(request->nodename());
    reply->set_status(true);

    auto find = std::find_if(m_all_nodes.begin(), m_all_nodes.end(), [=](const node_items &items)                    {
        return items.node_name==request->nodename();
        });
    
    //如果当前 要退出的节点 还有 发布的话题 就不能直接清空
    if(find!=m_all_nodes.end()){
        if(find->publish_topics.size()==0){
            hDebug(Color::FG_GREEN) << "节点 " << request->nodename() << " 下线";
            m_all_nodes.erase(find);
        }
        else{
            //节点数据 保留
            find->subscribe_topics.clear();
            find->status = RESERVE;
        }
    }

    ServerUnaryReactor *reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
}

ServerUnaryReactor *hansolo_master::GetTopics(CallbackServerContext *context,
                                  const requestTopics *request,
                                  replyTopics *reply)
{

    
    for (int i = 0; i < m_all_nodes.size();i++){
        for (int j = 0; j < m_all_nodes[i].publish_topics.size();j++){
            hansolo::singleTopic *topic = reply->add_alltopics();
            topic->set_topicname(m_all_nodes[i].publish_topics[j].published_topic_name);
            
        }
    }

    ServerUnaryReactor *reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
}





 ServerUnaryReactor *hansolo_master::EchoTopic(CallbackServerContext *context,
                                                  const requestEchoTopic *request,
                                                  replyEchoTopic *reply)
{
    //首先注册记录 hansolotopic 的节点名称
    // auto find = std::find_if(all_hansoloTopic_nodes.begin(), all_hansoloTopic_nodes.end(), [=](const std::string &item){ 
    //     return item == request->nodename();
    // });
    // //发现重名的hansolotopic
    // if(find!=all_hansoloTopic_nodes.end()){
    //     std::string newname = "hansoloTopic_" + std::to_string(hansoloTopicNums);
    //     reply->set_nodename(newname);
    //     hansoloTopicNums++;
    // }
    // else{
    //     reply->set_nodename("");
    // }
    bool findTopic = false;
    for (int i = 0; i < m_all_nodes.size(); i++)
    {
        for(auto pub:m_all_nodes[i].publish_topics){
               if(pub.published_topic_name==request->topicname()){
                   reply->set_port(pub.self_port);
                   findTopic = true;
                   break;
               }
        }
    }
    if(findTopic){
        reply->set_status(true);
    }
    else{
        reply->set_status(false);
    }

    ServerUnaryReactor *reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
}

ServerUnaryReactor *hansolo_master::StopEchoTopic(CallbackServerContext *context,
                                                const requestStopEchoTopic *request,
                                                replyStopEchoTopic *reply)
{
    ServerUnaryReactor *reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
}