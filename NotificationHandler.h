#ifndef NOTIFICATION_HANDLER_H
#define NOTIFICATION_HANDLER_H

#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include <functional>

class DisplayManager;
class SoundController;

class NotificationHandler {
public:
    using MessageCallback = std::function<void(const char*)>;
    
    NotificationHandler(const char* host, uint16_t port, const char* endpoint);
    ~NotificationHandler();
    
    void initialize();
    void checkNotifications();
    void setDisplayManager(DisplayManager* display) { this->display = display; }
    void setSoundController(SoundController* sound) { this->sound = sound; }
    void setMessageCallback(MessageCallback callback) { this->message_callback = callback; }
    
private:
    const char* host;
    uint16_t port;
    const char* endpoint;
    ip_addr_t backend_ip;
    bool ready;
    
    DisplayManager* display;
    SoundController* sound;
    MessageCallback message_callback;
    
    typedef struct {
        char buf[1024];
        int len;
        NotificationHandler* handler;
    } http_ctx_t;
    
    static err_t receive_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    static err_t connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err);
    
    void processMessage(const char* message);
};

#endif 
