#include "server_socket.h"
#include "../globals.h"
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

Server *new_server_socket() {
	Server *ret = smalloc(sizeof(Server));

	assert_succ(ret->fd = socket(AF_INET, SOCK_STREAM, 0));

	int opt = 0;

	assert_succ(setsockopt(ret->fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
						   &opt, sizeof(ret->fd)));

	ret->address.sin_family = AF_INET;
	ret->address.sin_addr.s_addr = INADDR_ANY;
	ret->address.sin_port = htons(SERVER_PORT);

	ret->addr_len = sizeof(ret->address);

	assert_succ(bind(ret->fd, (struct sockaddr *)&ret->address, ret->addr_len));

	assert_succ(listen(ret->fd, 4));

	ret->fd_len = sizeof(ret->fd);

	return ret;
}

void close_socket(socket_fd socket) { assert_succ(close(socket)); }
