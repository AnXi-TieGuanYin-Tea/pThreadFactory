/*
 * Author:  kid7st (), kid7st@gmail.com
 */

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(...) do{fprintf(stderr, __VA_ARGS__);} while(false);
#else 
#define DEBUG_PRINT(...) do{}while(false);
#endif

#include "pool.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <boost/shared_ptr.hpp>

Worker::Worker(Pool *pool)
    :pool(pool){
}

void* Worker::RunHelper(void *ptr){
    Worker* worker = (Worker*)ptr;
    DEBUG_PRINT("RUN IN WORKER\n");
    worker->work();
}

void Worker::work(){
    while(true){
        pthread_mutex_lock(pool->queue_mutex);

        while(pool->job_queue.empty() && pool->isActive()){
            pthread_cond_wait(pool->queue_not_empty, pool->queue_mutex); 
        }

        if(!pool->isActive()){
            pthread_mutex_unlock(pool->queue_mutex);
            pthread_exit(NULL);
        }

        boost::shared_ptr<Job> job = pool->job_queue.front();
        pool->job_queue.pop();
        if(pool->job_queue.empty()){
            pthread_cond_signal(pool->queue_empty);
        }
        pthread_mutex_unlock(pool->queue_mutex);

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

Pool::Pool(const int &pthread_num)
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

void Pool::start(){
    active = true;
}

void Pool::destroy(){
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

bool Pool::insert(const boost::shared_ptr<Job> &job){
    DEBUG_PRINT("INSERTING JOB\n");

    pthread_mutex_lock(queue_mutex);
    job_queue.push(job);
    pthread_mutex_unlock(queue_mutex);
    pthread_cond_signal(queue_not_empty);

    return true;
}
