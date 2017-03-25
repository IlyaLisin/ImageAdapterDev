#include "ImageAdapter.h"


ImageAdapter::ImageAdapter(char const *host, int port, char const *vhost, char const *login, char const *password, char const *exchange, char const *routingkey)
{
	this->host = host;
	this->port = port;
	this->vhost = vhost;
	this->login = login;
	this->password = password;
	this->exchange = exchange;
	this->routingkey = routingkey;

	ImageAdapter();
}

ImageAdapter::ImageAdapter()
{
	this->conn = amqp_new_connection();
	this->socket = amqp_tcp_socket_new(conn);

	if (!socket) {
		die("creating TCP socket");
	}

	int status;
	status = amqp_socket_open(this->socket, this->host, this->port);
	if (status) {
		die("opening TCP socket");
	}
	printf("1");
	die_on_amqp_error(amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "guest"),
		"Logging in");
	printf("2");

	amqp_channel_open(conn, 1);
	die_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");

}

int ImageAdapter::SendImage(char* data, int size, char* guid)
{
	amqp_basic_properties_t props;
	props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
	props.content_type = amqp_cstring_bytes("text/plain");
	props.delivery_mode = 2; /* persistent delivery mode */

	char _size[3];
	_itoa(size, _size, 10);

		size_t len_data = strlen(data);
		size_t len_guid = strlen(guid);

		char* _data = new char[];

		strcpy(_data, guid);
		strcat(_data, "|");
		strcat(_data, _size);
		strcat(_data, "|");
		strcat(_data, data);

	die_on_error(amqp_basic_publish(this->conn,
		1,
		amqp_cstring_bytes(this->exchange),
		amqp_cstring_bytes(this->routingkey),
		0,
		0,
		&props,
		amqp_cstring_bytes(_data)),
		"Publishing");

	return 1;
}

int ImageAdapter::StartLearn(char* guid, int size)
{
	amqp_basic_properties_t props;
	props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
	props.content_type = amqp_cstring_bytes("text/plain");
	props.delivery_mode = 2; /* persistent delivery mode */

	char _size[3];
	_itoa(size, _size, 10);

	//size_t len_data = strlen(data);
	size_t len_guid = strlen(guid);

	char* _data = new char[];

	strcpy(_data, guid);
	strcat(_data, "|");
	strcat(_data, _size);

	die_on_error(amqp_basic_publish(this->conn,
		1,
		amqp_cstring_bytes(this->exchange),
		amqp_cstring_bytes("Learn"),
		0,
		0,
		&props,
		amqp_cstring_bytes(_data)),
		"Publishing");

	return 1;
}

ImageAdapter::~ImageAdapter()
{
	die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS), "Closing channel");
	die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS), "Closing connection");
	die_on_error(amqp_destroy_connection(conn), "Ending connection");
}
