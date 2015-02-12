#include "threading_pthread.h"

using namespace std;


#if defined(__hpux)
#   define PTHREAD_NULL cma_c_null
#else  // !defined(__hpux)
#   define PTHREAD_NULL NULL
#endif // !defined(__hpux)

// ----------------------------------------------------------------------------
//  CLASS : sc_thread_pool_pthread
//
//  Thread pool class implemented with Posix Threads.
// ----------------------------------------------------------------------------

//#define INSTRUMENT_THREAD_POOL
#ifdef INSTRUMENT_THREAD_POOL
#include <time.h>
#define CAPTURE_TIME(t) clock_gettime(CLOCK_MONOTONIC,&t)
#define DECLARE_TIME(...) struct timespec __VA_ARGS__
#define COMPARE_TIME(str,a,b) printf ("%s %ld\n", str, b.tv_nsec - a.tv_nsec)
#undef INSTRUMENT_THREAD_POOL
#else
#define CAPTURE_TIME(t)
#define DECLARE_TIME(...)
#define COMPARE_TIME(str,a,b)
#endif

void *worker_main_loop (void *arg)
{
    struct sc_thread_pool_worker_args *args = (sc_thread_pool_worker_args *)arg;
    sc_thread_pool_pthread *pool = args->pool;
    
    while (true)
    {
        pthread_mutex_lock (&pool->m_mainmutex);
        
        // indicate that the thread is about to block
        pool->m_active_workers--;
        pthread_cond_signal (&pool->m_maincond);
        
        // block the thread (also has the effect of unlocking the mutex, so
        // that the main thread can recieve the signal we just sent)
        pthread_cond_wait (&pool->m_threadcond, &pool->m_mainmutex);
        
        // thread has been woken up, main thread is blocking now
        pthread_mutex_unlock (&pool->m_mainmutex);
        
        DECLARE_TIME (a, b);
        CAPTURE_TIME (a);
        pool->m_callback (pool->m_callback_arg);
        CAPTURE_TIME (b);
        COMPARE_TIME ("actual callback execution time", a, b);
    }
    
    return NULL; // never happens
}

sc_thread_pool_pthread::sc_thread_pool_pthread (void *(*callback) (void *), void *callback_arg)
    : sc_thread_pool (callback, callback_arg), m_callback_arg (callback_arg)
{
    if (m_num_threads == 1)
        return;

    pthread_mutex_init (&m_mainmutex, PTHREAD_NULL);
    pthread_cond_init (&m_maincond, PTHREAD_NULL);
    pthread_cond_init (&m_threadcond, PTHREAD_NULL);
    
    m_threads = new pthread_t[m_num_threads];
    m_worker_args = new sc_thread_pool_worker_args[m_num_threads];
    
    m_active_workers = m_num_threads;
    
    for (int i = 0; i < m_num_threads; i++)
    {
        m_worker_args[i].pool = this;
        m_worker_args[i].threadnum = i;
		assert (pthread_create (&m_threads[i], NULL, worker_main_loop, (void *)&m_worker_args[i]) != -1);
	}
	
	block_on_workers ();
}

// block until all the worker threads have started blocking
inline void sc_thread_pool_pthread::block_on_workers ()
{
    pthread_mutex_lock (&m_mainmutex);
    while (m_active_workers > 0)
        pthread_cond_wait (&m_maincond, &m_mainmutex);
}

void sc_thread_pool_pthread::execute ()
{
    // don't use the workers if threading is disabled
    if (m_num_threads == 1)
    {
        m_callback (m_callback_arg);
        return;
    }

    DECLARE_TIME (a, b, c);
    CAPTURE_TIME (a);

    // wake up all the workers
    m_active_workers = m_num_threads;
    pthread_cond_broadcast (&m_threadcond);
    pthread_mutex_unlock (&m_mainmutex);
    
    CAPTURE_TIME (b);
    
    block_on_workers ();

    CAPTURE_TIME (c);
    COMPARE_TIME ("minimum time before wakeup", a, b);
    COMPARE_TIME ("overall execution time", a, c);
}

sc_thread_pool_pthread::~sc_thread_pool_pthread ()
{
    if (m_num_threads == 1)
        return;

    for (int i = 0; i < m_num_threads; i++)
		assert (pthread_cancel (m_threads[i]) != -1);
    delete[] m_threads;
    delete[] m_worker_args;
	pthread_cond_destroy (&m_maincond);
	pthread_cond_destroy (&m_threadcond);
	pthread_mutex_destroy (&m_mainmutex);
}

// ----------------------------------------------------------------------------
//  CLASS : sc_mutex_pthread
//
//  Mutex class implemented with Posix Threads for queue management.
// ----------------------------------------------------------------------------

// constructor
sc_mutex_pthread::sc_mutex_pthread()
{
	pthread_mutex_init(&mutex, PTHREAD_NULL);
}

sc_mutex_pthread::~sc_mutex_pthread()
{
	pthread_mutex_destroy(&mutex);

}

void sc_mutex_pthread::lock()
{
	pthread_mutex_lock(&mutex);
}

void sc_mutex_pthread::unlock()
{
	pthread_mutex_unlock(&mutex);
}
