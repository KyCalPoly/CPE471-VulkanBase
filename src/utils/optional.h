#pragma once
#ifndef CSC471_OPTIONAL_SUBSTITUTE_H_
#define CSC471_OPTIONAL_SUBSTITUTE_H_

#if __cplusplus >= 201703L
#include <optional>
namespace opt{
    using std::optional;
    using std::nullopt_t;
    using std::nullopt;
    using std::bad_optional_access;
}
#else

#include <exception>

namespace opt{


struct nullopt_t
{
    explicit constexpr nullopt_t(int) {}
};

constexpr nullopt_t nullopt{42};

class bad_optional_access : std::exception
{
 public:
    virtual const char* what() const noexcept{
        return("error: optional has no contained value");
    }
};

/* A minimal replica of optional found in C++17 */
template<typename T>
class optional
{
 public:
    using value_type = T;

    optional() noexcept {}
    optional(const value_type& value) {
        _mHasValue = true;
        _assign(value);
    }
    optional(nullopt_t) noexcept {}

    optional<T>& operator=(nullopt_t) noexcept {
        reset();
        return(*this);
    }
    optional<T>& operator=(const value_type& value) {
        _mHasValue = true;
        _assign(value);
        return(*this);
    }

    const value_type* operator->() const {
        return(reinterpret_cast<const T*>(_mValue));
    }
    value_type* operator->() {
        return(reinterpret_cast<T*>(_mValue));
    }
    const value_type& operator*() const {
        return(*reinterpret_cast<const T*>(_mValue));
    }
    value_type& operator*() {
        return(*reinterpret_cast<T*>(_mValue));
    }


    explicit operator bool() const noexcept {return(_mHasValue);}
    bool has_value() const noexcept {return(_mHasValue);}

    T& value() {
        if(!_mHasValue)
            throw bad_optional_access();
        T* interpreted_value = reinterpret_cast<T*>(_mValue);
        return(*interpreted_value);
    }
    const T& value() const {
        if(!_mHasValue)
            throw bad_optional_access();
        const T* interpreted_value = reinterpret_cast<const T*>(_mValue);
        return(*interpreted_value);
    }

    void reset() noexcept {
        if(_mHasValue){
            value().~T();
        }
        _mHasValue = false;
    }

    friend bool operator==(const optional<T>& lhs, const optional<T>& rhs){
        bool sameHaveValue = lhs.has_value() == rhs.has_value();
        return(sameHaveValue && (!lhs.has_value() || *lhs == *rhs) );
    }
    friend bool operator==(const optional<T>& lhs, const value_type& rhs) {return(lhs.has_value() && *lhs == rhs);}
    friend bool operator==(const value_type& lhs, const optional<T>& rhs) {return(rhs.has_value() && lhs == *rhs);}
    friend bool operator==(const optional<T>& lhs, nullopt_t) {return(!lhs.has_value());}
    friend bool operator==(nullopt_t, const optional<T>& rhs) {return(!rhs.has_value());}

    friend bool operator!=(const optional<T>& lhs, const optional<T>& rhs) {return(!operator==(lhs, rhs));}
    friend bool operator!=(const optional<T>& lhs, const value_type& rhs) {return(!operator==(lhs, rhs));}
    friend bool operator!=(const value_type& lhs, const optional<T>& rhs) {return(!operator==(lhs, rhs));}
    friend bool operator!=(const optional<T>& lhs, nullopt_t) {return(!operator==(lhs, nullopt));}
    friend bool operator!=(nullopt_t, const optional<T>& rhs) {return(!operator==(nullopt, rhs));}
    
 protected:
    void _assign(const value_type& value) {
        *reinterpret_cast<T*>(_mValue) = value; 
    }
    
 private:
    bool _mHasValue = false; 
    uint8_t _mValue[sizeof(value_type)];
};

} // end namespace opt

#endif

#endif 