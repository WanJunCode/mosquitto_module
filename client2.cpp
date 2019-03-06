/* This provides a crude manner of testing the performance of a broker in messages/s. */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
// mosquitto头文件，可以进去看看里面的函数方法
#include <mosquitto.h>


#define HOST "localhost"
#define PORT 1883

size_t MESSAGE_SIZE = 100;
size_t MESSAGE_COUNT = 10;
size_t mid = 0;

static bool run = true;
static int message_count = 0;
static struct timeval start, stop;
// 连接回调函数，当连接成功时会进入这里，可以在这里进行连接成功之后的操作，比如连接之后的信息同步
void my_connect_callback(struct mosquitto *mosq, void *obj, int rc)
{
        printf("rc: %d\n", rc);
        gettimeofday(&start, NULL);
}
// 断开连接回调函数，在断开连接之后进入
void my_disconnect_callback(struct mosquitto *mosq, void *obj, int result)
{
        run = false;
}
// 发布消息回调函数
void my_publish_callback(struct mosquitto *mosq, void *obj, int mid)
{
        message_count++;
        if(message_count == MESSAGE_COUNT){
                gettimeofday(&stop, NULL);
                mosquitto_disconnect((struct mosquitto *)obj);
        }
}

void my_log_callback(struct mosquitto *, void *, int, const char *){
    printf("log\n");
}

// 订阅消息回调
void my_subscribe_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    int i;
    printf("Subscribed (mid: %d)\n", mid);
}

int main(int argc, char *argv[])
{
    struct mosquitto *mosq;
    int i;
    double dstart, dstop, diff;
    uint8_t *buf;

    buf = (uint8_t *)malloc(MESSAGE_SIZE*MESSAGE_COUNT);
    if(!buf){
        printf("Error: Out of memory.\n");
        return 1;
    }

    start.tv_sec = 0;
    start.tv_usec = 0;
    stop.tv_sec = 0;
    stop.tv_usec = 0;

    mosquitto_lib_init();
    // 定义一个客户端名为perftest的发布端。客户端表示发布端的唯一性
    mosq = mosquitto_new("perftest", true, NULL);
    mosquitto_connect_callback_set(mosq, my_connect_callback);
    mosquitto_disconnect_callback_set(mosq, my_disconnect_callback);
    // 发布回调
    mosquitto_publish_callback_set(mosq, my_publish_callback);
    //订阅回调
    mosquitto_message_callback_set(mosq, my_subscribe_callback);
    mosquitto_log_callback_set(mosq,my_log_callback);
    // 连接服务端
    mosquitto_connect(mosq, HOST, PORT, 600);

    printf("before loop start\n");
    mosquitto_loop_start(mosq);
    printf("after loop start\n");

    i=0;
    for(i=0; i<MESSAGE_COUNT; i++){
        mosquitto_publish(mosq, NULL, "perf/test", MESSAGE_SIZE, &buf[i*MESSAGE_SIZE], 0, false);
    }

    printf("before loop stop\n");
    mosquitto_loop_stop(mosq, false);

    dstart = (double)start.tv_sec*1.0e6 + (double)start.tv_usec;
    dstop = (double)stop.tv_sec*1.0e6 + (double)stop.tv_usec;
    diff = (dstop-dstart)/1.0e6;

    printf("Start: %g\nStop: %g\nDiff: %g\nMessages/s: %g\n", dstart, dstop, diff, (double)MESSAGE_COUNT/diff);

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}