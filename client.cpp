#include <mosquitto.h>
#include <mosquittopp.h>
#include <stdio.h>
#include "MainServer.h"

void mosquitto_client(){
    // mosquitto_lib_init();
    // mosquitto *client = mosquitto_new("wanjun",true,NULL);
    // int err = mosquitto_connect(client,"localhost",1883,60);
    // if(err == MOSQ_ERR_SUCCESS){
    //     printf("mosquitto connect successful\n");
    // }


    // err = mosquitto_disconnect(client);
    // if(err == MOSQ_ERR_SUCCESS){
    //     printf("mosquitto disconnect successful\n");
    // }
    // mosquitto_subscribe(client,NULL,"wanjun_topic",0);
    
    // // 非阻塞式的循环
    // // mosquitto_loop(client,0,1);
    // // 阻塞式的循环
    // mosquitto_loop_forever(client,0,-1);


    // printf("after loop\n");
    // mosquitto_destroy(client);
    // mosquitto_lib_cleanup();
}

int main(int argc,const char *argv[]){

    mosquitto_lib_init();
    printf("mosquitto client begin\n");
    // mosquitto_client();

    MainServer& server = MainServer::getInstance();

    server.service_loop();
    
    mosquitto_lib_cleanup();
    return 0;
}