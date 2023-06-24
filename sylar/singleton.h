#ifndef __SYLAR_SINGLETON_H__
#define __SYLAR_SINGLETON_H__
#include<memory>

namespace sylar{

/**
 * @brief 单例模式
*/
template<class T, class X = void, int N = 0>
class Singleton{
public:
    static T* Getinstance(){
        static T v;
        return &v;
    }
};

template<class T, class X = void, int N = 0>
class SingletonPtr{
public:
    static std::shared_ptr<T> Getinstance(){
        static std::shared_ptr<T> v(new T) ;
        return v;
    }
};

}


#endif