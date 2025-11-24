#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/ip4_addr.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>

#define STATIC_IP "192.168.1.100"

static bool runtime_debug_enabled = false;

bool pico_httpd_get_debug_flag(void) {
    return runtime_debug_enabled;
}

void pico_httpd_set_debug_flag(bool enabled) {
    runtime_debug_enabled = enabled;
}

static absolute_time_t wifi_connected_time;
static struct tcp_pcb *http_listener;

static err_t http_send_simple(struct tcp_pcb *tpcb, const char *body) {
    char buf[256];
    size_t body_len = strlen(body);
    size_t len = (size_t)snprintf(buf, sizeof(buf),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Content-Length: %u\r\n"
        "Connection: close\r\n\r\n"
        "%s",
        (unsigned)body_len, body);
    if (len >= sizeof(buf)) {
        len = sizeof(buf) - 1;
    }
    err_t err = tcp_write(tpcb, buf, (u16_t)len, TCP_WRITE_FLAG_COPY);
    if (err == ERR_OK) {
        tcp_output(tpcb);
    }
    return err;
}

static err_t http_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    LWIP_UNUSED_ARG(arg);
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    u16_t payload_len = p->tot_len;
    tcp_recved(tpcb, payload_len);
    pbuf_free(p);

    char body[128];
    uint64_t uptime_s = absolute_time_diff_us(wifi_connected_time, get_absolute_time()) / 1000000ULL;
    snprintf(body, sizeof(body), "Pico W HTTP OK\nUptime=%" PRIu64 "s\n", uptime_s);
    
    http_send_simple(tpcb, body);

    // Close after response
    tcp_close(tpcb);
    return ERR_OK;
}

static err_t http_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);
    tcp_setprio(newpcb, TCP_PRIO_MIN);
    tcp_recv(newpcb, http_recv);
    return ERR_OK;
}

static void http_server_start(void) {
    http_listener = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!http_listener) return;

    if (tcp_bind(http_listener, IP_ANY_TYPE, 80) != ERR_OK) {
        tcp_close(http_listener);
        http_listener = NULL;
        return;
    }
    http_listener = tcp_listen_with_backlog(http_listener, 2);
    tcp_accept(http_listener, http_accept);
}

// Return some characters from the ascii representation of the mac address
// e.g. 112233445566
// chr_off is index of character in mac to start
// chr_len is length of result
// chr_off=8 and chr_len=4 would return "5566"
// Return number of characters put into destination
static size_t get_mac_ascii(int idx, size_t chr_off, size_t chr_len, char *dest_in) {
    static const char hexchr[16] = "0123456789ABCDEF";
    uint8_t mac[6];
    char *dest = dest_in;
    assert(chr_off + chr_len <= (2 * sizeof(mac)));
    cyw43_hal_get_mac(idx, mac);
    for (; chr_len && (chr_off >> 1) < sizeof(mac); ++chr_off, --chr_len) {
        *dest++ = hexchr[mac[chr_off >> 1] >> (4 * (1 - (chr_off & 1))) & 0xf];
    }
    return dest - dest_in;
}

int pico_httpd_start(void) {
    
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();

    printf("Setting Static Ip: %s\n", STATIC_IP);
    cyw43_arch_lwip_begin();
    struct netif *netif = &cyw43_state.netif[CYW43_ITF_STA];

    printf("Connecting to WiFi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect.\n");
        return 1;
    } else {
        printf("Connected.\n");
    }
    printf("\nReady, serving HTTP on %s\n", ip4addr_ntoa(netif_ip4_addr(netif_list)));

    wifi_connected_time = get_absolute_time();

    // Start minimal TCP HTTP server
    cyw43_arch_lwip_begin();
    http_server_start();
    cyw43_arch_lwip_end();

    while(true) {
#if PICO_CYW43_ARCH_POLL
        cyw43_arch_poll();
        
#else 

#endif
    }
    cyw43_arch_deinit();
}
