#include <stdio.h>
#include <mosquittopp.h>
#include <mosquitto.h>
#include <iostream>
#include <string.h>

void mosquittopp_test(){
    mosqpp::mosquittopp client("wanjun");
    client.connect("localhost");
    client.disconnect();
}

void mosquitto_test(){
    if(MOSQ_ERR_SUCCESS != mosquitto_lib_init()){
        goto fail;
    }else{
        printf("libarary init successful\n");
        mosquitto *client = mosquitto_new("wanjun",true,NULL);
        if(client == NULL){
            goto fail;
        }else{
            printf("mosquitto new successful\n");
        }

        const char *will = "hello this is will";
        // 设置 遗言
        int err = mosquitto_will_set(client,"wanjun_topic",strlen(will),will,1,false);
        if(err == MOSQ_ERR_SUCCESS){
            printf("mosquitto will set successful\n");
        }
        // 设置 账号 密码
        // mosquitto_username_pw_set
        err = mosquitto_connect(client,"localhost",1883,60);
        if(err == MOSQ_ERR_SUCCESS){
            printf("mosquitto connect successful\n");
        }

        int messageId = 0;
        const char *message = "hello world";
        mosquitto_publish(client,&messageId,"wanjun_topic",strlen(message),message,1,true);

        mosquitto_disconnect(client);
        mosquitto_destroy(client);
        mosquitto_lib_cleanup();
        return;
    }
    
fail:
    printf("something wrong\n");    
}

int main(int argc,char *argv[]){
    printf("begin\n");
    mosquitto_test();

    return 0;
}