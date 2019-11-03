#ifndef __POLL_TCP_H__
#define __POLL_TCP_H__

#include "poll.h"
 #include <sys/epoll.h>

class TcpPoll : Poll{
	static const int s_max_events_size = 64;
public:
	TcpPoll(uint16_t port,int backlog,uint16_t server_for):port_(port), backlog_(backlog), server_for_(server_for),
		runing_(false)
		{}
	~TcpPoll(){}
public:
	virtual emSocketPoll socket_run();
	virtual void Init();
	virtual void stop();
public:
	bool add_ctl(int fd,int sock,void* data);
private:
	bool			runing_;
	uint16_t 		port_;
	int				backlog_;
	uint16_t		server_for_;

	int				epfd_;
	epoll_event		events_[s_max_events_size];
	int				event_count_;
	int				event_index_;
};

#endif
