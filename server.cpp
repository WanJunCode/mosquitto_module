#include <stdio.h>
#include <mosquittopp.h>
#include <mosquitto.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

#define HOST "localhost"
#define PORT  1883
#define KEEP_ALIVE 60

bool session = true;

// this is ok in linux
#define ngx_atomic_cmp_set(lock, old, set)                                    \
    __sync_bool_compare_and_swap(lock, old, set)

#define ngx_atomic_fetch_add(value, add)                                      \
    __sync_fetch_and_add(value, add)


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

// mosquitto server
const char *myTopic = "wanjun_topic";
const char *serverName = "shengming1hao";


void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    if(message->payloadlen){
        printf("[%s] [%s]", message->topic, (char *)message->payload);
        if( strncmp(reinterpret_cast<char *>(message->payload),"quit",4) == 0){
            printf("quit from server\n");
            mosquitto_disconnect(mosq);
        }
    }else{
        printf("%s (null)\n", message->topic);
    }
    fflush(stdout);
}

void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    int i;
    if(!result){
        /* Subscribe to broker information topics on successful connect. */
        mosquitto_subscribe(mosq, NULL, myTopic, 2);
    }else{
        fprintf(stderr, "Connect failed\n");
    }
}

void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
    int i;
    printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
    for(i=1; i<qos_count; i++){
        printf(", %d", granted_qos[i]);
    }
    printf("\n");
}

void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
    /* Pring all log messages regardless of level. */
    // 打印所有的 message 信息，忽略等级
    printf("%s\n", str);
}

int server(){
    //libmosquitto 库初始化
    mosquitto_lib_init();

    struct mosquitto *mosq = NULL;
    //创建mosquitto客户端
    mosq = mosquitto_new(serverName,session,NULL);
    if(!mosq){
        printf("create client failed..\n");
        mosquitto_lib_cleanup();
        return 1;
    }
    //设置回调函数，需要时可使用
    mosquitto_log_callback_set(mosq, my_log_callback);
    mosquitto_connect_callback_set(mosq, my_connect_callback);
    mosquitto_message_callback_set(mosq, my_message_callback);
    mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);
    
    //客户端连接服务器
    if(mosquitto_connect(mosq, HOST, PORT, KEEP_ALIVE)){
        fprintf(stderr, "Unable to connect.\n");
        return 1;
    }
    
    //循环处理网络消息
    mosquitto_loop_forever(mosq, -1, 1);

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    printf("server end\n");
    return 0;
}

int main(int argc,char *argv[]){
    // mosquitto_test();
    server();
    return 0;
}