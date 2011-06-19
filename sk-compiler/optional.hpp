#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#include <assert.h>

struct nothing {
};

template <typename T>
struct optional
{
    typedef optional<T> this_type;
    bool valid;
    T v;

    optional()
        :valid(false){}
    optional(T const &v)
        :valid(true), v(v)
    {}
    optional(nothing)
        :valid(false){}

    operator this_type *() const { return valid?(this_type*)this:NULL; }

    bool operator !() { return !valid; }
    const T &operator *() const {
        assert(valid);
        return v;
    }

    const T *operator ->() const {
        assert(valid);
        return &v;
    }
};


#endif
