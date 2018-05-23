#pragma once

#include <typeinfo>
#include<type_traits>

namespace net
{

    class Any
    {
    public:
        Any() noexcept: _value_ptr(nullptr)
        {}

        ~Any()noexcept
        {
            delete _value_ptr;
        }

        template<typename ValueType
                ,typename=std::enable_if<!std::is_same<typename std::decay<ValueType>::type,Any>::value
                        &&std::is_copy_constructible<typename std::decay<ValueType>::type>::value> >
        explicit Any(ValueType &&value)
                : _value_ptr(new AnyImpl<typename std::decay<ValueType>::type>(std::forward<ValueType>(value)))
        {}

        Any(const Any &other)
                : _value_ptr(other._value_ptr ? other._value_ptr->clone() : nullptr)
        {}

        Any(Any &&other)noexcept
                : _value_ptr(other._value_ptr)
        {
            other._value_ptr = nullptr;
        }

    public:
        Any &swap(Any &rhs)noexcept
        {
            std::swap(_value_ptr, rhs._value_ptr);
            return *this;
        }

        template<typename ValueType>
        Any &operator=(ValueType &&rhs)
        {
            *this = Any{std::forward<ValueType>(rhs)};
            return *this;
        }

        Any &operator=(const Any &rhs)
        {
            *this = Any{rhs};
            return *this;
        }

        Any &operator=(Any &&rhs) noexcept
        {
            reset();
            _value_ptr = rhs._value_ptr;
            rhs._value_ptr = nullptr;
            return *this;
        }

        bool has_value() const noexcept
        {
            return _value_ptr != nullptr;
        }

        const std::type_info &type() const noexcept
        {
            return has_value() ? _value_ptr->type() : typeid(void);
        }

        void reset() noexcept
        {
            delete _value_ptr;
            _value_ptr = nullptr;
        }

        template<typename ValueType>
        const ValueType *_Cast() const noexcept
        {
            if (has_value() && type() == typeid(ValueType)) {
                return &(static_cast<Any::AnyImpl<ValueType> * >(_value_ptr)->_value);
            }
            return nullptr;
        }

        template<typename ValueType>
        ValueType *_Cast()noexcept
        {
            return const_cast<ValueType *>(const_cast<const Any *>(this)->_Cast<ValueType>());
        }

    protected:
        struct AnyImplBase
        {
            virtual ~AnyImplBase()noexcept
            {}

            virtual const std::type_info &type() const noexcept= 0;

            virtual AnyImplBase *clone() const =0;
        };

        template<typename ValueType>
        struct AnyImpl : public AnyImplBase
        {
            AnyImpl(const ValueType &value)
                    : _value(value)
            {}

            AnyImpl(ValueType &&value)
                    : _value(std::move(value))
            {}

            virtual const std::type_info &type() const noexcept
            {
                return typeid(ValueType);
            }

            virtual AnyImplBase *clone() const
            {
                return new AnyImpl(_value);
            }

            ValueType _value;
        };

    private:
        AnyImplBase *_value_ptr;
    };

    template<typename ValueType>
    ValueType *any_cast(Any *a) noexcept
    {
        return a->_Cast<ValueType>();
    }

    template<typename ValueType>
    const ValueType *any_cast(const Any *a)noexcept
    {
        return a->_Cast<ValueType>();
    }

    template<typename ValueType>
    ValueType any_cast(Any &a)
    {
        auto const result = any_cast<typename std::decay<ValueType>::type>(&a);

        if (!result) {
            throw std::bad_cast();
        }

        return static_cast<ValueType>(*result);
    }
}