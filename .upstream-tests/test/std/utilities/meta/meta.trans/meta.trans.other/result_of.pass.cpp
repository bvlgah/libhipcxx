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

// <cuda/std/functional>
// TODO(HIP): Add support for functional header.
// UNSUPPORTED: hipcc

// result_of<Fn(ArgTypes...)>

#define _LIBCUDACXX_ENABLE_CXX20_REMOVED_TYPE_TRAITS
#define _LIBCUDACXX_DISABLE_DEPRECATION_WARNINGS

#include <hip/std/type_traits>
// #include <hip/std/memory>
#include <hip/std/cassert>
#include <hip/functional>
#include "test_macros.h"

struct S
{
    typedef short (*FreeFunc)(long);
    __host__ __device__
    operator FreeFunc() const;
    __host__ __device__
    double operator()(char, int&);
    __host__ __device__
    double const& operator()(char, int&) const;
    __host__ __device__
    double volatile& operator()(char, int&) volatile;
    __host__ __device__
    double const volatile& operator()(char, int&) const volatile;
};


struct SD : public S { };

struct NotDerived {};

template <class Tp>
struct Voider {
    typedef void type;
};

template <class T, class = void>
struct HasType : hip::std::false_type {};

template <class T>
struct HasType<T, typename Voider<typename T::type>::type> : hip::std::true_type {};

#if TEST_STD_VER > 11
template <typename T, typename U>
struct test_invoke_result;

template <typename Fn, typename ...Args, typename Ret>
struct test_invoke_result<Fn(Args...), Ret>
{
    __host__ __device__
    static void call()
    {
        static_assert(hip::std::is_invocable<Fn, Args...>::value, "");
        static_assert(hip::std::is_invocable_r<Ret, Fn, Args...>::value, "");
        ASSERT_SAME_TYPE(Ret, typename hip::std::invoke_result<Fn, Args...>::type);
    }
};
#endif

template <class T, class U>
__host__ __device__
void test_result_of()
{
    ASSERT_SAME_TYPE(U, typename hip::std::result_of<T>::type);
#if TEST_STD_VER > 11
    test_invoke_result<T, U>::call();
#endif
}

#if TEST_STD_VER > 11
template <typename T>
struct test_invoke_no_result;

template <typename Fn, typename ...Args>
struct test_invoke_no_result<Fn(Args...)>
{
    __host__ __device__
    static void call()
    {
        static_assert(hip::std::is_invocable<Fn, Args...>::value == false, "");
        static_assert((!HasType<hip::std::invoke_result<Fn, Args...> >::value), "");
    }
};
#endif

template <class T>
__host__ __device__
void test_no_result()
{
#if TEST_STD_VER >= 11
    static_assert((!HasType<hip::std::result_of<T> >::value), "");
#endif
#if TEST_STD_VER > 11
    test_invoke_no_result<T>::call();
#endif
}

#if defined(__NVCC__) || defined(__HIPCC__)
template <class Ret, class Fn>
__host__ __device__
void test_lambda(Fn &&)
{
    ASSERT_SAME_TYPE(Ret, typename hip::std::result_of<Fn()>::type);

#if TEST_STD_VER > 11
    ASSERT_SAME_TYPE(Ret, typename hip::std::invoke_result<Fn>::type);
#endif
}
#endif

int main(int, char**)
{
    typedef NotDerived ND;
    { // functor object
    test_result_of<S(int), short> ();
    test_result_of<S&(unsigned char, int&), double> ();
    test_result_of<S const&(unsigned char, int&), double const &> ();
    test_result_of<S volatile&(unsigned char, int&), double volatile&> ();
    test_result_of<S const volatile&(unsigned char, int&), double const volatile&> ();
    }
    { // pointer to function
    typedef bool        (&RF0)();
    typedef bool*       (&RF1)(int);
    typedef bool&       (&RF2)(int, int);
    typedef bool const& (&RF3)(int, int, int);
    typedef bool        (&RF4)(int, ...);
    typedef bool        (*PF0)();
    typedef bool*       (*PF1)(int);
    typedef bool&       (*PF2)(int, int);
    typedef bool const& (*PF3)(int, int, int);
    typedef bool        (*PF4)(int, ...);
    typedef bool        (*&PRF0)();
    typedef bool*       (*&PRF1)(int);
    typedef bool&       (*&PRF2)(int, int);
    typedef bool const& (*&PRF3)(int, int, int);
    typedef bool        (*&PRF4)(int, ...);
    test_result_of<RF0(), bool>();
    test_result_of<RF1(int), bool*>();
    test_result_of<RF2(int, long), bool&>();
    test_result_of<RF3(int, long, int), bool const&>();
    test_result_of<RF4(int, float, void*), bool>();
    test_result_of<PF0(), bool>();
    test_result_of<PF1(int), bool*>();
    test_result_of<PF2(int, long), bool&>();
    test_result_of<PF3(int, long, int), bool const&>();
    test_result_of<PF4(int, float, void*), bool>();
    test_result_of<PRF0(), bool>();
    test_result_of<PRF1(int), bool*>();
    test_result_of<PRF2(int, long), bool&>();
    test_result_of<PRF3(int, long, int), bool const&>();
    test_result_of<PRF4(int, float, void*), bool>();
    }
    { // pointer to member function

    typedef int         (S::*PMS0)();
    typedef int*        (S::*PMS1)(long);
    typedef int&        (S::*PMS2)(long, int);
    typedef const int&  (S::*PMS3)(int, ...);
    test_result_of<PMS0(                             S),   int> ();
    test_result_of<PMS0(                             S&),  int> ();
    test_result_of<PMS0(                             S*),  int> ();
    test_result_of<PMS0(                             S*&), int> ();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_result_of<PMS0(      hip::std::reference_wrapper<S>),  int> ();
    test_result_of<PMS0(const hip::std::reference_wrapper<S>&), int> ();
    test_result_of<PMS0(      hip::std::reference_wrapper<SD>),  int> ();
    test_result_of<PMS0(const hip::std::reference_wrapper<SD>&), int> ();
    test_result_of<PMS0(hip::std::unique_ptr<S>),  int> ();
    test_result_of<PMS0(hip::std::unique_ptr<SD>), int> ();
#endif
    test_no_result<PMS0(const          S&)>();
    test_no_result<PMS0(volatile       S&)>();
    test_no_result<PMS0(const volatile S&)>();
    test_no_result<PMS0(ND &                           )>();
    test_no_result<PMS0(const ND&                      )>();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_no_result<PMS0(hip::std::unique_ptr<S const>       )>();
    test_no_result<PMS0(hip::std::reference_wrapper<S const>)>();
    test_no_result<PMS0(hip::std::reference_wrapper<ND>     )>();
    test_no_result<PMS0(hip::std::unique_ptr<ND>            )>();
#endif

    test_result_of<PMS1(                             S,   int), int*> ();
    test_result_of<PMS1(                             S&,  int), int*> ();
    test_result_of<PMS1(                             S*,  int), int*> ();
    test_result_of<PMS1(                             S*&, int), int*> ();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_result_of<PMS1(hip::std::unique_ptr<S>,               int), int*> ();
    test_result_of<PMS1(hip::std::unique_ptr<SD>,              int), int*> ();
    test_result_of<PMS1(hip::std::reference_wrapper<S>,        int), int*> ();
    test_result_of<PMS1(const hip::std::reference_wrapper<S>&, int), int*> ();
    test_result_of<PMS1(hip::std::reference_wrapper<SD>,        int), int*> ();
    test_result_of<PMS1(const hip::std::reference_wrapper<SD>&, int), int*> ();
#endif
    test_no_result<PMS1(const          S&, int)>();
    test_no_result<PMS1(volatile       S&, int)>();
    test_no_result<PMS1(const volatile S&, int)>();
    test_no_result<PMS1(ND &,                            int)>();
    test_no_result<PMS1(const ND&,                       int)>();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_no_result<PMS1(hip::std::unique_ptr<S const>,        int)>();
    test_no_result<PMS1(hip::std::reference_wrapper<S const>, int)>();
    test_no_result<PMS1(hip::std::reference_wrapper<ND>,      int)>();
    test_no_result<PMS1(hip::std::unique_ptr<ND>,             int)>();
#endif

    test_result_of<PMS2(               S,   int, int), int&> ();
    test_result_of<PMS2(               S&,  int, int), int&> ();
    test_result_of<PMS2(               S*,  int, int), int&> ();
    test_result_of<PMS2(               S*&, int, int), int&> ();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_result_of<PMS2(hip::std::unique_ptr<S>, int, int), int&> ();
    test_result_of<PMS2(hip::std::unique_ptr<SD>, int, int), int&> ();
    test_result_of<PMS2(hip::std::reference_wrapper<S>,         int, int), int&> ();
    test_result_of<PMS2(const hip::std::reference_wrapper<S>&,  int, int), int&> ();
    test_result_of<PMS2(hip::std::reference_wrapper<SD>,        int, int), int&> ();
    test_result_of<PMS2(const hip::std::reference_wrapper<SD>&, int, int), int&> ();
#endif
    test_no_result<PMS2(const          S&, int, int)>();
    test_no_result<PMS2(volatile       S&, int, int)>();
    test_no_result<PMS2(const volatile S&, int, int)>();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_no_result<PMS2(hip::std::unique_ptr<S const>,   int, int)>();
    test_no_result<PMS2(hip::std::reference_wrapper<S const>, int, int)>();
#endif
    test_no_result<PMS2(const ND&,                  int, int)>();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_no_result<PMS2(hip::std::reference_wrapper<ND>, int, int)>();
    test_no_result<PMS2(hip::std::unique_ptr<ND>,        int, int)>();
#endif

    test_result_of<PMS3(S&, int), const int &>();
    test_result_of<PMS3(S&, int, long), const int &>();

    typedef int        (S::*PMS0C)() const;
    typedef int*       (S::*PMS1C)(long) const;
    typedef int&       (S::*PMS2C)(long, int) const;
    typedef const int& (S::*PMS3C)(int, ...) const;
    test_result_of<PMS0C(               S),   int> ();
    test_result_of<PMS0C(               S&),  int> ();
    test_result_of<PMS0C(const          S&),  int> ();
    test_result_of<PMS0C(               S*),  int> ();
    test_result_of<PMS0C(const          S*),  int> ();
    test_result_of<PMS0C(               S*&), int> ();
    test_result_of<PMS0C(const          S*&), int> ();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_result_of<PMS0C(hip::std::unique_ptr<S>), int> ();
    test_result_of<PMS0C(hip::std::unique_ptr<SD>), int> ();
    test_result_of<PMS0C(hip::std::reference_wrapper<S>              ), int> ();
    test_result_of<PMS0C(hip::std::reference_wrapper<const S>        ), int> ();
    test_result_of<PMS0C(const hip::std::reference_wrapper<S> &      ), int> ();
    test_result_of<PMS0C(const hip::std::reference_wrapper<const S> &), int> ();
    test_result_of<PMS0C(hip::std::reference_wrapper<SD>             ), int> ();
    test_result_of<PMS0C(hip::std::reference_wrapper<const SD>       ), int> ();
    test_result_of<PMS0C(const hip::std::reference_wrapper<SD> &     ), int> ();
    test_result_of<PMS0C(const hip::std::reference_wrapper<const SD> &), int> ();
#endif
    test_no_result<PMS0C(volatile       S&)>();
    test_no_result<PMS0C(const volatile S&)>();

    test_result_of<PMS1C(               S,   int), int*> ();
    test_result_of<PMS1C(               S&,  int), int*> ();
    test_result_of<PMS1C(const          S&,  int), int*> ();
    test_result_of<PMS1C(               S*,  int), int*> ();
    test_result_of<PMS1C(const          S*,  int), int*> ();
    test_result_of<PMS1C(               S*&, int), int*> ();
    test_result_of<PMS1C(const          S*&, int), int*> ();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_result_of<PMS1C(hip::std::unique_ptr<S>, int), int*> ();
#endif
    test_no_result<PMS1C(volatile       S&, int)>();
    test_no_result<PMS1C(const volatile S&, int)>();

    test_result_of<PMS2C(               S,   int, int), int&> ();
    test_result_of<PMS2C(               S&,  int, int), int&> ();
    test_result_of<PMS2C(const          S&,  int, int), int&> ();
    test_result_of<PMS2C(               S*,  int, int), int&> ();
    test_result_of<PMS2C(const          S*,  int, int), int&> ();
    test_result_of<PMS2C(               S*&, int, int), int&> ();
    test_result_of<PMS2C(const          S*&, int, int), int&> ();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_result_of<PMS2C(hip::std::unique_ptr<S>, int, int), int&> ();
#endif
    test_no_result<PMS2C(volatile       S&, int, int)>();
    test_no_result<PMS2C(const volatile S&, int, int)>();

    test_result_of<PMS3C(S&, int), const int &>();
    test_result_of<PMS3C(S&, int, long), const int &>();

    typedef int       (S::*PMS0V)() volatile;
    typedef int*       (S::*PMS1V)(long) volatile;
    typedef int&       (S::*PMS2V)(long, int) volatile;
    typedef const int& (S::*PMS3V)(int, ...) volatile;
    test_result_of<PMS0V(               S),   int> ();
    test_result_of<PMS0V(               S&),  int> ();
    test_result_of<PMS0V(volatile       S&),  int> ();
    test_result_of<PMS0V(               S*),  int> ();
    test_result_of<PMS0V(volatile       S*),  int> ();
    test_result_of<PMS0V(               S*&), int> ();
    test_result_of<PMS0V(volatile       S*&), int> ();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_result_of<PMS0V(hip::std::unique_ptr<S>), int> ();
#endif
    test_no_result<PMS0V(const          S&)>();
    test_no_result<PMS0V(const volatile S&)>();

    test_result_of<PMS1V(               S,   int), int*> ();
    test_result_of<PMS1V(               S&,  int), int*> ();
    test_result_of<PMS1V(volatile       S&,  int), int*> ();
    test_result_of<PMS1V(               S*,  int), int*> ();
    test_result_of<PMS1V(volatile       S*,  int), int*> ();
    test_result_of<PMS1V(               S*&, int), int*> ();
    test_result_of<PMS1V(volatile       S*&, int), int*> ();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_result_of<PMS1V(hip::std::unique_ptr<S>, int), int*> ();
#endif
    test_no_result<PMS1V(const          S&, int)>();
    test_no_result<PMS1V(const volatile S&, int)>();

    test_result_of<PMS2V(               S,   int, int), int&> ();
    test_result_of<PMS2V(               S&,  int, int), int&> ();
    test_result_of<PMS2V(volatile       S&,  int, int), int&> ();
    test_result_of<PMS2V(               S*,  int, int), int&> ();
    test_result_of<PMS2V(volatile       S*,  int, int), int&> ();
    test_result_of<PMS2V(               S*&, int, int), int&> ();
    test_result_of<PMS2V(volatile       S*&, int, int), int&> ();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_result_of<PMS2V(hip::std::unique_ptr<S>, int, int), int&> ();
#endif
    test_no_result<PMS2V(const          S&, int, int)>();
    test_no_result<PMS2V(const volatile S&, int, int)>();

    test_result_of<PMS3V(S&, int), const int &>();
    test_result_of<PMS3V(S&, int, long), const int &>();

    typedef int        (S::*PMS0CV)() const volatile;
    typedef int*       (S::*PMS1CV)(long) const volatile;
    typedef int&       (S::*PMS2CV)(long, int) const volatile;
    typedef const int& (S::*PMS3CV)(int, ...) const volatile;
    test_result_of<PMS0CV(               S),   int> ();
    test_result_of<PMS0CV(               S&),  int> ();
    test_result_of<PMS0CV(const          S&),  int> ();
    test_result_of<PMS0CV(volatile       S&),  int> ();
    test_result_of<PMS0CV(const volatile S&),  int> ();
    test_result_of<PMS0CV(               S*),  int> ();
    test_result_of<PMS0CV(const          S*),  int> ();
    test_result_of<PMS0CV(volatile       S*),  int> ();
    test_result_of<PMS0CV(const volatile S*),  int> ();
    test_result_of<PMS0CV(               S*&), int> ();
    test_result_of<PMS0CV(const          S*&), int> ();
    test_result_of<PMS0CV(volatile       S*&), int> ();
    test_result_of<PMS0CV(const volatile S*&), int> ();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_result_of<PMS0CV(hip::std::unique_ptr<S>), int> ();
#endif

    test_result_of<PMS1CV(               S,   int), int*> ();
    test_result_of<PMS1CV(               S&,  int), int*> ();
    test_result_of<PMS1CV(const          S&,  int), int*> ();
    test_result_of<PMS1CV(volatile       S&,  int), int*> ();
    test_result_of<PMS1CV(const volatile S&,  int), int*> ();
    test_result_of<PMS1CV(               S*,  int), int*> ();
    test_result_of<PMS1CV(const          S*,  int), int*> ();
    test_result_of<PMS1CV(volatile       S*,  int), int*> ();
    test_result_of<PMS1CV(const volatile S*,  int), int*> ();
    test_result_of<PMS1CV(               S*&, int), int*> ();
    test_result_of<PMS1CV(const          S*&, int), int*> ();
    test_result_of<PMS1CV(volatile       S*&, int), int*> ();
    test_result_of<PMS1CV(const volatile S*&, int), int*> ();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_result_of<PMS1CV(hip::std::unique_ptr<S>, int), int*> ();
#endif

    test_result_of<PMS2CV(               S,   int, int), int&> ();
    test_result_of<PMS2CV(               S&,  int, int), int&> ();
    test_result_of<PMS2CV(const          S&,  int, int), int&> ();
    test_result_of<PMS2CV(volatile       S&,  int, int), int&> ();
    test_result_of<PMS2CV(const volatile S&,  int, int), int&> ();
    test_result_of<PMS2CV(               S*,  int, int), int&> ();
    test_result_of<PMS2CV(const          S*,  int, int), int&> ();
    test_result_of<PMS2CV(volatile       S*,  int, int), int&> ();
    test_result_of<PMS2CV(const volatile S*,  int, int), int&> ();
    test_result_of<PMS2CV(               S*&, int, int), int&> ();
    test_result_of<PMS2CV(const          S*&, int, int), int&> ();
    test_result_of<PMS2CV(volatile       S*&, int, int), int&> ();
    test_result_of<PMS2CV(const volatile S*&, int, int), int&> ();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_result_of<PMS2CV(hip::std::unique_ptr<S>, int, int), int&> ();
#endif

    test_result_of<PMS3CV(S&, int), const int &>();
    test_result_of<PMS3CV(S&, int, long), const int &>();
    }
    { // pointer to member data
    typedef char S::*PMD;
    test_result_of<PMD(S&), char &>();
    test_result_of<PMD(S*), char &>();
    test_result_of<PMD(S* const), char &>();
    test_result_of<PMD(const S&), const char&> ();
    test_result_of<PMD(const S*), const char&> ();
    test_result_of<PMD(volatile S&), volatile char&> ();
    test_result_of<PMD(volatile S*), volatile char&> ();
    test_result_of<PMD(const volatile S&), const volatile char&> ();
    test_result_of<PMD(const volatile S*), const volatile char&> ();
    test_result_of<PMD(SD&), char &>();
    test_result_of<PMD(SD const&), const char&>();
    test_result_of<PMD(SD*), char&>();
    test_result_of<PMD(const SD*), const char&>();
#if !(defined(__NVCC__) || defined(__CUDACC_RTC__) || defined(__HIPCC__) || defined(__HIPCC_RTC__))
    test_result_of<PMD(hip::std::unique_ptr<S>), char &>();
    test_result_of<PMD(hip::std::unique_ptr<S const>), const char&>();
#if TEST_STD_VER >= 11
    test_result_of<PMD(hip::std::reference_wrapper<S>), char&>();
    test_result_of<PMD(hip::std::reference_wrapper<S const>), const char&>();
#endif
#endif
    test_no_result<PMD(ND&)>();
    }
#if defined(__NVCC__) || defined(__HIPCC__)
    { // extended lambda
#if defined(__CUDA_ARCH__) || defined(__HIPC_DEVICE_COMPILE__)
    test_lambda<int>([] __host__ __device__ () -> int { return 42; });
    test_lambda<double>([] __host__ __device__ () -> double { return 42.0; });
    test_lambda<SD>([] __host__ __device__ () -> SD { return {}; });
#endif
    test_lambda<double>(hip::proclaim_return_type<double>(
        [] __device__ () -> double { return 42.0; }));
    }
#endif

  return 0;
}