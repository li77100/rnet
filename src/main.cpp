#include "server_manager.h"
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include "poll_tcp.h"
#include "time_wheel.h"

using namespace std;
// ssh liya@192.168.20.133
// cd /mnt/hgfs/code/linux/rnet/src
// iconv -c -t us-ascii -f utf-8 source_file -o dest_file 
// git config --global user.name "li77100"
// git config --global user.email "771009984@qq.com"

void thread_moniter(){
	while(1){
		sleep(1);
	}
}

void thread_socket(void* p){
	cout << "thread_socket" << endl;
	Poll* poll = (Poll*)p;
	poll->Init();
	while(1){
		auto rs =  poll->socket_run();
		if(socket_poll_close == rs){
			break;
		}
		if(socket_poll_continue == rs){
			continue;
		}
		sleep(1);
	}	
}

void thread_timer(){
	cout << "thread_timer" << endl;

	TimeWheel* instance = TimeWheel::get_instance();
	while (1)
	{
		if (!instance->run_sink())
		{
			cout << "thread_timer break" << endl;
			break;
		}
	}
	cout << "thread_timer2" << endl;
}

void thread_job(){
	cout << "thread_job" << endl;
}

int main(){
	int thread_num = 8;
	TcpPoll poll(6666,10,2);

	vector<thread*> v_thread;
	v_thread.push_back(new thread(thread_moniter));
	v_thread.push_back(new thread(thread_socket,&poll));
	v_thread.push_back(new thread(thread_timer));
	for(int i=0;i<8;i++){
		v_thread.push_back(new thread(thread_job));
	}

	for(auto it:v_thread){
		it->join();
	}
	for_each(v_thread.begin(),v_thread.end(),[](thread* t){
		delete t;
	});
	v_thread.clear();

	cout << "hello" << endl;
	return 0;
}
