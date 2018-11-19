#ifndef PDSPROJECT_SOCKETUTILS_H
#define PDSPROJECT_SOCKETUTILS_H

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include <iostream>
#include "esp_log.h"

#define SERVER_ADDR         CONFIG_SERVER_ADDRESS
#define SERVER_PORT         CONFIG_SERVER_PORT
#define LOG_LOCAL_LEVEL  CONFIG_LOG_DEFAULT_LEVEL

using namespace std;

extern const char *TAG;

/**
  * @brief     wrapper to send data of size n in more than one send if required
  *
  * @param     fd 		socket where sending data
  * @param     vptr		data buffer
  * @param 	   n 		size of vptr
  * @param 	   flags 	send flags
  *
  * @return
  *    - -1 if erro
  *	   - size of sent data
  */
ssize_t sendn (int fd, const void *vptr, size_t n, int flags);

/**
  * @brief     
  *
  * @param     sockPtr socket pointer of connection to the server SERVER_ADDR on
  *			   SERVER_PORT
  *
  * @return
  *    - ESP_OK: succeed
  *	   - ESP_FAIL: fail
  */
esp_err_t connect_to_server(int *sockPtr);

#endif //PDSPROJECT_SOCKETUTILS_H
