#ifndef _MEM_STACK_ALLOC_H
#define _MEM_STACK_ALLOC_H

#define HAS_TYPE_TRAITS 0

#include <cstddef>
#include <memory>

#if HAS_TYPE_TRAITS
#include <type_traits>
#else
#include <tr1/type_traits>
#endif

template <class T, std::size_t N, typename Alloc = std::allocator<T> >
class StackAlloc
{
public:
    typedef std::size_t       size_type;
    typedef T                 value_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef value_type&       reference;
    typedef const value_type& const_reference;
    typedef std::ptrdiff_t    difference_type;
    static constexpr size_t   stack_size = N;
private:
#if HAS_TYPE_TRAITS
    typename std::aligned_storage<sizeof(T) * N, 16>::type buf_;
#else
    typename std::tr1::aligned_storage<sizeof(T) * N, 16>::type buf_;
#endif
    pointer ptr_;
    Alloc seconderyAllocator;

public:
    StackAlloc() throw() : ptr_((pointer)&buf_), seconderyAllocator() {}

    StackAlloc(const StackAlloc&) throw() 
        : ptr_((pointer)&buf_), 
        seconderyAllocator() {}

    template <class U> StackAlloc(const StackAlloc<U, N>&) throw()
        : ptr_((pointer)&buf_),
        seconderyAllocator() {}

    template <class U> struct rebind {typedef StackAlloc<U, N> other;};

private:
    StackAlloc& operator=(const StackAlloc&);

public:
    pointer allocate(size_type n, typename StackAlloc<void, N>::const_pointer p = 0) {
        if (static_cast<size_type>((pointer)&buf_ + N - ptr_) >= n) {
            pointer r = ptr_;
            ptr_ += n;
            return r;
        } 
        else {
            return seconderyAllocator.allocate(n, p);
        }
    }

    void deallocate(pointer p, size_type n) {
        if ((pointer)&buf_ <= p && p < (pointer)&buf_ + N) {
            if (p + n == ptr_)
                ptr_ = p;
        }
        else {
            seconderyAllocator.deallocate(p, n);
        }
    }

    size_type max_size() const throw() { return seconderyAllocator.max_size();}

    void destroy(T* p) {p->~T();}

    void construct(pointer p) { ::new((void*)p) T(); }

    template <class A0>
    void construct(pointer p, const A0& a0) { ::new((void*)p) T(a0); }

    pointer address (reference value) const {
        return seconderyAllocator.address(value);
    }

    const_pointer address (const_reference value) const { 
        return seconderyAllocator.address(value);
    }

    bool operator==(StackAlloc& a) const {
        return &buf_ == &a.buf_ && seconderyAllocator == a.seconderyAllocator;
    }
    bool operator!=(StackAlloc& a) const {
        return &buf_ == &a.buf_ && seconderyAllocator == a.seconderyAllocator;
    }

};

template <std::size_t N, typename Alloc>
class StackAlloc<void, N, Alloc>
{
public:
    typedef const void*       const_pointer;
    typedef void              value_type;
};

#endif  // _MEM_STACK_ALLOC_H 
