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

/* The pThread class Worker to do the job */
class Worker{
    public:
        Worker(Factory *factory);
        virtual ~Worker(){};
        
        virtual void work();
        static void* RunHelper(void*);  //pthread_create() just can use the static function;

        pthread_t tid;
    private:
        Factory *factory;
};

/* Job Class define the job to be worked */
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
        bool insert(const boost::shared_ptr<Job> &job);     //insert a job to the job queue;

        bool isActive(){
            return active;
        };
 
        /* the mutex and condition for the queue for synchronization */
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
