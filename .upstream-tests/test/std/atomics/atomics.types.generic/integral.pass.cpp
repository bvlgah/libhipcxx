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
//
// UNSUPPORTED: libcpp-has-no-threads, pre-sm-60
// UNSUPPORTED: windows && pre-sm-70

// <cuda/std/atomic>

// template <>
// struct atomic<integral>
// {
//     bool is_lock_free() const volatile;
//     bool is_lock_free() const;
//     void store(integral desr, memory_order m = memory_order_seq_cst) volatile;
//     void store(integral desr, memory_order m = memory_order_seq_cst);
//     integral load(memory_order m = memory_order_seq_cst) const volatile;
//     integral load(memory_order m = memory_order_seq_cst) const;
//     operator integral() const volatile;
//     operator integral() const;
//     integral exchange(integral desr,
//                       memory_order m = memory_order_seq_cst) volatile;
//     integral exchange(integral desr, memory_order m = memory_order_seq_cst);
//     bool compare_exchange_weak(integral& expc, integral desr,
//                                memory_order s, memory_order f) volatile;
//     bool compare_exchange_weak(integral& expc, integral desr,
//                                memory_order s, memory_order f);
//     bool compare_exchange_strong(integral& expc, integral desr,
//                                  memory_order s, memory_order f) volatile;
//     bool compare_exchange_strong(integral& expc, integral desr,
//                                  memory_order s, memory_order f);
//     bool compare_exchange_weak(integral& expc, integral desr,
//                                memory_order m = memory_order_seq_cst) volatile;
//     bool compare_exchange_weak(integral& expc, integral desr,
//                                memory_order m = memory_order_seq_cst);
//     bool compare_exchange_strong(integral& expc, integral desr,
//                                 memory_order m = memory_order_seq_cst) volatile;
//     bool compare_exchange_strong(integral& expc, integral desr,
//                                  memory_order m = memory_order_seq_cst);
//
//     integral
//         fetch_add(integral op, memory_order m = memory_order_seq_cst) volatile;
//     integral fetch_add(integral op, memory_order m = memory_order_seq_cst);
//     integral
//         fetch_sub(integral op, memory_order m = memory_order_seq_cst) volatile;
//     integral fetch_sub(integral op, memory_order m = memory_order_seq_cst);
//     integral
//         fetch_and(integral op, memory_order m = memory_order_seq_cst) volatile;
//     integral fetch_and(integral op, memory_order m = memory_order_seq_cst);
//     integral
//         fetch_or(integral op, memory_order m = memory_order_seq_cst) volatile;
//     integral fetch_or(integral op, memory_order m = memory_order_seq_cst);
//     integral
//         fetch_xor(integral op, memory_order m = memory_order_seq_cst) volatile;
//     integral fetch_xor(integral op, memory_order m = memory_order_seq_cst);
//
//     atomic() = default;
//     constexpr atomic(integral desr);
//     atomic(const atomic&) = delete;
//     atomic& operator=(const atomic&) = delete;
//     atomic& operator=(const atomic&) volatile = delete;
//     integral operator=(integral desr) volatile;
//     integral operator=(integral desr);
//
//     integral operator++(int) volatile;
//     integral operator++(int);
//     integral operator--(int) volatile;
//     integral operator--(int);
//     integral operator++() volatile;
//     integral operator++();
//     integral operator--() volatile;
//     integral operator--();
//     integral operator+=(integral op) volatile;
//     integral operator+=(integral op);
//     integral operator-=(integral op) volatile;
//     integral operator-=(integral op);
//     integral operator&=(integral op) volatile;
//     integral operator&=(integral op);
//     integral operator|=(integral op) volatile;
//     integral operator|=(integral op);
//     integral operator^=(integral op) volatile;
//     integral operator^=(integral op);
// };

#include <hip/std/atomic>
#include <hip/std/cassert>

#include <cmpxchg_loop.h>

#include "test_macros.h"
#if !defined(TEST_COMPILER_C1XX)
  #include "placement_new.h"
#endif
#include "cuda_space_selector.h"

template <class A, class T, template<typename, typename> class Selector>
__host__ __device__ __noinline__
void
do_test()
{
    Selector<A, constructor_initializer> sel;
    A & obj = *sel.construct(T(0));
    bool b0 = obj.is_lock_free();
    ((void)b0); // mark as unused
    obj.store(T(0));
    assert(obj == T(0));
    obj.store(T(1), hip::std::memory_order_release);
    assert(obj == T(1));
    assert(obj.load() == T(1));
    assert(obj.load(hip::std::memory_order_acquire) == T(1));
    assert(obj.exchange(T(2)) == T(1));
    assert(obj == T(2));
    assert(obj.exchange(T(3), hip::std::memory_order_relaxed) == T(2));
    assert(obj == T(3));
    T x = obj;
    assert(cmpxchg_weak_loop(obj, x, T(2)) == true);
    assert(obj == T(2));
    assert(x == T(3));
    assert(obj.compare_exchange_weak(x, T(1)) == false);
    assert(obj == T(2));
    assert(x == T(2));
    x = T(2);
    assert(obj.compare_exchange_strong(x, T(1)) == true);
    assert(obj == T(1));
    assert(x == T(2));
    assert(obj.compare_exchange_strong(x, T(0)) == false);
    assert(obj == T(1));
    assert(x == T(1));
    assert((obj = T(0)) == T(0));
    assert(obj == T(0));
    assert(obj++ == T(0));
    assert(obj == T(1));
    assert(++obj == T(2));
    assert(obj == T(2));
    assert(--obj == T(1));
    assert(obj == T(1));
    assert(obj-- == T(1));
    assert(obj == T(0));
    obj = T(2);
    assert((obj += T(3)) == T(5));
    assert(obj == T(5));
    assert((obj -= T(3)) == T(2));
    assert(obj == T(2));
    assert((obj |= T(5)) == T(7));
    assert(obj == T(7));
    assert((obj &= T(0xF)) == T(7));
    assert(obj == T(7));
    assert((obj ^= T(0xF)) == T(8));
    assert(obj == T(8));

#if __cplusplus > 201703L
    {
#if !defined(__CUDA_ARCH__) || __CUDA_ARCH__ >= 700
        TEST_ALIGNAS_TYPE(A) char storage[sizeof(A)] = {23};
        A& zero = *new (storage) A();
        assert(zero == 0);
        zero.~A();
#endif
    }
#endif
}

template <class A, class T, template<typename, typename> class Selector>
__host__ __device__ __noinline__
void test()
{
    do_test<A, T, Selector>();
    do_test<volatile A, T, Selector>();
}

template<template<typename, hip::thread_scope> typename Atomic, hip::thread_scope Scope, template<typename, typename> class Selector>
__host__ __device__
void test_for_all_types()
{
    test<Atomic<char, Scope>, char, Selector>();
    test<Atomic<signed char, Scope>, signed char, Selector>();
    test<Atomic<unsigned char, Scope>, unsigned char, Selector>();
    test<Atomic<short, Scope>, short, Selector>();
    test<Atomic<unsigned short, Scope>, unsigned short, Selector>();
    test<Atomic<int, Scope>, int, Selector>();
    test<Atomic<unsigned int, Scope>, unsigned int, Selector>();
    test<Atomic<long, Scope>, long, Selector>();
    test<Atomic<unsigned long, Scope>, unsigned long, Selector>();
    test<Atomic<long long, Scope>, long long, Selector>();
    test<Atomic<unsigned long long, Scope>, unsigned long long, Selector>();
#ifndef _LIBCUDACXX_HAS_NO_UNICODE_CHARS
    test<Atomic<char16_t, Scope>, char16_t, Selector>();
    test<Atomic<char32_t, Scope>, char32_t, Selector>();
#endif  // _LIBCUDACXX_HAS_NO_UNICODE_CHARS
    test<Atomic<wchar_t, Scope>, wchar_t, Selector>();

    test<Atomic<int8_t, Scope>,    int8_t, Selector>();
    test<Atomic<uint8_t, Scope>,  uint8_t, Selector>();
    test<Atomic<int16_t, Scope>,   int16_t, Selector>();
    test<Atomic<uint16_t, Scope>, uint16_t, Selector>();
    test<Atomic<int32_t, Scope>,   int32_t, Selector>();
    test<Atomic<uint32_t, Scope>, uint32_t, Selector>();
    test<Atomic<int64_t, Scope>,   int64_t, Selector>();
    test<Atomic<uint64_t, Scope>, uint64_t, Selector>();
}

template<typename T, hip::thread_scope Scope>
using cuda_std_atomic = hip::std::atomic<T>;

template<typename T, hip::thread_scope Scope>
using cuda_atomic = hip::atomic<T, Scope>;

int main(int, char**)
{
    // this test would instantiate more cases than just the ones below
    // but ptxas already consumes 5 GB of RAM while translating these
    // so in the interest of not eating all memory, it's limited to the current set
    //
    // the per-function tests *should* cover the other codegen aspects of the
    // code, and the cross between scopes and memory locations below should provide
    // a *reasonable* subset of all the possible combinations to provide enough
    // confidence that this all actually works

#if !defined(__CUDA_ARCH__) || __CUDA_ARCH__ >= 700
    test_for_all_types<cuda_std_atomic, hip::thread_scope_system, local_memory_selector>();
    test_for_all_types<cuda_atomic, hip::thread_scope_system, local_memory_selector>();
#endif
#if defined(__CUDA_ARCH__) || defined(__HIP_DEVICE_COMPILE__)
    test_for_all_types<cuda_std_atomic, hip::thread_scope_system, shared_memory_selector>();
    test_for_all_types<cuda_atomic, hip::thread_scope_block, shared_memory_selector>();

    test_for_all_types<cuda_std_atomic, hip::thread_scope_system, global_memory_selector>();
    test_for_all_types<cuda_atomic, hip::thread_scope_device, global_memory_selector>();
#endif

  return 0;
}