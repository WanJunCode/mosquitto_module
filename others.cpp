
// #include <errno.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <stdint.h>


// #include <mosquitto.h>

// static char **topics = NULL;
// static int topic_count = 0;
// static int topic_qos = 0;
// static char *username = NULL;
// static char *password = NULL;
// int verbose = 0;
// bool quiet = false;

// void my_message_callback(void *obj, const struct mosquitto_message *message)
// {
//     //收到消息时，打印
//     if(verbose){
//         if(message->payloadlen){
//             printf("%s %s\n", message->topic, message->payload);
//         }else{
//             printf("%s (null)\n", message->topic);
//         }
//         fflush(stdout);
//     }else{
//         if(message->payloadlen){
//             printf("%s\n", message->payload);
//             fflush(stdout);
//         }
//     }
// }

// void my_connect_callback(void *obj, int result)
// {
//     //连接建立后订阅主题
//     struct mosquitto *mosq = (mosquitto *)obj;

//     int i;
//     if(!result){
//         for(i=0; i<topic_count; i++){
//             mosquitto_subscribe(mosq, NULL, topics[i], topic_qos);
//         }
//     }else{
//         switch(result){
//             case 1:
//                 if(!quiet) fprintf(stderr, "Connection Refused: unacceptable protocol version\n");
//                 break;
//             case 2:
//                 if(!quiet) fprintf(stderr, "Connection Refused: identifier rejected\n");
//                 break;
//             case 3:
//                 if(!quiet) fprintf(stderr, "Connection Refused: broker unavailable\n");
//                 break;
//             case 4:
//                 if(!quiet) fprintf(stderr, "Connection Refused: bad user name or password\n");
//                 break;
//             case 5:
//                 if(!quiet) fprintf(stderr, "Connection Refused: not authorised\n");
//                 break;
//             default:
//                 if(!quiet) fprintf(stderr, "Connection Refused: unknown reason\n");
//                 break;
//         }
//     }
// }

// void my_subscribe_callback(void *obj, uint16_t mid, int qos_count, const uint8_t *granted_qos)
// {
//     //打印订阅的相关信息
//     int i;

//     if(!quiet) printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
//     for(i=1; i<qos_count; i++){
//         if(!quiet) printf(", %d", granted_qos[i]);
//     }
//     if(!quiet) printf("\n");
// }

// void print_usage(void)
// {
//     //打印mosquitto_sub帮助信息
//     printf("mosquitto_sub is a simple mqtt client that will subscribe to a single topic and print all messages it receives.\n\n");
//     printf("Usage: mosquitto_sub [-c] [-h host] [-k keepalive] [-p port] [-q qos] [-v] -t topic ...\n");
//     printf("                     [-i id] [-I id_prefix]\n");
//     printf("                     [-d] [--quiet]\n");
//     printf("                     [-u username [-P password]]\n");
//     printf("                     [--will-topic [--will-payload payload] [--will-qos qos] [--will-retain]]\n\n");
//     printf(" -c : disable 'clean session' (store subscription and pending messages when client disconnects).\n");
//     printf(" -d : enable debug messages.\n");
//     printf(" -h : mqtt host to connect to. Defaults to localhost.\n");
//     printf(" -i : id to use for this client. Defaults to mosquitto_sub_ appended with the process id.\n");
//     printf(" -I : define the client id as id_prefix appended with the process id. Useful for when the\n");
//     printf("      broker is using the clientid_prefixes option.\n");
//     printf(" -k : keep alive in seconds for this client. Defaults to 60.\n");
//     printf(" -p : network port to connect to. Defaults to 1883.\n");
//     printf(" -q : quality of service level to use for the subscription. Defaults to 0.\n");
//     printf(" -t : mqtt topic to subscribe to. May be repeated multiple times.\n");
//     printf(" -u : provide a username (requires MQTT 3.1 broker)\n");
//     printf(" -v : print published messages verbosely.\n");
//     printf(" -P : provide a password (requires MQTT 3.1 broker)\n");
//     printf(" --quiet : don't print error messages.\n");
//     printf(" --will-payload : payload for the client Will, which is sent by the broker in case of\n");
//     printf("                  unexpected disconnection. If not given and will-topic is set, a zero\n");
//     printf("                  length message will be sent.\n");
//     printf(" --will-qos : QoS level for the client Will.\n");
//     printf(" --will-retain : if given, make the client Will retained.\n");
//     printf(" --will-topic : the topic on which to publish the client Will.\n");
//     printf("\nSee http://mosquitto.org/ for more information.\n\n");
// }

// int main(int argc, char *argv[])
// {
//     char *id = NULL;
//     char *id_prefix = NULL;
//     int i;
//     char *host = "localhost";
//     int port = 1883;
//     int keepalive = 60;
//     bool clean_session = true;
//     bool debug = false;
//     struct mosquitto *mosq = NULL;
//     int rc;
//     char hostname[21];
//     char err[1024];

//     uint8_t *will_payload = NULL;
//     long will_payloadlen = 0;
//     int will_qos = 0;
//     bool will_retain = false;
//     char *will_topic = NULL;

//     for(i=1; i<argc; i++){
//         if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")){    //-p
//             if(i==argc-1){
//                 fprintf(stderr, "Error: -p argument given but no port specified.\n\n");
//                 print_usage();
//                 return 1;
//             }else{
//                 port = atoi(argv[i+1]);
//                 if(port<1 || port>65535){
//                     fprintf(stderr, "Error: Invalid port given: %d\n", port);
//                     print_usage();
//                     return 1;
//                 }
//             }
//             i++;
//         }else if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--disable-clean-session")){    //-c，在client断开连接之后，仍保持订阅
//             clean_session = false;
//         }else if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--debug")){    //-d
//             debug = true;
//         }else if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--host")){        //-h
//             if(i==argc-1){
//                 fprintf(stderr, "Error: -h argument given but no host specified.\n\n");
//                 print_usage();
//                 return 1;
//             }else{
//                 host = argv[i+1];
//             }
//             i++;
//         }else if(!strcmp(argv[i], "-i") || !strcmp(argv[i], "--id")){        //-i
//             if(id_prefix){
//                 fprintf(stderr, "Error: -i and -I argument cannot be used together.\n\n");
//                 print_usage();
//                 return 1;
//             }
//             if(i==argc-1){
//                 fprintf(stderr, "Error: -i argument given but no id specified.\n\n");
//                 print_usage();
//                 return 1;
//             }else{
//                 id = argv[i+1];
//             }
//             i++;
//         }else if(!strcmp(argv[i], "-I") || !strcmp(argv[i], "--id-prefix")){        //-I
//             if(id){
//                 fprintf(stderr, "Error: -i and -I argument cannot be used together.\n\n");
//                 print_usage();
//                 return 1;
//             }
//             if(i==argc-1){
//                 fprintf(stderr, "Error: -I argument given but no id prefix specified.\n\n");
//                 print_usage();
//                 return 1;
//             }else{
//                 id_prefix = argv[i+1];
//             }
//             i++;
//         }else if(!strcmp(argv[i], "-k") || !strcmp(argv[i], "--keepalive")){    //-k，keepalive，定义客户端多久向server发送PING以保持连接
//             if(i==argc-1){
//                 fprintf(stderr, "Error: -k argument given but no keepalive specified.\n\n");
//                 print_usage();
//                 return 1;
//             }else{
//                 keepalive = atoi(argv[i+1]);
//                 if(keepalive>65535){
//                     fprintf(stderr, "Error: Invalid keepalive given: %d\n", keepalive);
//                     print_usage();
//                     return 1;
//                 }
//             }
//             i++;
//         }else if(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--qos")){        //-q
//             if(i==argc-1){
//                 fprintf(stderr, "Error: -q argument given but no QoS specified.\n\n");
//                 print_usage();
//                 return 1;
//             }else{
//                 topic_qos = atoi(argv[i+1]);
//                 if(topic_qos<0 || topic_qos>2){
//                     fprintf(stderr, "Error: Invalid QoS given: %d\n", topic_qos);
//                     print_usage();
//                     return 1;
//                 }
//             }
//             i++;
//         }else if(!strcmp(argv[i], "--quiet")){        //-quiet
//             quiet = true;
//         }else if(!strcmp(argv[i], "-t") || !strcmp(argv[i], "--topic")){        //-t，可订阅多个主题
//             if(i==argc-1){
//                 fprintf(stderr, "Error: -t argument given but no topic specified.\n\n");
//                 print_usage();
//                 return 1;
//             }else{
//                 topic_count++;
//                 topics = (char **)realloc(topics, topic_count*sizeof(char *));
//                 topics[topic_count-1] = argv[i+1];
//             }
//             i++;
//         }else if(!strcmp(argv[i], "-u") || !strcmp(argv[i], "--username")){        //-u
//             if(i==argc-1){
//                 fprintf(stderr, "Error: -u argument given but no username specified.\n\n");
//                 print_usage();
//                 return 1;
//             }else{
//                 username = argv[i+1];
//             }
//             i++;
//         }else if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")){        //-v，详细打印消息，格式：topic payload
//             verbose = 1;
//         }else if(!strcmp(argv[i], "-P") || !strcmp(argv[i], "--pw")){        //-P
//             if(i==argc-1){
//                 fprintf(stderr, "Error: -P argument given but no password specified.\n\n");
//                 print_usage();
//                 return 1;
//             }else{
//                 password = argv[i+1];
//             }
//             i++;
//         }else if(!strcmp(argv[i], "--will-payload")){        //--will-payload
//             if(i==argc-1){
//                 fprintf(stderr, "Error: --will-payload argument given but no will payload specified.\n\n");
//                 print_usage();
//                 return 1;
//             }else{
//                 will_payload = (uint8_t *)argv[i+1];
//                 will_payloadlen = strlen((char *)will_payload);
//             }
//             i++;
//         }else if(!strcmp(argv[i], "--will-qos")){   //--will-qos
//             if(i==argc-1){
//                 fprintf(stderr, "Error: --will-qos argument given but no will QoS specified.\n\n");
//                 print_usage();
//                 return 1;
//             }else{
//                 will_qos = atoi(argv[i+1]);
//                 if(will_qos < 0 || will_qos > 2){
//                     fprintf(stderr, "Error: Invalid will QoS %d.\n\n", will_qos);
//                     return 1;
//                 }
//             }
//             i++;
//         }else if(!strcmp(argv[i], "--will-retain")){    //--will-retain
//             will_retain = true;
//         }else if(!strcmp(argv[i], "--will-topic")){        //--will-topic
//             if(i==argc-1){
//                 fprintf(stderr, "Error: --will-topic argument given but no will topic specified.\n\n");
//                 print_usage();
//                 return 1;
//             }else{
//                 will_topic = argv[i+1];
//             }
//             i++;
//         }else{
//             fprintf(stderr, "Error: Unknown option '%s'.\n",argv[i]);
//             print_usage();
//             return 1;
//         }
//     }
//     if(clean_session == false && (id_prefix || !id)){        //设置了-c，就必须使用-i，不能用-I
//         if(!quiet) fprintf(stderr, "Error: You must provide a client id if you are using the -c option.\n");
//         return 1;
//     }
//     if(id_prefix){
//         id = (char *)malloc(strlen(id_prefix)+10);
//         if(!id){
//             if(!quiet) fprintf(stderr, "Error: Out of memory.\n");
//             return 1;
//         }
//         snprintf(id, strlen(id_prefix)+10, "%s%d", id_prefix, getpid());
//     }else if(!id){
//         id = (char *)malloc(30);
//         if(!id){
//             if(!quiet) fprintf(stderr, "Error: Out of memory.\n");
//             return 1;
//         }
//         memset(hostname, 0, 21);
//         gethostname(hostname, 20);
//         snprintf(id, 23, "mosq_sub_%d_%s", getpid(), hostname);
//     }

//     if(topic_count == 0){
//         fprintf(stderr, "Error: You must specify a topic to subscribe to.\n");
//         print_usage();
//         return 1;
//     }
//     if(will_payload && !will_topic){
//         fprintf(stderr, "Error: Will payload given, but no will topic given.\n");
//         print_usage();
//         return 1;
//     }
//     if(will_retain && !will_topic){
//         fprintf(stderr, "Error: Will retain given, but no will topic given.\n");
//         print_usage();
//         return 1;
//     }
//     if(password && !username){
//         if(!quiet) fprintf(stderr, "Warning: Not using password since username not set.\n");
//     }
//     mosquitto_lib_init();        //任何mosquitto functions之前都必须调用的函数，初始化操作
//     mosq = mosquitto_new(id,true,NULL);        //新建一个 mosquitto client实例
//     if(!mosq){
//         if(!quiet) fprintf(stderr, "Error: Out of memory.\n");
//         return 1;
//     }
//     if(debug){
//         mosquitto_log_init(mosq, MOSQ_LOG_DEBUG | MOSQ_LOG_ERR | MOSQ_LOG_WARNING
//                 | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO, MOSQ_LOG_STDERR);
//     }
//     if(will_topic && mosquitto_will_set(mosq, true, will_topic, will_payloadlen, will_payload, will_qos, will_retain)){
//         if(!quiet) fprintf(stderr, "Error: Problem setting will.\n");
//         return 1;
//     }
//     if(username && mosquitto_username_pw_set(mosq, username, password)){
//         if(!quiet) fprintf(stderr, "Error: Problem setting username and password.\n");
//         return 1;
//     }
//     mosquitto_connect_callback_set(mosq, my_connect_callback);        //设置client与broker建立连接后所调用的函数
//     mosquitto_message_callback_set(mosq, my_message_callback);        //设置client从broker收到消息后所调用的函数
//     if(debug){
//         mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);    //设置当broker相应一个订阅请求时所调用的函数
//     }

//     rc = mosquitto_connect(mosq, host, port, keepalive, clean_session);        //建立连接
//     if(rc){
//         if(!quiet){
//             if(rc == MOSQ_ERR_ERRNO){
// #ifndef WIN32
//                 strerror_r(errno, err, 1024);
// #else
//                 FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errno, 0, (LPTSTR)&err, 1024, NULL);
// #endif
//                 fprintf(stderr, "Error: %s\n", err);
//             }else{
//                 fprintf(stderr, "Unable to connect (%d).\n", rc);
//             }
//         }
//         return rc;
//     }

//     do{
//         rc = mosquitto_loop(mosq, -1);    //主网络循环监控client，以保持client和broker之间的通信正常
//     }while(rc == MOSQ_ERR_SUCCESS);

//     mosquitto_destroy(mosq);    //释放mosquitto实例的内存空间
//     mosquitto_lib_cleanup();    //释放library所使用的资源

//     return rc;
// }