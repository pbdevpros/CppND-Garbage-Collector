// This class defines an element that is stored
// in the garbage collection information list.
//
/// @file PrtDetails.cpp

template<class T>
class PtrDetails {
  public:
    PtrDetails(T* member, int size = 1 ) ; // https://en.cppreference.com/w/cpp/language/default_arguments - argument can only be a constant or static member variable
    T refCount_;
    T *memPtr_;
    bool isArray_;
    int arraySize_;
};

template<class T>
PtrDetails<T>::PtrDetails(T* member, int size)
{
    /**
     * https://stackoverflow.com/questions/33523585/how-do-sizeofarr-sizeofarr0-work
     */
    int len =  sizeof(member) / sizeof(T) ;
    if ( len > 1 ) {
        isArray_ = true;
    }
}

// not a member function...
template <class T>
bool operator==(const PtrDetails<T> &obj_1,
                const PtrDetails<T> &obj_2)
{
    return obj_1.refCount_ == obj_2.refCount_;
}