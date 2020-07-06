#ifndef CIRCLE_QUEUE_H
#define CIRCLE_QUEUE_H
#include <vector>

template <class T>
class Circle_Queue
{
public:
    Circle_Queue(int queueCapacity);
    virtual ~Circle_Queue();//销毁队列
    void ClearQueue();//清空队列
    bool QueueEmpty();//判空队列
    bool QueueFull();//判满队列
    int QueueLength() const;//队列长度
    bool EnQueue(T element);//新元素入队
    bool DeQueue(T &element);//首元素出队
    int get_tail();
    int GetQueneFullLength();
    std::vector<T> QueueTraverse(int cnt);//遍历队列，遍历cnt个数据

private:
    T *m_pQueue;//队列数组指针
    int m_iQueueLen;//队列元素个数
    int m_iQueueCapacity;//队列数组容量
    int m_iHead;
    int m_iTail;
    std::vector<T> data;
};


template <class T>
Circle_Queue<T>::Circle_Queue(int queueCapacity)
{

    m_iQueueCapacity = queueCapacity;
    m_pQueue = new T[m_iQueueCapacity];
    ClearQueue();

}

template <class T>
Circle_Queue<T>::~Circle_Queue()
{
    delete[] m_pQueue;
    m_pQueue = nullptr;
}

template <class T>
void Circle_Queue<T>::ClearQueue()
{
    m_iHead = 0;
    m_iTail = 0;
    m_iQueueLen = 0;
}


template <class T>
bool Circle_Queue<T>::QueueEmpty()
{
    return m_iQueueLen == 0 ?  true : false;
}


template <class T>
bool Circle_Queue<T>::QueueFull()
{
    return m_iQueueLen == m_iQueueCapacity ? true : false;
}



template <class T>
bool Circle_Queue<T>::EnQueue(T element)
{
    if (QueueFull())
    {
        T mr;
        DeQueue(mr);

        m_pQueue[m_iTail] = element;
        m_iTail++;
        m_iTail = m_iTail % m_iQueueCapacity;
        m_iQueueLen++;
        return false;
    }
    else {
        m_pQueue[m_iTail] = element;
        m_iTail++;
        m_iTail = m_iTail % m_iQueueCapacity;
        m_iQueueLen++;
        return true;
    }
}


template <class T>
bool Circle_Queue<T>::DeQueue(T &element) //传入引用是为了可以直接修改实参的值，
{
    if (QueueEmpty())
    {
        return false;
    }
    else {
        element = m_pQueue[m_iHead] ;
        m_iHead++;
        m_iHead = m_iHead % m_iQueueCapacity;
        m_iQueueLen--;
        return true;
    }
}

template <class T>
int Circle_Queue<T>::get_tail()
{
    return m_iTail;

}

template <class T>
int Circle_Queue<T>::GetQueneFullLength()
{
    return m_iQueueCapacity;

}

template <class T>
std::vector<T> Circle_Queue<T>::QueueTraverse(int cnt)
{
    int head;
    if(cnt>=m_iQueueLen)
    {
        head = m_iHead;
        cnt = m_iQueueLen;
    }
    else
    {
        head = (m_iHead + m_iQueueLen - cnt)%m_iQueueCapacity;
    }

    data.clear();
    for (int i = head; i < head + cnt; i++)
    {
        data.append(m_pQueue[i%m_iQueueCapacity]);
    }

    return data;
}

template <class T>
int Circle_Queue<T>::QueueLength() const
{
    return m_iQueueLen;
}

#endif // CIRCLE_QUEUE_H
