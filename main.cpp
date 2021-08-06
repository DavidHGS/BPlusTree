#include <iostream>
#include <time.h>
#include <unistd.h>
#include <string>
#include "BPlus_Node.h"
using namespace std;

#define M 50
#define N 100

unsigned long GetTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

int main()
{
    BPlusTree bt;
    unsigned long t1, t2;
    t1 = GetTickCount();
    //插入测试
    printf("----------------------insert test------------------\n");
    for (int i = 1; i <= M; i++)
    {
        int m = i;
        usleep(20);
        bt.Insert(m, (double)m);
    }
    t2 = GetTickCount();
    bt.PrintLeaves();
    printf("Insert for %d times:%d ms,Average:%lf ms\n", M, t2 - t1, (t2 - t1) * 1.0 / M);
     //查找测试
    printf("----------------------search test------------------\n");
    t1 = GetTickCount();
    for (int i = 1; i <= N; i++)
    {
        usleep(20);
       
        if (bt.search(i))
        {
            printf("B+ exsit:%d\n", i);
        }
        else
        {
            printf("B+ not exsit:%d\n", i);
        }
    }
    t2 = GetTickCount();
    printf("search for %d times:%d ms,Average:%lf ms\n", N, t2 - t1, (t2 - t1) * 1.0 / N);

    //修改测试
    printf("----------------------Update test------------------\n");
    t1 = GetTickCount();
    for (int i = 1; i <= M; i++)
    {
        int n = i;
        bt.Update(n,(double)n+1);
        usleep(20);
        bt.PrintLeaves();
    }
    t2 = GetTickCount();
    printf("update for %d times:%d ms,,Average:%lf ms\n\n", M, t2 - t1, (t2 - t1) * 1.0 / M);
    //删除测试
    printf("----------------------delete test------------------\n");

    t1 = GetTickCount();
    for (int i = 1; i <= M; i++)
    {
        int n = i;
        bt.Remove(n);
        usleep(20);
        bt.PrintLeaves();
    }
    t2 = GetTickCount();
    printf("delete for %d times:%d ms,,Average:%lf ms\n\n", M, t2 - t1, (t2 - t1) * 1.0 / M);
}