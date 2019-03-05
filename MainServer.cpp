#include "MainServer.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void message(struct mosquitto *, void *, const struct mosquitto_message *){
    printf("message...\n");
}

MainServer::MainServer()
    :mqttpp_(NULL){
    mosquitto_lib_init();
    base_ = event_base_new();
    initMqtt();
    init(mqtt_);
}

MainServer::~MainServer(){
    printf("析构函数...\n");
    delete mqttpp_;
    event_free(stdin_);
    event_base_free(base_);
    mosquitto_destroy(mqtt_);
    mosquitto_lib_cleanup();
}

void
MainServer::service_loop(){
    stdin_ = event_new(base_, STDIN_FILENO, EV_READ | EV_PERSIST, stdinCB, this);
    event_add(stdin_, NULL);
    event_base_dispatch(base_);
}

void MainServer::initMqtt(){
    mqttpp_ = new TMqttWrapper("wan",true);
    mqttpp_->connect("wanjun","","localhost");
    mqttpp_->subscribe_topic("wanjun");
}

void MainServer::init(mosquitto *client){
    client = mosquitto_new("wanjun",true,NULL);
    mosquitto_message_callback_set(client,message);
    int err = mosquitto_connect_async(client,"localhost",1883,60);
    if(err == MOSQ_ERR_SUCCESS){
        printf("mosquitto connect successful\n");
    }

    err = mosquitto_disconnect(client);
    if(err == MOSQ_ERR_SUCCESS){
        printf("mosquitto disconnect successful\n");
    }
    mosquitto_subscribe(client,NULL,"wanjun_topic",0);
    
    // 非阻塞式的循环
    err = mosquitto_loop_start(client);
    if(err == MOSQ_ERR_SUCCESS){
        printf("mosquitto loop start successful\n");
    }
    // 阻塞式的循环
    // mosquitto_loop_forever(client,0,-1);

    printf("after loop\n");
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
