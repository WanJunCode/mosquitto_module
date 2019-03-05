#ifndef __CF_MQTT_WRAPPER_H
#define __CF_MQTT_WRAPPER_H

#include <string>
#include <mosquittopp.h>

class TMqtt;
class TMqttWrapper : public mosqpp::mosquittopp {
public:
    explicit TMqttWrapper(const std::string &id = std::string(), bool clean_session = true);
    virtual ~TMqttWrapper();

public:
    bool connect(const std::string &username, const std::string &password, const std::string &host, int port = 1883, int keepalive = 60);
//Override
public:
    // names in the functions commented to prevent unused parameter warning
    virtual void on_connect(int /*rc*/);
    virtual void on_disconnect(int /*rc*/);
    virtual void on_publish(int /*mid*/);
    virtual void on_message(const struct mosquitto_message * /*message*/);
    virtual void on_subscribe(int /*mid*/, int /*qos_count*/, const int * /*granted_qos*/);
    virtual void on_unsubscribe(int /*mid*/);
    virtual void on_log(int /*level*/, const char * /*str*/);
    virtual void on_error();

    void subscribe_topic(std::string topic);
private:
    bool mqtt_conn_;
    bool mqtt_exit_;
    bool mqtt_disconnect_;
};
#endif

