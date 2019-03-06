#ifndef _WJ_MAINSERVER_H
#define _WJ_MAINSERVER_H

#include <event.h>
#include <mosquitto.h>

class MainServer{
public:
    // single
    static MainServer& getInstance(){
        static MainServer root;
        return root;
    }
    ~MainServer();
    void service_loop();

private:
    MainServer();
    void init( );

private:
    struct event_base * base_;
    struct event *      stdin_;
    struct mosquitto *  mqtt_;

private:
    static void stdinCB(evutil_socket_t socket, short what, void *arg);
};

#endif