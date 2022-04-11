#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include "parser.hpp"

using namespace std;

queue<string> m_queue;
mutex mtx_;
condition_variable cv_;
bool Produced = false;

void Publisher()
{
    std::string data;
    unique_lock<mutex> ul(mtx_);
    bool subscribed = false;
    while (true)
    {
        if (Produced)
        {
            cout << "\t--> Publisher Is Waiting For Consumer To Consume Data" << endl;
            cv_.wait(ul);
        }
        cout << __func__ << ":" << endl;
        cout << "\tData Recived From Subscriber : " << m_queue.front() << endl;

        if(m_queue.front() == "subscribe")
        {
            subscribed = 1;
            m_queue.pop();
            m_queue.push("Subscribed");
            Produced = true;
            cv_.notify_all();
        }
        else if(m_queue.front() == "unsubscribe")
        {
            subscribed = 0;
            m_queue.pop();
            m_queue.push("Unsubscribed");
            Produced = true;
            cv_.notify_all();
        }
        else
        {
            cout << "\tEnter Data TO Send TO Subscriber : ";
            getline(cin, data);

            m_queue.push(data);
            Produced = true;
            cv_.notify_all();
        }

    }
    
}

void Subscriber()
{

    std::string data;
    unique_lock<mutex> ul(mtx_);
    while (true)
    {
        if (!Produced)
        {
            cout << "\t--> Waiting For Producer TO Produce Data" << endl;
            cv_.wait(ul);
        }
        Produced = false;

        cout << __func__ << ":" << endl;
        cout << "\tData Recived From Publisher : " << m_queue.front() << endl;

        if(m_queue.front() == "Subscribed")
        {
            m_queue.push("unsubscribe");
            m_queue.pop();
            cv_.notify_all();
        }
        else
        {
            m_queue.pop();
            cout << "\tEnter Data TO Send TO Publisher : ";
            getline(cin, data);
            if(data == "quit" || data == "exit") return;
            m_queue.push(data);
            cv_.notify_one();
        }
    }
}


int main()
{
    thread thrd(Publisher);
    thread thrd1(Subscriber);
    thrd.join();
    thrd1.join();
    return 0;
}