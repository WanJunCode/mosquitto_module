#include "TMqttWrapper.h"

// mosquittopp 连接
TMqttWrapper::TMqttWrapper(const std::string &id, bool clean_session)
    : mosqpp::mosquittopp(id.empty() ? NULL : id.c_str(), clean_session)
    , mqtt_conn_(false)
    , mqtt_exit_(false)
    , mqtt_disconnect_(false) {

}

// mosquittopp 断开连接
TMqttWrapper::~TMqttWrapper() {
    mqtt_exit_ = true;
    disconnect();
    int ret = loop_stop();
    if (MOSQ_ERR_SUCCESS  != ret) {
        printf("loop_stop failed:%s", mosqpp::strerror(ret));
    }
}

bool TMqttWrapper::connect(const std::string &username, const std::string &password, const std::string &host, int port, int keepalive) {
    if (!mqtt_conn_) {
        int ret = MOSQ_ERR_SUCCESS;
        //config login user
        if (!username.empty()) {
            // mosquittopp 设置用户名，密码
            ret = username_pw_set(username.c_str(), password.c_str());
            if (MOSQ_ERR_SUCCESS != ret) {
                printf("setting mqtt client login user %s error:%s", username.c_str(), mosqpp::strerror(ret));
                return false;
            }
        }
        // mosquittopp 异步连接 broker
        ret = connect_async(host.c_str(), port, keepalive);
        if (MOSQ_ERR_SUCCESS != ret) {
            printf("Connect to an MQTT broker error:%s", mosqpp::strerror(ret));
            return false;
        }
        // create thread loop message 创建异步线程循环
        ret = loop_start();
        if (MOSQ_ERR_SUCCESS != ret) {
            printf("loop_start failed:%s", mosqpp::strerror(ret));
            return false;
        }
    } else {
        printf("mqtt client had been started\n");
    }
    return true;
}

// names in the functions commented to prevent unused parameter warning
void TMqttWrapper::on_connect(int rc) {
    printf("on connect\n");
}

void TMqttWrapper::on_disconnect(int rc) {
    printf("on disconnect\n");
}

void TMqttWrapper::on_publish(int mid) {
    printf("on publish\n");
}

void TMqttWrapper::on_message(const struct mosquitto_message * message) {
    printf("on message\n");
}

void TMqttWrapper::on_subscribe(int mid, int qos_count, const int * granted_qos) {
    printf("on subscribe\n");
}

void TMqttWrapper::on_unsubscribe(int mid) {
    printf("on unsubscribe\n");
}

void TMqttWrapper::on_log(int level, const char * str) {
    printf("on log\n");
}

void TMqttWrapper::on_error() {
    printf("on error\n");
}

void TMqttWrapper::subscribe_topic(std::string topic){
    subscribe(NULL, topic.c_str(), 0);
}