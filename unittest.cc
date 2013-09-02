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
#include <gtest/gtest.h>

using namespace std;

class pThreadFactoryTest: public testing::Test{
};

TEST_F(pThreadFactoryTest, Test_Factory){
    Factory factory(20); 
    factory.start();
    EXPECT_TRUE(factory.isActive());
    for(int i = 0; i < 100; ++i){
        char buff[100];
        sprintf(buff, "%d", i);
        boost::shared_ptr<Job> job(new Job("echo", buff));
        factory.insert(job); 
    }

    sleep(30);
    EXPECT_TRUE(factory.job_queue.empty());
    factory.destroy();
}
int main(int argc, char **argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

