#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
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

mutex g_mutex;
int g_count(0);
int g_sum(0);

void producer(Buffer& buffer, int id) {
    int i=-1;
    while(++i<100) {
        int data = i;
        buffer.push(data);
        {
            lock_guard<mutex> lock(g_mutex);
            cout<<++g_count<<" "<<id<<" produce "<<data<<"\n";
            // check
            g_sum += data;
        }
    }
}

void process(int &data) {
    this_thread::sleep_for(std::chrono::milliseconds(100));
    data*=-1;
}

void consumer(Buffer& buffer, int id) {
    int i=-1;
    while(++i<100) {
        int data;
        buffer.pop(data);
        // concurrently executing process
        process(data);
        {
            lock_guard<mutex> lock(g_mutex);
            cout<<++g_count<<" "<<id<<" consume "<<data<<"\n";
            // check
            g_sum += data;
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
    cout<<"g_sum: "<<g_sum<<endl;
}
