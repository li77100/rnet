///

#include "time_wheel.h"
#include<thread>
#include <ctime>
#include <string.h>
#include "server_manager.h"

TimeWheel TimeWheel::s_instance(1);
const int TimeWheel::s_arr_timer[s_arr_timer_count - 1] = { 1000, 60 * 1000, 60 * 60 * 1000 };

TimeWheel::~TimeWheel() {
	for (auto it : timer_all_) {
		delete it;
	}
	timer_all_.clear();
	timer_storage_.clear();
	for (int i = 0; i < s_arr_timer_count; i++) {
		timer_type_[i].clear();
	}
}

void TimeWheel::init() {
	clock_t clock = static_cast<clock_t> (std::clock() * 1000 / CLOCKS_PER_SEC);

	for (int i = 0; i < CountArray(nxt_clock_); i++) {
		nxt_clock_[i] = clock + s_arr_timer[i];
	}
}

bool TimeWheel::run_sink() {
	std::this_thread::sleep_for(std::chrono::milliseconds(20));

	std::unique_lock<std::mutex> lock(timer_mutex_);
	clock_t clock = static_cast<clock_t> (std::clock() * 1000 / CLOCKS_PER_SEC);

	// list_timer_event ll;
	bool big_clock = false;
	for (int i = CountArray(nxt_clock_) - 1; i >= 0; i--) {
		if (big_clock || clock >= nxt_clock_[i]) {
			big_clock = true;
			nxt_clock_[i] = clock + s_arr_timer[i];
			for (auto it = timer_type_[i + 1].begin(); it != timer_type_[i + 1].end(); ) {
				if ((*it)->nxt_trigger_time_ - clock < s_arr_timer[i]) {
					//   if (!add(*it)){
					//    ll.push_back(*it);
					//   }
					timer_type_[i].push_back(*it);
					it = timer_type_[i + 1].erase(it);
				}
				else {
					it++;
				}
			}
		}
	}

	auto instance = ServerManager::get_instance();
	for (auto it = timer_type_[0].begin(); it != timer_type_[0].end();) {
		bool erase = false;
		if ((*it)->nxt_trigger_time_ <= clock) {
			auto msg = instance->create_message();
			msg->from_ = server_id_;
			msg->to_ = (*it)->server_id_;
			msg->data_ = (*it)->data_;
			msg->data_size_ = (*it)->data_size_;
			instance->push_message(msg);

			if (--(*it)->repeat_ <= 0) {
				release_timer(*it);
				it = timer_type_[0].erase(it);
				erase = true;
			}
			else {
				(*it)->nxt_trigger_time_ = clock + (*it)->interval_;
				if ((*it)->interval_ >= s_arr_timer[0]) {
					add(*it);
					it = timer_type_[0].erase(it);
					erase = true;
				}
			}
		}
		if (!erase) {
			it++;
		}
	}

	//  for (auto it : ll){
	//   if (--(it->repeat_) > 0){
	//			 // post
	//    it->nxt_trigger_time_ += it->interval_;
	//    add(it);
	//   }
	//   it->call_back_(it->data_);
	//  }

	return true;
}

void TimeWheel::add(TimerEvent* timer_event)
{
	for (int i = CountArray(nxt_clock_) - 1; i >= 0; i--) {
		if (timer_event->nxt_trigger_time_ >= nxt_clock_[i]) {
			timer_type_[i + 1].push_back(timer_event);
			return;
		}
	}

	timer_type_[0].push_back(timer_event);
	return;
}

uint32_t TimeWheel::add_timer(uint16_t server_id, int interval, void* data, uint16_t data_size, int repeat/* = 1*/) {
	if (interval <= 0) {
		return 0;
	}
	std::unique_lock<std::mutex> lock(timer_mutex_);

	TimerEvent* timer_event = create_timer();
	timer_event->timer_id_ = cur_time_id_++;
	timer_event->server_id_ = server_id;
	timer_event->interval_ = static_cast<clock_t>(interval);
	timer_event->data_ = data;
	timer_event->nxt_trigger_time_ = timer_event->interval_ + static_cast<clock_t> (std::clock() * 1000 / CLOCKS_PER_SEC);
	timer_event->repeat_ = repeat;

	add(timer_event);
	return timer_event->timer_id_;
}

bool TimeWheel::delete_timer(uint32_t timer_id) {
	std::unique_lock<std::mutex> lock(timer_mutex_);

	for (int i = 0; i < CountArray(timer_type_); i++) {
		auto& ll = timer_type_[i];
		for (auto it = ll.begin(); it != ll.end(); it++) {
			if ((*it)->timer_id_ == timer_id) {
				release_timer(*it);
				ll.erase(it);
				return true;
			}
		}
	}

	return false;
}

inline TimeWheel::TimerEvent* TimeWheel::create_timer() {
	TimerEvent* timer_event = nullptr;
	if (timer_storage_.empty()) {
		try
		{
			timer_event = new TimerEvent;
			timer_all_.push_back(timer_event);
		}
		catch (...)
		{
			// log error
			exit(1);
		}
	}
	else {
		timer_event = timer_storage_.front();
		timer_storage_.pop_front();
	}
	memset(timer_event, 0, sizeof(*timer_event));
	return timer_event;
}

inline void TimeWheel::release_timer(TimerEvent* timer_event) {
	timer_storage_.push_back(timer_event);
}
