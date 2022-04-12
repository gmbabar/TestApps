#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

queue<string> m_queue;
mutex mtx_;
condition_variable cv_;
bool Produced = false;

void Publisher() {
    std::string data;
    unique_lock<mutex> ul(mtx_);
    bool subscribed = false;
    while (true) {
        if (Produced) {
            cout << "\t--> Server Is Waiting For Client Response" << endl;
            cv_.wait(ul);
        }
        cout << __func__ << ":" << endl;
        cout << "\tData Recived From Subscriber : " << m_queue.front() << endl;

        if(m_queue.front() == "subscribe") {
            subscribed = 1;
            m_queue.pop();
            m_queue.push("Subscribed");
            Produced = true;
            cv_.notify_all();
        }
        else if(m_queue.front() == "unsubscribe") {
            subscribed = 0;
            m_queue.pop();
            m_queue.push("Unsubscribed");
            Produced = true;
            cv_.notify_all();
        }
        else {
            m_queue.push("Exchange Started");
            Produced = true;
            cv_.notify_all();
        }

    }
    
}

void Subscriber() {

    std::string data;
    unique_lock<mutex> ul(mtx_);
    while (true) {
        if (!Produced) {
            cout << "\t--> Waiting For Server Response" << endl;
            cv_.wait(ul);
        }
        Produced = false;

        cout << __func__ << ":" << endl;
        cout << "\tData Recived From Publisher : " << m_queue.front() << endl;

        if(m_queue.front() == "Subscribed") {
            cout << "\tSending Unsubscribe Message To Server" << endl;
            m_queue.push("unsubscribe");
            m_queue.pop();
            cv_.notify_all();
        }
        else {
            m_queue.pop();
            cout << "\tEnter Data TO Send TO Publisher : ";
            getline(cin, data);
            if(data == "quit" || data == "exit") return;
            m_queue.push(data);
            cv_.notify_one();
        }
    }
}

int main() {
    thread thrd(Publisher);
    thread thrd1(Subscriber);
    thrd.join();
    thrd1.join();
    return 0;
}


