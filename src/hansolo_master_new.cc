#include"hansolo_master_new.h"
#include<iostream>



ServerUnaryReactor* hansolo_master::SayRegist(CallbackServerContext*context,const RegistRequest*request,RegistReply*reply)
{

    hDebug(Color::FG_GREEN)  << "节点 " << request->nodename() << " 申请注册";

    reply->set_nodename(request->nodename());

    //记录
    bool alreadyHasSameNodeName = false;
    for (int i = 0; i < m_all_nodes.size(); i++)
    {
        //目前已经有注册相同名字的节点
        if(m_all_nodes[i].node_name==request->nodename()){
            alreadyHasSameNodeName = true;
            break;
        }
    }
    if(alreadyHasSameNodeName){
        reply->set_registstatus(requestStatus::RegistNodeAlreadyHaveSameNodeName);
        hDebug(Color::FG_RED) << "已存在相同名称的节点! " << request->nodename() << '\n';
    }
    else{
        node_items node;
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
    reply->set_port(m_port);

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



            // hDebug(Color::FG_GREEN) << "发布者: " << request->nodename() << "<-->"
            //                         << " 订阅者 " <<' ' << " 话题名称: " << request->topicname();
            break;
        }
    }

    //记录节点的申请发布的话题名称和端口
    if(!preRegist){
        for (int i = 0; i < m_all_nodes.size(); i++)
        {
            if (m_all_nodes[i].node_name == request->nodename())
            {
                node_publish_item pr{request->topicname(), m_port};
                m_all_nodes[i].publish_topics.push_back(pr);
                break;
            }
        }
        m_port++;
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

    auto find= std::find_if(m_all_nodes.begin(), m_all_nodes.end(), [=](const node_items &items) {
        if(items.node_name==request->nodename())
        {
            return true;
        }
        else{
            return false;
        }
    });
    if(find!=m_all_nodes.end()){
        hDebug(Color::FG_GREEN) << "节点 " << request->nodename() << " 下线";
        m_all_nodes.erase(find);
    }

    ServerUnaryReactor *reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
}
