#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "mysql.h"
#include <list>
#include <unordered_map>

using namespace std;
list<int> clients;
unordered_map<string,int> name2sock;
unordered_map<int,string> sock2name;

MYSQL *mysql=NULL;

pthread_rwlock_t rwlockClient,rwlockTrans;

void*service(void*);

int main()
{
    mysql=new MYSQL;
	mysql = mysql_init(mysql);

    //连接存放名称，密码的数据库
	if(!mysql_real_connect(mysql, "localhost", "root", "1314", "clients", 0, NULL, 0))/*针对的是本地数据库且无密码的情况*/
	{
		cout << "Failed to connect to database. ERROR: "<<mysql_error(mysql) <<endl;
		exit(1);
	}
    cout << "mysql conenct success" << endl;

    //初始化两个读写锁
    pthread_rwlock_init(&rwlockClient,NULL);
    pthread_rwlock_init(&rwlockTrans,NULL);
    int serverSock=0,clientSock=0;
    if((serverSock=socket(AF_INET,SOCK_STREAM,0))<0){
        cout<<"Can not open socket."<<endl;
        exit(0);
    }

    //将服务器端口绑定到8000端口上
    sockaddr_in serverAddr,clientAddr;
    serverAddr.sin_addr.s_addr=INADDR_ANY;
    serverAddr.sin_family=PF_INET;
    serverAddr.sin_port=htons(8000);
    if(bind(serverSock,(sockaddr*)&serverAddr,sizeof(sockaddr))!=0){
        cout<<"Bind error."<<endl;
        exit(0);
    }

    //开始监听
    if(listen(serverSock,10)!=0){
        cout<<"set Listen error."<<endl;
        exit(0);
    }
    cout<<"Waiting for connection....."<<endl;

    //主线程中不断接受新的连接请求，并为每一个连接请求建立一线程
    socklen_t len=0;
    while(true){
        if((clientSock=accept(serverSock,(sockaddr*)(&clientAddr),&len))<0){
            cout<<"Accept error."<<endl;
        }
        else{
            pthread_t thread;
            pthread_create(&thread,NULL,service,(void*)&clientSock);
        }
    }

    //销毁读写锁
    pthread_rwlock_destroy(&rwlockClient);
    pthread_rwlock_destroy(&rwlockTrans);
    //关闭socket
    close(serverSock);
    //关闭数据库
    mysql_close(mysql);

    return 0;
}

/** \brief
 *1，如果账号存在，验证密码的合法性
 *2.账号不存在，以账号和密码新建一个账号
 * \param name：用户名
 * \param password：用户密码
 * \return bool,验证的结果正确与否
 *
 */

bool verify(string&name,string&password){
    //进行查询
    string cmd="SELECT password FROM clients WHERE name='"+name+"';";
    if(mysql_query(mysql,cmd.c_str())!=0){
        cout<<"Query databases failed."<<endl;
        return false;
    }

    MYSQL_RES*res;
    res=mysql_store_result(mysql);

    int row=mysql_num_rows(res);
    //如果查询的结果为空，说明数据库中不存在这样的账户，则新建一个账户
    if(row==0){

        cmd="INSERT INTO clients";
        cmd+=" VALUES(NULL,";
        cmd+="'"+name+"',";
        cmd+="'"+password+"'";
        cmd+=");";
        //新建账户成功与否
        if(mysql_query(mysql,cmd.c_str())!=0){
            cout<<"Can not add a new client,name:"<<name<<endl;
            return false;
        }
        else{
            cout<<"Add a new client,name:"<<name<<endl;
            return true;
        }

    }
    else{
        string token;
        //获取查询到的那一行
        MYSQL_ROW data=mysql_fetch_row(res);
        int len=*mysql_fetch_lengths(res);
        copy(data[0],data[0]+len,back_inserter(token));

        //验证密码
        if(token==password){
            return true;
        }
        else{
            return false;
        }
    }

    return true;
}

/** \brief 某一用户主动退出或者异常退出之后请清理函数
 *
 * \param sock：要清理的用户的socket号
 * \param
 * \return void
 *
 */

void disconnect(int sock){
    list<int>::iterator target;
    //直接删除会出错
    for(auto iter=clients.begin();iter!=clients.end();iter++){
        if(*iter==sock){
            target=iter;
            break;
        }
    }

    //要秀给全局变量，要获取写锁
    pthread_rwlock_wrlock(&rwlockClient);
    pthread_rwlock_wrlock(&rwlockTrans);
    //清理当前用户的信息
    clients.erase(target);
    name2sock.erase(sock2name[sock]);
    sock2name.erase(sock);
    pthread_rwlock_unlock(&rwlockClient);
    pthread_rwlock_unlock(&rwlockTrans);
}

/** \brief 群发消息函数
 *
 * \param src:消息发送者
 * \param msg：要发送的消息（已包含发送者姓名）
 * \return void
 *
 */

void sendToAll(int src,string&msg){
    char buff[BUFSIZ];
    copy(msg.begin(),msg.end(),buff);

    //要获取读锁
    pthread_rwlock_wrlock(&rwlockClient);
    for(int des:clients){
        //不向发送者群发消息
        if(des!=src){
            if(send(des,buff,msg.size(),0)<=0){
                cout<<"client "<<des<<"failed,abort."<<endl;
            }
        }
    }
    pthread_rwlock_unlock(&rwlockClient);
}

/** \brief 单发消息给某个人
 *
 * \param someone：消息发送对象
 * \param msg：要发送的消息（已经包含消息发送者姓名）
 * \return void
 *
 */

void sendToSomeone(int someone,string&msg){
    char buff[BUFSIZ];
    copy(msg.begin(),msg.end(),buff);
    if(send(someone,buff,msg.size(),0)<=0){
        cout<<"Client "<<someone<<"failed."<<endl;
    }
}

/** \brief 服务函数，为每一个客户进行服务
 *
 * \param sock：要服务的对象的套接字
 * \param
 * \return
 *
 */


void*service(void*sock){
    const int client=*(int*)sock;
    string str,name,password;
    char buff[BUFSIZ];

    //首先接收客户端发过来的用户名和密码
    int len=0;
    if((len=recv(client,buff,BUFSIZ,0))<=0){
        cout<<"Client "<<client<<" failed."<<endl;
        disconnect(client);
    }
    copy(buff,buff+len,back_inserter(str));
    int pos=str.find(":");
    name=str.substr(0,pos);
    password=str.substr(pos+1);

    //验证密码
    if(verify(name,password)==true){
        //为新来的用户添加记录，先获取写锁，注意预防死锁
        pthread_rwlock_wrlock(&rwlockClient);
        pthread_rwlock_wrlock(&rwlockTrans);
        clients.push_back(client);
        name2sock[name]=client;
        sock2name[client]=name;
        pthread_rwlock_unlock(&rwlockClient);
        pthread_rwlock_unlock(&rwlockTrans);

        //提示用户密码验证成功
        string msg="OK";
        sendToSomeone(client,msg);

        //群发消息，提醒有新用户上线了
        msg=name+" now online.";
        cout<<msg<<endl;
        sendToAll(client,msg);
    }
    else{
        //验证失败，提醒用户
        string msg="Fail";
        sendToSomeone(client,msg);
        return NULL;
    }

    //不断接收用户发来的信息，根据不同参数群发，单发消息，或者退出服务
    while(true){
        string msg;
        if((len=recv(client,buff,BUFSIZ,0))<=0){
            cout<<"Client "<<client<<" failed."<<endl;
            break;
        }

        copy(buff,buff+len,back_inserter(msg));
        if(msg=="quit"){
            msg=name+" "+"quit";
            cout<<msg<<endl;
            sendToAll(client,msg);
            break;
        }

        int pos=msg.find(":");
        string des=msg.substr(0,pos);
        msg=name+":"+msg.substr(pos+1);

        //第一个命令为0发送给指定人员
        if(des=="all"){
            sendToAll(client,msg);
        }
        else{
            sendToSomeone(name2sock[des],msg);
        }
    }

    disconnect(client);
    return NULL;
}






























