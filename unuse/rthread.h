#ifndef __BASE_SERVER_H__
#define __BASE_SERVER_H__
#include <thread>

using std::thread;
class RThread
{
public:
	RThread() :thread_(run, this) { runing_ = false; };
	virtual ~RThread(){}
	virtual void start() { thread_.join(); }
	virtual void stop() { runing_ = false; }
	virtual bool run_sink() { return true; }
	
	static void run(RThread* thread);
	
	thread	thread_;
	bool	runing_;
};

#endif
