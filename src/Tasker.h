/**
 *
 *		Tasker for Arduino
 *
 *
 * 
 */

#ifndef TASKER_H
 #define TASKER_H

#include <Arduino.h>
#include <functional>
#include <memory>
#include <list>




class Task {
public:
	enum Task_t: uint8_t { ASYNC, SYNC }; 
	enum Task_state: uint8_t { INIT, WAITING, RUNNING, FINISHED }; 
	typedef std::function<void(Task & t)> cb_t; 
	typedef std::function<void(void)> endFn_t; 
	typedef std::list<std::unique_ptr<Task>> st_t; 
	Task() {}; 
	~Task(); 
	Task(cb_t cb);  
	Task & add(cb_t cb = nullptr); 
	bool remove(const Task * task);
	bool run(bool override = true);  /*  override gives repeat behaviour */ 
	void runNow();
	Task & setType(Task_t type); 
	Task & setTimeout(uint32_t timeout);
	Task & setRepeat(bool repeat);
	Task & setRepeat(int repeat); 
	Task & setDelete(bool deletable); 
	Task & onEnd(endFn_t Fn); 
	Task & setName(String name); 
	Task & setMicros(bool useMicros); 
	Task_state getState(); 
	uint32_t getTimeout(); 
	String  getName(); 
	void dump(Stream & stream, int indent = 0); 
	bool canDelete(); 

	void reset(); 
	bool finished();

private:
	Task_t _type{ASYNC}; 
	Task_state _state{INIT}; 
	cb_t _cb; 
	endFn_t _endCb; 
	st_t _storage; 
	uint32_t _lastrun{0}; 
	uint32_t _timeout{0}; 
	bool _repeat{false};
	uint32_t _remaining{1}; 
	String _name; 
	bool _useMicros{false}; 
	st_t::iterator _it; 
	bool _canDelete{true}; 

};

