#include "threading_base.h"
#include <pthread.h>

class sc_mutex_pthread;
typedef sc_mutex_pthread sc_kernel_mutex_t;
class sc_thread_pool_pthread;
typedef sc_thread_pool_pthread sc_thread_pool_t;
// no templated typedefs, so we use the preprocessor
#define sc_thread_local_pointer_t sc_thread_local_pointer_pthread


// ----------------------------------------------------------------------------
//  CLASS : sc_thread_local_pointer_pthread
//
//  A pointer type which can be set and retrieved independently for each
//  thread (Posix Threads implementation)
// ----------------------------------------------------------------------------
template < class T >
class sc_thread_local_pointer_pthread : public sc_thread_local_pointer<T>
{
public:
    sc_thread_local_pointer_pthread ();
    ~sc_thread_local_pointer_pthread ();
    void set (T *val);
    T *get ();
private:
    pthread_key_t m_key;
};

template < class T >
sc_thread_local_pointer_pthread<T>::sc_thread_local_pointer_pthread ()
{
    assert (pthread_key_create (&m_key, NULL) != -1);
}

template < class T >
sc_thread_local_pointer_pthread<T>::~sc_thread_local_pointer_pthread ()
{
    assert (pthread_key_delete (m_key) != -1);
}

template < class T >
void sc_thread_local_pointer_pthread<T>::set (T *val)
{
    assert (pthread_setspecific (m_key, val) != -1);
}

template < class T >
T *sc_thread_local_pointer_pthread<T>::get (void)
{
    return (T *)pthread_getspecific (m_key);
}



// ----------------------------------------------------------------------------
//  CLASS : sc_mutex_pthread
//
//  mutex class for Posix Threads
//
// Notes:
//   
// ----------------------------------------------------------------------------

class sc_mutex_pthread : public sc_kernel_mutex
{
public:
	// Constructor
	sc_mutex_pthread();
	
	// Destructor
	~sc_mutex_pthread();
	
	// Lock the mutex
	void lock();
	
	// Unlock the mutex
	void unlock();
	
private:
	pthread_mutex_t		mutex;        // Mutexes to hold queues.
};

// ----------------------------------------------------------------------------
//  CLASS : sc_threadpool_pthread
//
//  mutex class for Posix Threads
//
// Notes:
//   
// ----------------------------------------------------------------------------

class sc_thread_pool_pthread;
struct sc_thread_pool_worker_args
{
    sc_thread_pool_pthread *pool;
    int threadnum;
};

class sc_thread_pool_pthread : public sc_thread_pool
{
public:
	// Constructor
	sc_thread_pool_pthread (void *(*callback) (void *), void *callback_arg);
	
	// Destructor
	~sc_thread_pool_pthread ();
	
	// Run the callback
	void execute ();

private:
    inline void block_on_workers ();

private:
    void *m_callback_arg;
	pthread_t *m_threads;
	pthread_mutex_t m_mainmutex;
	pthread_cond_t m_maincond;
	pthread_cond_t m_threadcond;
	int m_active_workers;
	struct sc_thread_pool_worker_args *m_worker_args;
	friend void *worker_main_loop (void *arg);
};
