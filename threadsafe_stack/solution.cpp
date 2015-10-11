#include <stack>
#include <thread>
#include <mutex>
#include <iostream>
#include <iterator>

using namespace std;

template<typename T>
class threadsafe_stack {
    stack<T> data;
    mutable mutex m;
public:
    threadsafe_stack() {};
    threadsafe_stack(const threadsafe_stack& other) {
        lock_guard<mutex> lock(other.m);
        data = other.data;
    }
    threadsafe_stack& operator=(const threadsafe_stack&) = delete;
    void push(T value) {
        lock_guard<mutex> lock(m);
        data.push(value);
    }
    shared_ptr<T> pop() {
        lock_guard<mutex> lock(m);
        if(data.empty()) throw;
        shared_ptr<T> const p = make_shared<T>(data.top());
        data.pop();
        return p;
    }
    void pop(T &value) {
        lock_guard<mutex> lock(m);
        if(data.empty()) throw;
        value = data.top();
        data.pop();
    }
    bool empty() const {
        lock_guard<mutex> lock(m);
        return data.empty();
    }
    void print() const {
        lock_guard<mutex> lock(m);
        stack<T> data_copy(data);
        while(!data_copy.empty()) {
            cout<<data_copy.top()<<" ";
            data_copy.pop();
        }
        cout<<endl;
    }
};

template<typename T>
void do_some_task(const T& value) {
    for(int i=0; i<100; ++i) {
        cout<<value<<" ";
    }
}

template<typename T>
void f(threadsafe_stack<T>& ts, int id) {
    do_some_task(*ts.pop());
}

int main() {
    threadsafe_stack<int> mystack;
    for(int i=0; i<10; ++i)
        mystack.push(i);
    thread t1(f<int>,ref(mystack),1);
    thread t2(f<int>,ref(mystack),2);
    t1.join();
    t2.join();
}
