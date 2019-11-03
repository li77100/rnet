#ifndef __POLL__H_
#define __POLL__H_

enum emSocketPoll{
	socket_poll_close = 0,
	socket_poll_continue = 1,
	socket_poll_wait = 2
};


enum emSockType{
	sock_exit = 0,
	sock_data = 1,
	sock_close = 2,
	sock_open = 3,
	sock_err = 4,
	sock_accept = 5,
	sock_udp = 6,
	sock_warning = 7
};

struct Poll{
	virtual emSocketPoll socket_run() = 0;
	virtual void Init() = 0;
	virtual void stop() = 0;
};

#endif
