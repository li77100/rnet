#include "server_manager.h"
#include "assert.h"
#include "string.h"

ServerManager* ServerManager::s_manager = nullptr;

ServerManager::Server::~Server(){
	if(!queue_.empty() ){
		assert(false);
		// error log
	}
}
ServerManager* ServerManager::get_instance()
{
	if (nullptr == s_manager) {
		s_manager = new ServerManager;
	}
	return s_manager;
}

void ServerManager::Server::push_message(Message* msg){
	std::lock_guard<std::mutex> lck (mutex_);
	queue_.push(msg);
}

ServerManager::Message* ServerManager::Server::pick_message(){
	std::lock_guard<std::mutex> lck (mutex_);
	if(queue_.empty() ){
		return nullptr;
	}
	ServerManager::Message* msg = queue_.front();
	queue_.pop();
	return msg;
}

bool ServerManager::Server::empty() {
	std::lock_guard<std::mutex> lck(mutex_);
	return queue_.empty();
}

void ServerManager::add_server_queue(uint16_t server_id){
	if(server_id>=s_max_servernum || nullptr!=arr_msg_queue_[server_id]){
		// error log
		assert(false);
		exit(1);
	}
	arr_msg_queue_[server_id] = new ServerManager::Server(server_id);

	std::lock_guard<std::mutex> lck(mutext_worker_server_);
	v_worker_server_.push_back(server_id);
}

void ServerManager::add_server(uint16_t server_id) {
	if (server_id >= s_max_servernum || nullptr != arr_msg_queue_[server_id]) {
		// error log
		assert(false);
		exit(1);
	}
	arr_msg_queue_[server_id] = new ServerManager::Server(server_id);
}

void ServerManager::push_message(ServerManager::Message* msg){
	if(msg->to_>=s_max_servernum || nullptr == arr_msg_queue_[msg->to_]){
		assert(false);
		// error log
		return;
	}
	arr_msg_queue_[msg->to_]->push_message(msg);
}

ServerManager::Message* ServerManager::pick_message(uint16_t server_id){
	if(server_id>=s_max_servernum || nullptr == arr_msg_queue_[server_id]){
		return nullptr;
	}
	return arr_msg_queue_[server_id]->pick_message();
}

ServerManager::Server* ServerManager::pick_server() {
	std::lock_guard<mutex> lck(mutext_worker_server_);
	if (cur_worker_server_ >= v_worker_server_.size()) {
		cur_worker_server_ = 0;
	}
	auto cur = cur_worker_server_;
	for (int loop = 0; loop < v_worker_server_.size(); loop++) {
		if (++cur_worker_server_ >= v_worker_server_.size()) {
			cur_worker_server_ = 0;
		}
		if (!arr_msg_queue_[v_worker_server_[cur_worker_server_]]->empty()) {
			return arr_msg_queue_[v_worker_server_[cur_worker_server_]];
		}
	}
	return nullptr;
}

ServerManager::Message* ServerManager::create_message(){
	auto msg = new ServerManager::Message;
	memset(msg,0,sizeof(*msg) );
	return msg;
}

void ServerManager::release_message(Message* msg){
	delete msg;
}
