#ifndef FASTVECTOR_H_INCLUDED
#define FASTVECTOR_H_INCLUDED
#include <stdlib.h>
#include <string.h>
#include <thread>
namespace fast {
    template<typename T,int TOTAL_SIZE = 30>
    class vector{
        T array[TOTAL_SIZE];
        T* start = array;
        T* stop  = &array[TOTAL_SIZE-1];
        T* cursor = start;
        bool started = false;
    public:
        bool empty(void) {return (cursor==start);}
        T*& begin(void){return start;}
        T*& end(void){return cursor;}
        #define ck() if(!started)prepare()
        void prepare (void) {
            start = array;
            stop = &array[TOTAL_SIZE-1];
            cursor = start;
            started = true;
        }
        vector() {
            ck();
        }
        int size(void){ck();return ((float)(cursor-start)/(float)sizeof(T));}
        void push_back(T member,auto pointer) {ck();
            *pointer = member;
        }void push_back(T member){push_back(member,cursor);++cursor;}
        void pop(void){ck();--cursor;}
        void pop_back(void){pop();}
        void insert(auto& __dest,auto __begin,auto __end) {ck();
            for (; __begin != __end; ++__begin,++__dest) {
                push_back(*__begin,__dest);
            }
        }
        void clear(void){ck();cursor = start;}
        ~vector() {
            cursor = start;
        }
    };
}
#endif // FASTVECTOR_H_INCLUDED
