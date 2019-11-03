#include "rthread.h"
#include <iostream>
using std::cout;
using std::endl;

void RThread::run(RThread* thread) {
	thread->runing_ = true;
	for (;;)
	{
		if (!thread->runing_) {
			break;
		}
		if (!thread->run_sink()) {
			// log
			break;
		}
	}
};