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

using namespace std;
void *send(void *);
void *recive(void *);

int main(){
    int clientSock=0;
    if((clientSock=socket(AF_INET,SOCK_STREAM,0))<0){
        cout<<"Can not open socket."<<endl;
        exit(0);
    }

    sockaddr_in serverAddr;
    serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(8000);

    int val=0;
    if((val=connect(clientSock,(sockaddr*)&serverAddr,sizeof(sockaddr)))!=0){
        cout<<"Can not connect to server."<<endl;
        exit(0);
    }
    cout<<"Connect to server successfully."<<endl;

    cout<<"Enter your name and password."<<endl;
    string name,password;
    cin>>name>>password;
    name+=":"+password;
    char buff[BUFSIZ];

    copy(name.begin(),name.end(),buff);
    if(send(clientSock,buff,name.size(),0)==0){
        cout<<"Send error."<<endl;
        exit(0);
    }

    /*接收服务器的验证信息*/
    int len=0;
    if((len=recv(clientSock,buff,BUFSIZ,0))==0){
        cout<<"recive error."<<endl;
        exit(0);
    }
    string status;copy(buff,buff+len,back_inserter(status));
    if(status=="OK"){
        cout<<"Authentication OK."<<endl;
    }
    else{
        cout<<"Authentication failed."<<endl;
        exit(0);
    }


    pthread_t read=0,write=0;
    if(pthread_create(&read,NULL,send,(void*)&clientSock)!=0){
        cout<<"System error."<<endl;
        exit(0);
    }
    if(pthread_create(&write,NULL,recive,(void*)&clientSock)!=0){
        cout<<"System error."<<endl;
        exit(0);
    }

    pthread_join(read,NULL);
    close(clientSock);
    return 0;
}

void *send(void *ptr){
    const int clientSock=*(int*)ptr;
    char buff[BUFSIZ];

    while(true){
        string input;
        cin>>input;

        copy(input.begin(),input.end(),buff);
        if(send(clientSock,buff,input.size(),0)==-1){
            cout<<"send error."<<endl;
            return NULL;
        }
        if(input=="quit"){
            return NULL;
        }
    }
}
void *recive(void *ptr){
    const int clientSock=*(int*)ptr;
    char buff[BUFSIZ];

    while(true){
        int len=0;
        string msg;
        if((len=recv(clientSock,buff,BUFSIZ,0))==-1){
            cout<<"recive error."<<endl;
            return NULL;
        }
        copy(buff,buff+len,back_inserter(msg));
        cout<<msg<<endl;
    }
}
