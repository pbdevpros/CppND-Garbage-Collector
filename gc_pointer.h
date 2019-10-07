#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"

template <class T, int size = 0>
class Pointer{
private:
    // refContainer maintains the garbage collection list.
    static std::list<PtrDetails<T> > refContainer;
    T *addr;
    bool isArray;
    unsigned arraySize; // size of the array
    static bool first; // true when first Pointer is created
    // Return an iterator to pointer details in refContainer.
    typename std::list<PtrDetails<T> >::iterator findPtrInfo(T *ptr);
public:
    typedef Iter<T> GCiterator;
    Pointer(){
        Pointer(NULL);
    }
    Pointer(T*);
    Pointer(const Pointer &);
    ~Pointer();
    static bool collect();
    // Overload assignment of pointer to Pointer.
    T *operator=(T *t);
    // Overload assignment of Pointer to Pointer.
    Pointer &operator=(Pointer &rv);
    T &operator*(){
        return *addr;
    }
    T *operator->() { return addr; }
    T &operator[](int i){ return addr[i];}
    operator T *() { return addr; }
    Iter<T> begin(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr, addr, addr + _size);
    }
    Iter<T> end(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr + _size, addr, addr + _size);
    }
    static int refContainerSize() { return refContainer.size(); }
    // A utility function that displays refContainer.
    static void showlist();
    // Clear refContainer when program exits.
    static void shutdown();
};

template <class T, int size>
std::list<PtrDetails<T> > Pointer<T, size>::refContainer;
template <class T, int size>
bool Pointer<T, size>::first = true;

// Constructor for both initialized and uninitialized objects. -> see class interface
template<class T,int size>
Pointer<T,size>::Pointer(T *t){
    // Register shutdown() as an exit function.
    if (first)
        atexit(shutdown);
    first = false;
    std::cout << "Pointer is " << *t << " with memory address: " << t << std::endl;

    if (t != NULL) {
        std::cout << "Not null" << std::endl;
        PtrDetails<T> _newptr (t);
        std::cout << "Increment refCount" << std::endl;
        _newptr.refCount_++;
        std::cout << "assigning address " << t ;
        _newptr.memPtr_ = t;
        std::cout << ", successful," << t << std::endl;
        std::cout << "Instantiating the refContainer... Size is: " <<  refContainerSize() << std::endl;
        showlist();
        // add pointer to list
        refContainer.push_front(_newptr);
        return ;
    }

}
// Copy constructor.
template< class T, int size>
Pointer<T,size>::Pointer(const Pointer &ob){
    typename std::list<PtrDetails<T> >::iterator p;
    p = findPtrInfo(ob.addr);
    p->refCount_++;
    
    if ( p->isArray_)
        isArray = true;
}

/// @brief Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer(){
    typename std::list<PtrDetails<T> >::iterator p;
    p = findPtrInfo(addr);
    if ( p->refCount_)
        p->refCount_--;
    // Collect garbage when a pointer goes out of scope.
    collect();
}

/// @brief Collect garbage. Returns true if at least one object was freed.
template <class T, int size>
bool Pointer<T, size>::collect(){
    bool memfreed = false;
    typename std::list<PtrDetails<T> >::iterator p;
    do{
        // Scan refContainer looking for unreferenced pointers.
        for (p = refContainer.begin(); p != refContainer.end(); p++){

            if ( p->refCount_ == 0 )
                break;

            memfreed = true;
            refContainer.remove(*p);
            
            if ( p->memPtr_ ) {
                if ( p->isArray_ ) {
                    delete p->memPtr_;
                } else {
                    delete [] p->memPtr_;
                }
            }

            break;
        }
    } while (p != refContainer.end());
    return memfreed;
}

// Overload assignment of pointer to Pointer.
template <class T, int size>
T *Pointer<T, size>::operator=(T *t){
    typename std::list<PtrDetails<T> >::iterator p;
    p = findPtrInfo(addr);
    p->refCount_--;

    p = findPtrInfo(t);
    if ( p == refContainer.end() ) { // add pointer to list
        PtrDetails<T> _newptr (t);
        _newptr.memPtr_ = t;
        _newptr.refCount_++;
        refContainer.push_front(_newptr);
    } 
    p->refCount_++;
    addr = t;
    return t;
}

// Overload assignment of Pointer to Pointer.
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv){
    typename std::list<PtrDetails<T> >::iterator p;
    p = findPtrInfo(addr);
    p->refCount_--;

    p = findPtrInfo(rv.addr);
    p->refCount_++;
    addr = rv.addr;

    return rv;
}

// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist(){
    typename std::list<PtrDetails<T> >::iterator p;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end())
    {
        std::cout << " Container is empty!\n\n ";
    }
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        std::cout << "[" << (void *)p->memPtr_ << "]"
             << " " << p->refCount_ << " ";
        if (p->memPtr_)
            std::cout << " " << *p->memPtr_;
        else
            std::cout << "---";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T> >::iterator
Pointer<T, size>::findPtrInfo(T *ptr){
    typename std::list<PtrDetails<T> >::iterator p;
    // Find ptr in refContainer.
    for (p = refContainer.begin(); p != refContainer.end(); p++)
        if (p->memPtr_ == ptr)
            return p;
    return p;
}
// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown(){
    if (refContainerSize() == 0)
        return; // list is empty
    typename std::list<PtrDetails<T> >::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        // Set all reference counts to zero
        p->refCount_ = 0;
    }
    collect();
}
