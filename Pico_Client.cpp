#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include <string.h>
#include "Sound.h"

#define BACKEND_HOST "51.21.129.98"
#define BACKEND_PORT 3000
#define ENDPOINT_PATH "/notifications"

static ip_addr_t backend_ip;
static bool ready = false;

static err_t receive_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char *data = (char*)p->payload;
    char *body = strstr(data, "\r\n\r\n");

    if (body){
        body += 4;
        int len = p->len - (body - data);

        if (len > 0 && len < 256) {
            char json[256] = {0};
            strncpy(json, body, len);
            json[len] = '\0';

            // Simple JSON parse: find "message":"..."
            char *msg_start = strstr(json, "\"message\"");
            if (msg_start) {
                msg_start = strchr(msg_start, ':');
                if (msg_start) {
                    msg_start++; // skip ':'
                    while (*msg_start == ' ' || *msg_start == '\t') msg_start++; // skip whitespace
                    if (*msg_start == '"') {
                        msg_start++; // skip opening quote
                        char *msg_end = strchr(msg_start, '"');
                        if (msg_end) {
                            int msg_len = msg_end - msg_start;
                            if (msg_len > 0 && msg_len < 128) {
                                char msg[128] = {0};
                                strncpy(msg, msg_start, msg_len);
                                msg[msg_len] = '\0';
                                
                                printf("Notification: %s\n", msg);
                                show_notification(msg);
                            }
                        }
                    }
                }
            }
        }
    }
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    tcp_close(tpcb);
    return ERR_OK;

}

static err_t connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {
        printf("Connection failed\n");
        tcp_close(tpcb);
        return err;
    }

    char req[256];
    snprintf(req, sizeof(req),
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n\r\n",
        ENDPOINT_PATH, BACKEND_HOST);
    
    tcp_write(tpcb, req, strlen(req), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    tcp_recv(tpcb, receive_callback);
    return ERR_OK;
}

void check_notification() {
    if(!ready){
        return;
    }

    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        return;
    }

    if (tcp_connect(pcb, &backend_ip, BACKEND_PORT, connect_callback) != ERR_OK) {
        tcp_close(pcb);
    }
}

void pico_client_init() {
    IP4_ADDR(&backend_ip, 51, 21, 129, 98);
    ready = true;
    printf("Backend: %s:%d%s\n", BACKEND_HOST, BACKEND_PORT, ENDPOINT_PATH);
}

