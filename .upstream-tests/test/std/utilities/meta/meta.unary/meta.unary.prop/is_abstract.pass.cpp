//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Modifications Copyright (c) 2024 Advanced Micro Devices, Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// type_traits

// is_abstract

#include <hip/std/type_traits>
#include "test_macros.h"

template <class T>
__host__ __device__
void test_is_abstract()
{
    static_assert( hip::std::is_abstract<T>::value, "");
    static_assert( hip::std::is_abstract<const T>::value, "");
    static_assert( hip::std::is_abstract<volatile T>::value, "");
    static_assert( hip::std::is_abstract<const volatile T>::value, "");
#if TEST_STD_VER > 11
    static_assert( hip::std::is_abstract_v<T>, "");
    static_assert( hip::std::is_abstract_v<const T>, "");
    static_assert( hip::std::is_abstract_v<volatile T>, "");
    static_assert( hip::std::is_abstract_v<const volatile T>, "");
#endif
}

template <class T>
__host__ __device__
void test_is_not_abstract()
{
    static_assert(!hip::std::is_abstract<T>::value, "");
    static_assert(!hip::std::is_abstract<const T>::value, "");
    static_assert(!hip::std::is_abstract<volatile T>::value, "");
    static_assert(!hip::std::is_abstract<const volatile T>::value, "");
#if TEST_STD_VER > 11
    static_assert(!hip::std::is_abstract_v<T>, "");
    static_assert(!hip::std::is_abstract_v<const T>, "");
    static_assert(!hip::std::is_abstract_v<volatile T>, "");
    static_assert(!hip::std::is_abstract_v<const volatile T>, "");
#endif
}

class Empty
{
};

class NotEmpty
{
    __host__ __device__
    virtual ~NotEmpty();
};

union Union {};

struct bit_zero
{
    int :  0;
};

class Abstract
{
    __host__ __device__
    virtual ~Abstract() = 0;
};

template <class>
struct AbstractTemplate {
    __host__ __device__
  virtual void test() = 0;
};

template <>
struct AbstractTemplate<double> {};

int main(int, char**)
{
    test_is_not_abstract<void>();
    test_is_not_abstract<int&>();
    test_is_not_abstract<int>();
    test_is_not_abstract<double>();
    test_is_not_abstract<int*>();
    test_is_not_abstract<const int*>();
    test_is_not_abstract<char[3]>();
    test_is_not_abstract<char[]>();
    test_is_not_abstract<Union>();
    test_is_not_abstract<Empty>();
    test_is_not_abstract<bit_zero>();
    test_is_not_abstract<NotEmpty>();

    test_is_abstract<Abstract>();
    test_is_abstract<AbstractTemplate<int> >();
    test_is_not_abstract<AbstractTemplate<double> >();

  return 0;
}
