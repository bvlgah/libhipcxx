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

// is_reference

#include <hip/std/type_traits>
#include <hip/std/cstddef>         // for hip::std::nullptr_t
#include "test_macros.h"

template <class T>
__host__ __device__
void test_is_reference()
{
    static_assert( hip::std::is_reference<T>::value, "");
    static_assert( hip::std::is_reference<const T>::value, "");
    static_assert( hip::std::is_reference<volatile T>::value, "");
    static_assert( hip::std::is_reference<const volatile T>::value, "");
#if TEST_STD_VER > 11
    static_assert( hip::std::is_reference_v<T>, "");
    static_assert( hip::std::is_reference_v<const T>, "");
    static_assert( hip::std::is_reference_v<volatile T>, "");
    static_assert( hip::std::is_reference_v<const volatile T>, "");
#endif
}

template <class T>
__host__ __device__
void test_is_not_reference()
{
    static_assert(!hip::std::is_reference<T>::value, "");
    static_assert(!hip::std::is_reference<const T>::value, "");
    static_assert(!hip::std::is_reference<volatile T>::value, "");
    static_assert(!hip::std::is_reference<const volatile T>::value, "");
#if TEST_STD_VER > 11
    static_assert(!hip::std::is_reference_v<T>, "");
    static_assert(!hip::std::is_reference_v<const T>, "");
    static_assert(!hip::std::is_reference_v<volatile T>, "");
    static_assert(!hip::std::is_reference_v<const volatile T>, "");
#endif
}

class incomplete_type;

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

enum Enum {zero, one};

typedef void (*FunctionPtr)();


int main(int, char**)
{
    test_is_reference<int&>();
    test_is_reference<int&&>();
    test_is_not_reference<hip::std::nullptr_t>();
    test_is_not_reference<void>();
    test_is_not_reference<int>();
    test_is_not_reference<double>();
    test_is_not_reference<char[3]>();
    test_is_not_reference<char[]>();
    test_is_not_reference<void *>();
    test_is_not_reference<FunctionPtr>();
    test_is_not_reference<Union>();
    test_is_not_reference<incomplete_type>();
    test_is_not_reference<Empty>();
    test_is_not_reference<bit_zero>();
    test_is_not_reference<int*>();
    test_is_not_reference<const int*>();
    test_is_not_reference<Enum>();
    test_is_not_reference<NotEmpty>();
    test_is_not_reference<Abstract>();
    test_is_not_reference<int(int)>();
    test_is_not_reference<int Empty::*>();
    test_is_not_reference<void (Empty::*)(int)>();


  return 0;
}