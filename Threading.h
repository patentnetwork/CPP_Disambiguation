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
 * 线程运行实体类
 */
class Runnable
{
public:
    //运行实体
    virtual void run() = 0;
    virtual ~Runnable() {};
};

/*
 * 线程类
 *
 */
class Thread : public Runnable
{
private:
    //线程初始化序号
    static int threadInitNumber;
    //当前线程初始化序号
    int curThreadInitNumber;
    //线程体
    Runnable *target;
    //当前线程的线程ID
    pthread_t tid;
    //线程的状态
    int threadStatus;
    //线程属性
    pthread_attr_t attr;
    //线程优先级
    sched_param param;
    //获取执行方法的指针
    static void* run0(void* pVoid);
    //内部执行方法
    void* run1();
    //获取一个线程序号
    static int getNextThreadNum();

public:
    //线程的状态－新建
    static const int THREAD_STATUS_NEW = 0;
    //线程的状态－正在运行
    static const int THREAD_STATUS_RUNNING = 1;
    //线程的状态－运行结束
    static const int THREAD_STATUS_EXIT = -1;
    //构造函数
    Thread();
    //构造函数
    Thread(Runnable *iTarget);
    //析构
    ~Thread();
    //线程的运行实体
    void run();
    //开始执行线程
    bool start();
    //获取线程状态
    int getState();
    //等待线程直至退出
    void join();
    //等待线程退出或者超时
    void join(unsigned long millisTime);
    //比较两个线程时候相同,通过 curThreadInitNumber 判断
    bool operator ==(const Thread *otherThread);
    //获取This线程ID
    pthread_t getThreadID();
    //获取当前线程ID
    static pthread_t getCurrentThreadID();
    //当前线程是否和某个线程相等,通过 tid 判断
    static bool isEquals(Thread *iTarget);
    //设置线程的类型:绑定/非绑定
    void setThreadScope(bool isSystem);
    //获取线程的类型:绑定/非绑定
    bool getThreadScope();
    //设置线程的优先级,1-99,其中99为实时;意外的为普通
    void setThreadPriority(int priority);
    //获取线程的优先级
    int getThreadPriority();

};


#endif /* THREADING_H_ */

#if 0
//usage of the thread class
#include "Threading.h"
#include <iostream.h>

class MultiThread : public Thread
{
public:

    Thread *th1;
    Thread *th2;

    void Test()
    {
        th1 = new Thread(this);
        th1->setThreadPriority(99);
        th2 = new Thread(this);
        start();
        th1->start();
        th2->start();
        th1->join();
        th2->join();
    }

    void run()
    {
        //Thread->isEquals(th1)
        if (Thread::isEquals(th1))
        {
            int number = 100;
            for (int i = 0; i < 10; i++)
            {
                cout << "this is thread1 number is " << number++;
                cout << " \tpid is " << getpid() << " tid is " << getCurrentThreadID() << "  Priority:" << th1->getThreadPriority() << endl;
                sleep(1);
            }
        }else if (Thread::isEquals(th2))
        {
            int number = 200;
            for (int i = 0; i < 10; i++)
            {
                cout << "this is thread2 number is " << number++;
                cout << " \tpid is " << getpid() << " tid is " << getCurrentThreadID() << "  Priority:" << th2->getThreadPriority() << endl;
                sleep(1);
            }
        }else if (Thread::isEquals(this))
        {
            int number = 300;
            for (int i = 0; i < 10; i++)
            {
                cout << "this is thread0 number is " << number++;
                cout << " \tpid is " << getpid() << " tid is " << getCurrentThreadID() << "  Priority:" << this->getThreadPriority() << endl;
                sleep(1);
            }
        }

    }
};

/*
 *
 */
int main(int argc, char** argv)
{
    bool ret;
    MultiThread *mt;
    mt = new MultiThread();
    mt->Test();
    return (EXIT_SUCCESS);
}
#endif



