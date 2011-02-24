#include "Threading.h"

int Thread::threadInitNumber = 1;

int Thread::getNextThreadNum()
{
    return threadInitNumber++;
}

void* Thread::run0(void* pVoid)
{
    Thread* p = (Thread*) pVoid;
    p->run1();
    return p;
}

void* Thread::run1()
{

    threadStatus = THREAD_STATUS_RUNNING;
    tid = pthread_self();
    run();
    threadStatus = THREAD_STATUS_EXIT;
    tid = 0;
    pthread_exit(NULL);
}

void Thread::run()
{
    if (target != NULL)
    {
        (*target).run();
    }
}

Thread::Thread()
{
    tid = 0;
    threadStatus = THREAD_STATUS_NEW;
    curThreadInitNumber = getNextThreadNum();
    pthread_attr_init(&attr);
}

Thread::Thread(Runnable *iTarget)
{
    target = iTarget;
    tid = 0;
    threadStatus = THREAD_STATUS_NEW;
    curThreadInitNumber = getNextThreadNum();
    pthread_attr_init(&attr);
}

Thread::~Thread()
{
    pthread_attr_destroy(&attr);
}

bool Thread::start()
{
    return pthread_create(&tid, &attr, run0, this) == 0;
}

pthread_t Thread::getCurrentThreadID()
{
    return pthread_self();
}

pthread_t Thread::getThreadID()
{
    return tid;
}

int Thread::getState()
{
    return threadStatus;
}

void Thread::join()
{
    if (tid > 0)
    {
        pthread_join(tid, NULL);
    }
}

void Thread::join(unsigned long millisTime)
{

    if (tid == 0)
    {
        return;
    }
    if (millisTime == 0)
    {
        join();
    }else
    {
        unsigned long k = 0;
        while (threadStatus != THREAD_STATUS_EXIT && k <= millisTime)
        {
            usleep(100);
            k++;
        }
    }
}

bool Thread::operator ==(const Thread *otherThread)
{
    if (otherThread == NULL)
    {
        return false;
    }
    if (curThreadInitNumber == (*otherThread).curThreadInitNumber)
    {
        return true;
    }
    return false;
}

bool Thread::isEquals(Thread *iTarget)
{
    if (iTarget == NULL)
    {
        return false;
    }
    return pthread_self() == iTarget->tid;
}

void Thread::setThreadScope(bool isSystem)
{
    if (isSystem)
    {
        pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    }else
    {
        pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
    }
}

bool Thread::getThreadScope()
{
    int scopeType = 0;
    pthread_attr_getscope(&attr, &scopeType);
    return scopeType == PTHREAD_SCOPE_SYSTEM;
}

void Thread::setThreadPriority(int priority)
{
    pthread_attr_getschedparam(&attr, &param);
    param.__sched_priority = priority;
    pthread_attr_setschedparam(&attr, &param);
}

int Thread::getThreadPriority()
{
    pthread_attr_getschedparam(&attr, &param);
    return param.__sched_priority;
}
