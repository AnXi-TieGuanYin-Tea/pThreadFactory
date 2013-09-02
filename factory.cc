/*
 * Author:  kid7st (), kid7st@gmail.com
 */

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(...) do{fprintf(stderr, __VA_ARGS__);} while(false);
#else 
#define DEBUG_PRINT(...) do{}while(false);
#endif

#include "factory.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <boost/shared_ptr.hpp>

Worker::Worker(Factory *factory)
    :factory(factory){
}

void* Worker::RunHelper(void *ptr){
    Worker* worker = (Worker*)ptr;
    DEBUG_PRINT("RUN IN WORKER\n");
    worker->work();
}

void Worker::work(){
    while(true){
        pthread_mutex_lock(factory->queue_mutex);

        while(factory->job_queue.empty() && factory->isActive()){
            pthread_cond_wait(factory->queue_not_empty, factory->queue_mutex); 
        }

        if(!factory->isActive()){
            pthread_mutex_unlock(factory->queue_mutex);
            pthread_exit(NULL);
        }

        boost::shared_ptr<Job> job = factory->job_queue.front();
        factory->job_queue.pop();
        if(factory->job_queue.empty()){
            pthread_cond_signal(factory->queue_empty);
        }
        pthread_mutex_unlock(factory->queue_mutex);

        DEBUG_PRINT("GET A JOB TO RUN: %x\n", pthread_self());
        job->doJob();
    }
}

Job::Job(string command, string parameter)
    :command(command), parameter(parameter){
}

void Job::doJob(){
    cout<<parameter<<endl;
    sleep(1);
}

Factory::Factory(const int &pthread_num)
:pthread_num(pthread_num){
    queue_mutex = new pthread_mutex_t;
    pthread_mutex_init(queue_mutex, NULL);
    queue_not_empty = new pthread_cond_t;
    pthread_cond_init(queue_not_empty, NULL);
    queue_empty = new pthread_cond_t;
    pthread_cond_init(queue_empty, NULL);
    active = false;

    for(int i = 0; i<pthread_num; ++i){
        boost::shared_ptr<Worker> worker(new Worker(this));
        workers.push_back(worker);
        pthread_create(&worker->tid, NULL, Worker::RunHelper, worker.get());
    } 
    DEBUG_PRINT("FINISH POOL INIT\n");
}

void Factory::start(){
    active = true;
}

void Factory::destroy(){
    pthread_mutex_lock(queue_mutex);
    while(!job_queue.empty()){
        pthread_cond_wait(queue_empty, queue_mutex);
    }
    active = false;
    pthread_mutex_unlock(queue_mutex);

    pthread_cond_broadcast(queue_not_empty);
    for(vector< boost::shared_ptr<Worker> >::iterator it = workers.begin(); it != workers.end(); ++it){
        pthread_join((*it)->tid, NULL);
    }
    
    pthread_mutex_destroy(queue_mutex);
    pthread_cond_destroy(queue_empty);
    pthread_cond_destroy(queue_not_empty);

    return;
}

bool Factory::insert(const boost::shared_ptr<Job> &job){
    DEBUG_PRINT("INSERTING JOB\n");

    pthread_mutex_lock(queue_mutex);
    job_queue.push(job);
    pthread_mutex_unlock(queue_mutex);
    pthread_cond_signal(queue_not_empty);

    return true;
}
