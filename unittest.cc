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
#include <gtest/gtest.h>

using namespace std;

class pThreadPoolTest: public testing::Test{
};

TEST_F(pThreadPoolTest, Test_Pool){
    Pool pool(20); 
    pool.start();
    EXPECT_TRUE(pool.isActive());
    for(int i = 0; i < 100; ++i){
        char buff[100];
        sprintf(buff, "%d", i);
        boost::shared_ptr<Job> job(new Job("echo", buff));
        pool.insert(job); 
    }

    sleep(30);
    EXPECT_TRUE(pool.job_queue.empty());
    pool.destroy();
}
int main(int argc, char **argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

