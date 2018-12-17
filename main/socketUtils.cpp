#include "socketUtils.h"

ssize_t sendn (int fd, const void *vptr, size_t n, int flags) {
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = (char *)vptr;
    nleft = n;
    while (nleft > 0)
    {
        if ( (nwritten = send(fd, ptr, nleft, flags)) <= 0)
        {
                return -1;
        }
        nleft -= nwritten;
        ptr   += nwritten;
    }
    return n;
}


int espx_last_socket_errno(int socket) {
    int ret = 0;
    u32_t optlen = sizeof(ret);
    getsockopt(socket, SOL_SOCKET, SO_ERROR, &ret, &optlen);
    return ret;
}

esp_err_t connect_to_server(int *sockPtr) {
    int errCode;
    int ret_conn;
    struct sockaddr_in serverAddress;
    int sErrCode;

    // wait to be connected on the AP in the case of new connection or connection lost
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,
            false, true, portMAX_DELAY);
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_ADDR, &serverAddress.sin_addr.s_addr);
    serverAddress.sin_port = htons((u16_t) atoi(SERVER_PORT));
    ESP_LOGI(TAG, "creating socket");
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cout << "ERROR: error creating socket";
        return ESP_FAIL;
    }
    cout << "Connecting to: " << inet_ntoa(serverAddress.sin_addr) << 
            ":" << ntohs(serverAddress.sin_port) << endl;
    ret_conn = connect(sock, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in));
    if (ret_conn != 0) {
        sErrCode = espx_last_socket_errno(sock);
        errCode = errno;
        cout << "ERROR: fail to connect" << endl <<
                "Socket Error code: " << sErrCode << endl <<
                "errno :" << errCode << endl;
        close(sock);
        *sockPtr = -1;
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "connection done on socket %d\n", sock);
    const char *msg = "ciao";
    sendn(sock, msg, sizeof(msg), 0);
    *sockPtr = sock;
    
    return ESP_OK;
}