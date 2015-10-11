#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

using namespace std;

class Buffer {
    queue<int> data_queue;
    const int max_size = 10;
    mutex m;
    condition_variable cv;
public:
    void push(int data) {
        {
            unique_lock<mutex> lock(m);
            cv.wait(lock,[this](){ return data_queue.size()<max_size; });
            data_queue.push(data);
        }
        cv.notify_all();
    }
    void pop(int& data) {
        {
            unique_lock<mutex> lock(m);
            cv.wait(lock,[this](){ return data_queue.size()>0; });
            data = data_queue.front();
            data_queue.pop();
        }
        cv.notify_all();
    }
};

mutex g_m;

void producer(Buffer& buffer, int id) {
    int i=0;
    while(++i<100) {
        int data = i;
        buffer.push(data);
        {
            lock_guard<mutex> lock(g_m);
            cout<<id<<" produce "<<data<<"\n";
        }
    }
}

void consumer(Buffer& buffer, int id) {
    int i=0;
    while(++i<100) {
        int data;
        buffer.pop(data);
        {
            lock_guard<mutex> lock(g_m);
            cout<<id<<" consume "<<data<<"\n";
        }
    }
}

int main() {
    Buffer buffer;
    thread t1(producer,ref(buffer),1);
    thread t2(producer,ref(buffer),2);
    thread t3(consumer,ref(buffer),3);
    thread t4(consumer,ref(buffer),4);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}
