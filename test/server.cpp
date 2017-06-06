/*======================================================
    > File Name: main.cpp
    > Author: lyh
    > E-mail:  
    > Other :  
    > Created Time: 2016年11月20日 星期日 16时14分29秒
 =======================================================*/

#include<iostream>
#include<map>
#include<string>
#include<queue>
#include"../tcpserver.cpp"
#include"../servant.h"
#include"../buffer.h"

std::map<std::string,lnet::Servant*>  servantList;
std::queue<std::string>               match_queue;

typedef struct
{
    int msgtype;
    char name[20];
    char passwd[20];
    char opponent[20];
    int type;
    int x;
    int y;
    int play_flag;
}Message;

void copyStr(char* s1,std::string& s2)
{  
    std::copy(s2.begin(),s2.end(),s1);
    s1[s2.size()] = '\0';
}  

class TestServant : public lnet::Servant
{
public:
    void doRequest(const char *req)
    {
        Message *msg;
        msg = (Message*)req;
        std::cout << msg->msgtype << std::endl;
        switch(msg->msgtype)
        {
            //Login
            case 0:
                Login(msg);
                break;
            //Match
            case 1:
                matchOpponent(msg);
                break;
            //WalkChess    
            case 2:
                walkChess(msg);
                break;
            default:
                ;
        }
    }
private:
    void sendMessage(const char* buf,int len)
    {
        lnet::Buffer send_buf(1024);
        send_buf.writeBuffer((const char*)&len,sizeof(int));
        send_buf.writeBuffer((const char*)buf,len);
        this->sendResponse(send_buf.begin(),sizeof(int)+len);
    }
    void sendMessageOther(lnet::Servant* servant,
                const char* buf,
                int len)
    {
        lnet::Buffer send_buf(1024);
        send_buf.writeBuffer((const char*)&len,sizeof(int));
        send_buf.writeBuffer((const char*)buf,len);
        this->sendResponseOther(servant,
                    send_buf.begin(),
                    sizeof(int)+len);
    }
    void Login(Message* msg)
    {
        LOG_DEBUG("name:" + std::string(msg->name));
        LOG_DEBUG("passwd:" + std::string(msg->passwd));
        char buf[] = "success";
        servantList.insert(
                    std::pair<std::string,lnet::Servant*>(
                        std::string(msg->name),this));
        sendMessage(buf,strlen(buf));
    }
    void matchOpponent(Message* msg)
    {
        if(match_queue.size() < 1)
        {
            match_queue.push(std::string(msg->name));
            return;
        }
        std::string name1 = match_queue.front();
        match_queue.pop();

        lnet::Servant* servant1 = servantList[name1];

        Message msg1,msg2;
        msg1.play_flag = 0;
        std::string name2(msg->name);
        copyStr(msg1.opponent,name2);
        msg2.play_flag = 1;
        copyStr(msg2.opponent,name1);
        
        sendMessage((const char*)&msg2,sizeof(msg2));
        sendMessageOther(servant1,(const char*)&msg1,sizeof(msg1));
    }
    void walkChess(Message* msg)
    {
        lnet::Servant* servant = servantList[
            std::string(msg->opponent)];
        sendMessageOther(servant,(const char*)msg,sizeof(Message));
    }
};

int main()
{
    lnet::TcpServer<TestServant>  s(10000,"127.0.0.1");
    s.start(1);
    return 0;
}
