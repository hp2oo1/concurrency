#include <vector>
#include <atomic>
#include <thread>
#include <iostream>

using namespace std;

vector<int> queue_data;
atomic<int> count;

void populate_queue() {
    unsigned const number_of_items=20;
    queue_data.clear();
    for(unsigned i=0; i<number_of_items; ++i) {
        queue_data.push_back(i);
    }
    count.store(number_of_items,memory_order_release);
}

void consume_queue_items(int id) {
    unsigned const max_iterations = 100;
    unsigned i = -1;
    while( ++i<max_iterations ) {
        int item_index;
        if((item_index=count.fetch_sub(1,memory_order_acquire))<=0) {
            //wait_for_more_items();
            continue;
        }
        //process(queue_data[item_index-1]);
        cout<<id*queue_data[item_index-1]<<" ";
    }
}

int main() {
    thread a(populate_queue);
    thread b(consume_queue_items,1);
    thread c(consume_queue_items,10);
    a.join();
    b.join();
    c.join();
}
