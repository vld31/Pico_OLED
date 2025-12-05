#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include <string.h>
#include "Sound.h"

#define BACKEND_HOST "51.21.129.98"
#define BACKEND_PORT 3000
#define ENDPOINT_PATH "/api/notifications"

typedef struct {
    char buf[1024];
    int len;
} http_ctx_t;

static ip_addr_t backend_ip;
static bool ready = false;

static err_t receive_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    http_ctx_t *ctx = (http_ctx_t *)arg;
    if (!p) {
        if (ctx && ctx->len > 0) {
            ctx->buf[ctx->len] = '\0';
            char *body = strstr(ctx->buf, "\r\n\r\n");
            if (body) {
                body += 4;
                const char *cursor = body;
                while (1) {
                    const char *msg_key = strstr(cursor, "\"message\"");
                    if (!msg_key) break;
                    const char *colon = strchr(msg_key, ':');
                    if (!colon) break;
                    cursor = colon + 1;
                    while (*cursor == ' ' || *cursor == '\t') cursor++;
                    if (*cursor == '"') {
                        cursor++;
                        const char *end_quote = strchr(cursor, '"');
                        if (!end_quote) break;
                        int msg_len = (int)(end_quote - cursor);
                        if (msg_len > 0 && msg_len < 128) {
                            char msg[128] = {0};
                            strncpy(msg, cursor, msg_len);
                            msg[msg_len] = '\0';
                            printf("Notification: %s\n", msg);
                            show_notification(msg);
                        }
                        cursor = end_quote + 1;
                    } else {
                        break;
                    }
                }
            }
        }
        tcp_close(tpcb);
        if (ctx) delete ctx;
        return ERR_OK;
    }

    if (ctx) {
        u16_t copy_len = p->tot_len;
        if (ctx->len + copy_len > (int)sizeof(ctx->buf) - 1) {
            copy_len = sizeof(ctx->buf) - 1 - ctx->len;
        }
        if (copy_len > 0) {
            pbuf_copy_partial(p, ctx->buf + ctx->len, copy_len, 0);
            ctx->len += copy_len;
        }
    }

    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
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
    
    http_ctx_t *ctx = new http_ctx_t();
    ctx->len = 0;
    tcp_arg(tpcb, ctx);
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

bool wifi_init() {
    printf("Initializing WiFi...\n");
    if (cyw43_arch_init()) {
        printf("WiFi init failed\n");
        return false;
    }
    cyw43_arch_enable_sta_mode();
    
    printf("Connecting to WiFi: %s\n", WIFI_SSID);
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, 
        CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("WiFi connection failed\n");
        return false;
    }
    printf("WiFi connected!\n");
    return true;
}

void pico_client_init() {
    IP4_ADDR(&backend_ip, 51, 21, 129, 98);
    ready = true;
    printf("Backend: %s:%d%s\n", BACKEND_HOST, BACKEND_PORT, ENDPOINT_PATH);
}

