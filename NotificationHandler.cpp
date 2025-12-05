#include "NotificationHandler.h"
#include "DisplayManager.h"
#include "SoundController.h"
#include <stdio.h>
#include <string.h>

NotificationHandler::NotificationHandler(const char* host, uint16_t port, const char* endpoint)
    : host(host), port(port), endpoint(endpoint), ready(false), display(nullptr), sound(nullptr) {
}

NotificationHandler::~NotificationHandler() {
}

void NotificationHandler::initialize() {
    IP4_ADDR(&backend_ip, 51, 21, 129, 98);
    ready = true;
    printf("Backend: %s:%d%s\n", host, port, endpoint);
}

void NotificationHandler::checkNotifications() {
    if (!ready) {
        return;
    }
    
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        return;
    }
    
    http_ctx_t *ctx = new http_ctx_t();
    ctx->len = 0;
    ctx->handler = this;
    
    tcp_arg(pcb, ctx);
    
    if (tcp_connect(pcb, &backend_ip, port, connect_callback) != ERR_OK) {
        tcp_close(pcb);
        delete ctx;
    }
}

void NotificationHandler::processMessage(const char* message) {
    printf("Notification: %s\n", message);
    
    if (display) {
        display->showMessage("Notification:", message);
    }
    
    if (sound) {
        sound->beep(1000, 200);
    }
    
    if (message_callback) {
        message_callback(message);
    }
}

err_t NotificationHandler::connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    http_ctx_t *ctx = (http_ctx_t *)arg;
    
    if (err != ERR_OK) {
        printf("Connection failed\n");
        tcp_close(tpcb);
        delete ctx;
        return err;
    }
    
    char req[256];
    snprintf(req, sizeof(req),
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n\r\n",
        ctx->handler->endpoint, ctx->handler->host);
    
    tcp_write(tpcb, req, strlen(req), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    tcp_recv(tpcb, receive_callback);
    return ERR_OK;
}

err_t NotificationHandler::receive_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
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
                            ctx->handler->processMessage(msg);
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
