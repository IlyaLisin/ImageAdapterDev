#pragma once

#include <string.h>
#include <opencv\cv.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdint.h>
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>
#include "utils.h"


class ImageAdapter
{
private:
	char const *host = "localhost";
	int port = 5672;
	char const *login = "guest";
	char const *password = "guest";
	char const *vhost = "/";
	char const *exchange = "Image";
	char const *routingkey = "send";
	amqp_socket_t *socket = NULL;
	amqp_connection_state_t conn;
public:
	ImageAdapter();
	
	ImageAdapter(char const *host, int port, char const *vhost, char const *login, char const *password, char const *exchange, char const *routingkey);

	int SendImage(char* data, int size, char* guid);
	int StartLearn(char* guid, int size);

	~ImageAdapter();
};

