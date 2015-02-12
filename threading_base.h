#include <cassert>
#include <cstdlib>


// ----------------------------------------------------------------------------
//  CLASS : sc_thread_pool
//
//  Thread pool abstract base class.
// ----------------------------------------------------------------------------

class sc_thread_pool
{
public:
	sc_thread_pool(void *(*callback) (void *), void *callback_arg)
	{
		m_callback = callback;
		m_num_threads = 8; // TODO: make automatic. Hardcoded core limit for time being
	}
	
	virtual ~sc_thread_pool () {}
	
	virtual void execute () {}
	
protected:
	
	int m_num_threads;
	void *(*m_callback) (void *);
};

// ----------------------------------------------------------------------------
//  CLASS : sc_thread_local_pointer_pthread
//
//  A pointer type which can be set and retrieved independently for each
//  thread (abstract base class)
// ----------------------------------------------------------------------------
template < class T >
class sc_thread_local_pointer
{
public:
    sc_thread_local_pointer () {}
    virtual ~sc_thread_local_pointer () {}
    virtual void set (T *val) {}
    virtual T *get () {return (T *)NULL;}
};

// ----------------------------------------------------------------------------
//  CLASS : sc_kernel_mutex
//
//  Mutex abstract base class.
// ----------------------------------------------------------------------------

class sc_kernel_mutex
{
protected:
	// constructor
	sc_kernel_mutex() {}
	
public:
	
	//destructor
	virtual ~sc_kernel_mutex() {}
	
	virtual void lock() {}
	virtual void unlock() {}
	
};


