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
        // 设置 遗言
        int err = mosquitto_will_set(client,"wanjun_topic",strlen("hello"),"hello",1,true);
        if(err == MOSQ_ERR_SUCCESS){
            printf("mosquitto will set successful\n");
        }

        if(client == NULL){
            goto fail;
        }else{
            printf("mosquitto new successful\n");


        }
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