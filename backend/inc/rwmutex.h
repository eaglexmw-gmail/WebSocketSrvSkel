#ifndef _RW_MUTEX_H_
#define _RW_MUTEX_H_

#include <condition_variable>

template<class T>
class RWMutexT {
public:
	RWMutexT() : rw_nwaitreaders(0), rw_nwaitwriters(0), rw_refcount(0) { } 
	~RWMutexT() = default;
	RWMutexT(const RWMutexT&) = delete;
	RWMutexT& operator=(const RWMutexT&) = delete;

public:
	// 阻塞获取读锁
	void read_lock()    /* wait for reader lock */
	{
		rw_mutex.lock();
		{
			/* give preference to waiting writers */
			while (rw_refcount < 0 || rw_nwaitwriters > 0) { // 写优先
				rw_nwaitreaders++;
				rw_condreaders.wait(rw_mutex);
				rw_nwaitreaders--;
			}
			rw_refcount++;  /* another reader has a read lock */
		}
		rw_mutex.unlock();
	}
	// 尝试获取读锁，失败立即返回
	bool read_trylock() /* try to get reader lock */
	{
		bool res = true;
		rw_mutex.lock();
		{
			if (rw_refcount < 0 || rw_nwaitwriters > 0) { // 写优先
				res = false; /* held by a writer or waiting writers */
			}
			else {
				rw_refcount++; /* increment count of reader locks */
			}
		}
		rw_mutex.unlock();
		return res;
	}
	// 阻塞获取写锁
	void write_lock()    /* wait for writer lock */
	{
		rw_mutex.lock();
		{
			while (rw_refcount != 0) { /* wait other readers release the rd or wr lock */
				rw_nwaitwriters++;
				rw_condwriters.wait(rw_mutex);
				rw_nwaitwriters--;
			}
			rw_refcount = -1; /* acquire the wr lock */
		}
		rw_mutex.unlock();
	}
	// 尝试获取写锁，失败立即返回
	bool write_trylock() /* try to get writer lock */
	{
		bool res = true;
		rw_mutex.lock();
		{
			if (rw_refcount != 0) /* the lock is busy */
				res = false;
			else
				rw_refcount = -1; /* acquire the wr lock */
		}
		rw_mutex.unlock();
		return res;
	}
	void read_unlock() {unlock();}
	void write_unlock() {unlock();}

protected:
	// 释放写锁或读锁
	void unlock()    /* release reader or writer lock */
	{
		rw_mutex.lock();
		{
			if (rw_refcount > 0)
				rw_refcount--;
			else if (rw_refcount == -1)
				rw_refcount = 0;
			else
				// unexpected error
				fprintf(stderr, "RWLock::unlock unexpected error. rw_refcount = %d\n", rw_refcount);
			
			/* give preference to waiting writers over waiting readers */
			if (rw_nwaitwriters > 0) {
				if (rw_refcount == 0) {
					rw_condwriters.notify_one();
				}
			}
			else if (rw_nwaitreaders > 0) {
				rw_condreaders.notify_all(); /* rw lock is shared */
			}
		}
		rw_mutex.unlock();
	}

private:
	T rw_mutex;
	std::condition_variable_any rw_condreaders;
	std::condition_variable_any rw_condwriters;
	int rw_nwaitreaders;                        /* the number of waiting readers */
	int rw_nwaitwriters;                        /* the number of waiting writers */
	int rw_refcount;                            /* 0: not locked; -1: locked by one writer; > 0: locked by rw_refcount readers */
};

#include <mutex>
typedef RWMutexT<std::mutex> RWMutex;

#endif
