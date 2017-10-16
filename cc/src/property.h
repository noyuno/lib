/**
 *    The MIT License (MIT)
 *
 *    Copyright (c) 2013 HogeTatu
 *
 *    Permission is hereby granted, free of charge, to any person obtaining a copy of
 *    this software and associated documentation files (the "Software"), to deal in
 *    the Software without restriction, including without limitation the rights to
 *    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 *    the Software, and to permit persons to whom the Software is furnished to do so,
 *    subject to the following conditions:
 *
 *    The above copyright notice and this permission notice shall be included in all
 *    copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *    FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *    COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* 2017-10-16 changed variables name */

#pragma once

namespace lib {

    //! simple getter
    template <typename T>
    struct simple_getter
    {
        static const T& get(const T& value) { return value; }
    };

    //! simple setter
    template <typename T>
    struct simple_setter
    {
        static void set(T& value, const T& var) { value = T(var); }
    };

    //! property
    template <typename T>
    class property
    {
    public:
        property(T& value) : _value(value) {}
        property(const property& ref) : _value(ref._value) {}
        virtual ~property() {}

        property<T>& operator = (const property<T>& ref)
        {
            this->_value = T(ref._value);
            return *this;
        }

    protected:
        T&        _value;
    };

    //! writable property
    template <typename T, class getter = simple_getter<T>, class setter = simple_setter<T>>
    class wproperty : public property<T>, private getter, private setter
    {
    public:
        wproperty(T& value) : property<T>(value) {}
        wproperty(const wproperty& ref) : property<T>(ref) {}
        virtual ~wproperty() {}

    public:
        operator const T& () const { return this->get(this->_value); }
        const T& operator -> () const { return this->get(this->_value); }

        wproperty<T, getter, setter>& operator = (const T& var) { this->set(this->_value, var); return *this; }
    };

    //! readonly property
    template <typename T, class getter = simple_getter<T>>
    class rproperty : public property<T>, private getter
    {
    public:
        rproperty(T& value) : property<T>(value) {}
        rproperty(const rproperty& ref) : property<T>(ref) {}
        virtual ~rproperty() {}

    public:
        operator const T& () const { return this->get(this->_value); }
        const T& operator -> () const { return this->get(this->_value); }
    };
}

