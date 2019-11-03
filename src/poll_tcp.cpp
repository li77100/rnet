#include "poll_tcp.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "log.h"

emSocketPoll TcpPoll::socket_run()
{
	emSocketPoll rs = socket_poll_continue;

	while(1)
	{
		if(!runing_)
		{
			break;
		}

		if(event_count_ == event_index_)
		{
			event_count_ = epoll_wait(epfd_, events_, s_max_events_size, -1);
			if(event_count_ <= 0)
			{
				if(errno == EINTR)
				{
					continue;	
				}
			}
		}
	}
	return socket_poll_continue;
}

bool TcpPoll::add_ctl(int fd,int sock,void* data)
{
	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = data;
	return epoll_ctl(fd, EPOLL_CTL_ADD, sock, &ev) != -1;
}

void TcpPoll::Init(){
	int sockfd = socket(AF_INET, SOCK_STREAM,0);
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr) );

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons( port_ );

	if(bind(sockfd, (sockaddr*)&addr, sizeof(addr) ) == -1){
		// err log
		log("bind error");
		exit(1);
	}

	if(listen(sockfd, 10) == -1){
		// err log
		log("listen error");
		exit(1);
	}

	epfd_ = epoll_create(1024);
	if(!add_ctl(epfd_, sockfd, nullptr)){
		log("add_ctl error %s\n", strerror(errno));
		exit(1);
	}

	runing_ = true;
	event_count_ = 0;
	event_index_ = 0;
}

void TcpPoll::stop(){
	runing_ = false;
}
