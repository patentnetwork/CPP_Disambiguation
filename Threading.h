/*
 * Threading.h
 *
 *  Created on: Oct 6, 2010
 *      Author: ysun
 */

#ifndef THREADING_H_
#define THREADING_H_

#include <pthread.h>
#include <unistd.h>

/*
 * Threading Base
 */
class Runnable
{
public:
    virtual void run() = 0;
    virtual ~Runnable() {};
};

/*
 * Thread class
 * Steps to do multithreading:
 * 1. Create a worker class that inherits from Thread class ( synchronization objects can also be in the class ).
 * 2. The worker class should override the "void run()" function. DO NOT GET CONFUSED!
 * 3. At the place where multi-threading is expected, create worker OBJECTS.
 * 4. Start multi-threading by calling the method "bool start()" from worker objects.
 * 5. Block the main thread until child threads finish by calling "void join()" or void join(unsigned long)" from worker objects.
 *
 */
class Thread : public Runnable
{
private:
    static int threadInitNumber;
    int curThreadInitNumber;
    Runnable *target;
    pthread_t tid;
    int threadStatus;
    pthread_attr_t attr;
    sched_param param;
    static void* run0(void* pVoid);
    void* run1();
    static int getNextThreadNum();

public:
    static const int THREAD_STATUS_NEW = 0;
    static const int THREAD_STATUS_RUNNING = 1;
    static const int THREAD_STATUS_EXIT = -1;
    Thread();
    Thread(Runnable *iTarget);
    ~Thread();
    void run();
    bool start();
    int getState();
    void join();
    void join(unsigned long millisTime);
    bool operator ==(const Thread *otherThread);
    pthread_t getThreadID();
    static pthread_t getCurrentThreadID();
    static bool isEquals(Thread *iTarget);
    void setThreadScope(bool isSystem);
    bool getThreadScope();
    void setThreadPriority(int priority);
    int getThreadPriority();

};


#endif /* THREADING_H_ */


