#ifndef __TIME_WHEEL_H__
#define __TIME_WHEEL_H__

#include <stdint.h>
#include <mutex>
#include <list>
#include <atomic>
#include <vector>

#define CountArray(arr) (sizeof(arr)/sizeof(arr[0]))

class TimeWheel
{
private:
	struct TimerEvent {
		uint16_t server_id_;
		clock_t		interval_;
		clock_t		nxt_trigger_time_;
		void*		data_;
		int			data_size_;
		uint32_t timer_id_;
		int   repeat_;
	};
	typedef std::list<TimerEvent*> list_timer_event;
public:
	const static int s_arr_timer_count = 4;

	static TimeWheel* get_instance() { return &s_instance; }

	void  init();
	bool  run_sink();
	uint32_t add_timer(uint16_t server_id, int interval, void* data,uint16_t data_size, int repeat = 1);
	bool  delete_timer(uint32_t timer_id);
private:
	TimeWheel(uint16_t server_id):server_id_(server_id){}
	TimeWheel(const TimeWheel&) {}
	~TimeWheel();

	void  add(TimerEvent* timer_event);
	inline TimerEvent* create_timer();
	inline void release_timer(TimerEvent* timer_event);

	static TimeWheel s_instance;
	const static int   s_arr_timer[s_arr_timer_count - 1];
private:
	bool     running_;
	uint16_t server_id_;
	std::atomic<uint32_t> cur_time_id_;

	clock_t     cur_clock_;

	clock_t     nxt_clock_[s_arr_timer_count - 1];
	list_timer_event   timer_type_[s_arr_timer_count];
	list_timer_event  timer_storage_;
	list_timer_event  timer_all_;
	std::mutex    timer_mutex_;
};



#endif
