/*
 * Author:  kid7st (), kid7st@gmail.com
 */
#ifndef _POOL_H 
#define _POOL_H

#include <iostream>
#include <queue>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <boost/shared_ptr.hpp>

using namespace std;

class Factory;

class Worker{
    public:
        Worker(Factory *factory);
        virtual ~Worker(){};
        
        virtual void work();
        static void* RunHelper(void*);

        pthread_t tid;
    private:
        Factory *factory;
};

class Job{
    public:
        Job(string command, string parameter);
        virtual ~Job(){};
        virtual void doJob();
    private:
        string command;
        string parameter;
};

class Factory{
    public:
        Factory(const int &pthread_num);
        ~Factory(){};
        
        void start();
        void destroy();
        bool insert(const boost::shared_ptr<Job> &job);

        bool isActive(){
            return active;
        };
 
        pthread_mutex_t *queue_mutex;
        pthread_cond_t *queue_not_empty;
        pthread_cond_t *queue_empty;
        queue< boost::shared_ptr<Job> > job_queue;
    private:
        bool active;
        const int pthread_num;
        vector< boost::shared_ptr<Worker> > workers;
};

#endif

