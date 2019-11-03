#ifndef __SERVER_MANAGER_H__
#define __SERVER_MANAGER_H__

#include <queue>
#include <mutex>
#include <stdlib.h>
#include <vector>
#include <mutex>

using std::vector;
using std::mutex;

class ServerManager{
public:
	static const uint16_t s_max_servernum = 65535;
public:
struct Message{
	uint32_t	from_;
	uint32_t	to_;
	void*		data_;
	uint32_t	data_size_;
};

public:
class Server{
public:
	Server(uint16_t server_id): server_id_(server_id) {} ;
	~Server();	
	void push_message(Message* msg);
	Message* pick_message();
	bool empty();
private:
	uint16_t					server_id_;

	std::queue<Message*>		queue_;
	std::mutex					mutex_;
};

private:
	ServerManager() {
		for (int i = 0; i < s_max_servernum; i++) {
			arr_msg_queue_[i] = nullptr;
		}
		cur_worker_server_ = -1;
	}
	~ServerManager(){}
	ServerManager(const ServerManager&)=delete;
	ServerManager& operator=(const ServerManager&)=delete;

	static ServerManager* s_manager;
public:
	static ServerManager* get_instance();

	void		add_server_queue(uint16_t server_id);
	void		add_server(uint16_t server_id);

	void		push_message(Message* msg);
	Message*	pick_message(uint16_t server_id);
	Server*		pick_server();

	Message*	create_message();
	void		release_message(Message* msg);
private:
	Server*				arr_msg_queue_[s_max_servernum];
	int					cur_worker_server_;
	vector<uint16_t>	v_worker_server_;
	std::mutex			mutext_worker_server_;
};


#endif

