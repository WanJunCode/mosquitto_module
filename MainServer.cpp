#include "MainServer.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string>

// 连接回调函数，当连接成功时会进入这里，可以在这里进行连接成功之后的操作，比如连接之后的信息同步
void my_connect_callback(struct mosquitto *mosq, void *obj, int rc)
{
    printf("connect cb\n");
}
// 断开连接回调函数，在断开连接之后进入
void my_disconnect_callback(struct mosquitto *mosq, void *obj, int result)
{
    printf("disconnect cb\n");
}
// 发布消息回调函数
void my_publish_callback(struct mosquitto *mosq, void *obj, int mid)
{
    printf("publish cb\n");
}

void my_log_callback(struct mosquitto *mosq, void *obj, int level, const char *str){
    printf("log callback: level[%d] str[%s]\n",level,str);
    
}

void my_subscribe_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    std::string message((char *)(msg->payload),msg->payloadlen);
    printf("subscribe cb:[%s]\n",message.c_str());
}

MainServer::MainServer()
    :mqtt_(NULL){
    base_ = event_base_new();
    init();
}

MainServer::~MainServer(){
    printf("析构函数...\n");
    if(mqtt_ != NULL){
        mosquitto_loop_stop(mqtt_,true);
        mosquitto_destroy(mqtt_);
    }
    event_free(stdin_);
    event_base_free(base_);
}

void
MainServer::service_loop(){
    stdin_ = event_new(base_, STDIN_FILENO, EV_READ | EV_PERSIST, stdinCB, this);
    event_add(stdin_, NULL);
    event_base_dispatch(base_);
}

void MainServer::init(){
    mqtt_ = mosquitto_new("wanjun",true,NULL);

    mosquitto_connect_callback_set(mqtt_, my_connect_callback);
    mosquitto_disconnect_callback_set(mqtt_, my_disconnect_callback);
    mosquitto_publish_callback_set(mqtt_, my_publish_callback);
    mosquitto_message_callback_set(mqtt_, my_subscribe_callback);
    mosquitto_log_callback_set(mqtt_,my_log_callback);

    int err = mosquitto_connect(mqtt_,"localhost",1883,60);
    if(err == MOSQ_ERR_SUCCESS){
        printf("mosquitto connect successful\n");
    }

    mosquitto_subscribe(mqtt_,NULL,"wanjun_topic",0);
    
    // 非阻塞式的循环
    err = mosquitto_loop_start(mqtt_);
    if(err == MOSQ_ERR_SUCCESS){
        printf("mosquitto loop start successful\n");
    }
}

// STATIC
void
MainServer::stdinCB(evutil_socket_t fd, short what, void *arg){
    MainServer *server = (MainServer*)arg;

    char recvline[80];
    bzero(recvline,sizeof(recvline));
    ssize_t size = read(fd, recvline, sizeof(recvline));
    recvline[size-1] = '\0';
    printf("you have input cmd : [%s] \n", recvline);
    if( strncmp(recvline,"over",4) == 0){
        event_base_loopbreak(server->base_);
    }
}
