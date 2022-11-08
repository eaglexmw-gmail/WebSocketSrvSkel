#ifndef _READ_WRITE_MUTEX_H_
#define _READ_WRITE_MUTEX_H_

#include <condition_variable>

template<class T>
class ReadWriteMutexT{
public:
	//ReadWriteMutex() = default;
	ReadWriteMutexT(size_t read_count = 0, size_t write_count = 0, bool writing = false)
	: _read_count(read_count), _write_count(write_count), _writing(writing){ }
	~ReadWriteMutexT() = default;

	ReadWriteMutexT(const ReadWriteMutex&) = delete;
	ReadWriteMutexT& operator=(const ReadWriteMutex&) = delete;

	ReadWriteMutexT(const ReadWriteMutex&&) = delete;
	ReadWriteMutexT& operator=(const ReadWriteMutex&&) = delete;

	void read_lock(){
		std::unique_lock<std::mutex> lck(mutex_);
		_read_cv.wait(lck, [this]()-> bool {
			return _write_count == 0;
		});
		++_read_count;
	}
	bool read_trylock(){
		// 先锁住
		if (!mutex_.try_lock()){
			return false;
		}
		if (_write_count == 0) {
			mutex_.unlock();
			return false;
		}
		_read_count ++;
		mutex_.unlock();
		return true;
	}

	void read_unlock(){
		std::unique_lock<std::mutex> lck(mutex_);
		if(--_read_count == 0 && _write_count > 0){
			_write_cv.notify_one();
		}
	}

	void write_lock(){
		std::unique_lock<std::mutex> lck(mutex_);
		++_write_count;
		_write_cv.wait(lck, [this]()-> bool {
			return _read_count == 0 && !_writing;
		});
		_writing = true;
	}
	bool write_trylock(){
		// 先尝试锁住
		if (!mutex_.try_lock()){
			return false;
		}
		if ((_read_count != 0) || (_writing)) {
			mutex_.unlock();
			return false;
		}
		++_write_count;
		_writing = true;
		// 
		mutex_.unlock();
		return true;
	}

	void write_unlock(){
		std::unique_lock<std::mutex> lck(mutex_);
		if(--_write_count == 0){
			_read_cv.notify_all();	
		}else{
			_read_cv.notify_one();
		}
		_writing = false;
	}
private:
	volatile size_t         _read_count;
	volatile size_t         _write_count;
	volatile bool           _writing;
	T      					mutex_;
	std::condition_variable _read_cv;
	std::condition_variable _write_cv;
};

#include <mutex>
typedef ReadWriteMutexT<std::mutex> ReadWriteMutex;

#endif
