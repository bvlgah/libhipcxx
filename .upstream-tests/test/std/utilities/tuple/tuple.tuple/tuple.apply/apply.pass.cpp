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

// UNSUPPORTED: c++98, c++03, c++11, c++14
// UNSUPPORTED: nvrtc
// Internal compiler error in 14.24
// XFAIL: msvc-19.20, msvc-19.21, msvc-19.22, msvc-19.23, msvc-19.24, msvc-19.25
// <cuda/std/tuple>

// template <class F, class T> constexpr decltype(auto) apply(F &&, T &&)

// Test with different ref/ptr/cv qualified argument types.

#include <hip/std/tuple>

// Array tests are disabled
// #include <hip/std/array>

#include <hip/std/utility>
#include <hip/std/cassert>

#include "test_macros.h"

#include "type_id.h"

// hip::std::array is explicitly allowed to be initialized with A a = { init-list };.
// Disable the missing braces warning for this reason.
#include "disable_missing_braces_warning.h"

template <typename T>
__host__ __device__
constexpr bool unused(T &&) {return true;}

__host__ __device__
constexpr int constexpr_sum_fn() { return 0; }

template <class ...Ints>
__host__ __device__
constexpr int constexpr_sum_fn(int x1, Ints... rest) { return x1 + constexpr_sum_fn(rest...); }

struct ConstexprSumT {
    constexpr ConstexprSumT() = default;

    template <class ...Ints>
    __host__ __device__
    constexpr int operator()(Ints... values) const {
        return constexpr_sum_fn(values...);
    }
};


__host__ __device__
void test_constexpr_evaluation()
{
    constexpr ConstexprSumT sum_obj{};
    {
        using Tup = hip::std::tuple<>;
        using Fn = int(&)();
        constexpr Tup t;
        static_assert(hip::std::apply(static_cast<Fn>(constexpr_sum_fn), t) == 0, "");
        static_assert(hip::std::apply(sum_obj, t) == 0, "");
    }
    {
        using Tup = hip::std::tuple<int>;
        using Fn = int(&)(int);
        constexpr Tup t(42);
        static_assert(hip::std::apply(static_cast<Fn>(constexpr_sum_fn), t) == 42, "");
        static_assert(hip::std::apply(sum_obj, t) == 42, "");
    }
    {
        using Tup = hip::std::tuple<int, long>;
        using Fn = int(&)(int, int);
        constexpr Tup t(42, 101);
        static_assert(hip::std::apply(static_cast<Fn>(constexpr_sum_fn), t) == 143, "");
        static_assert(hip::std::apply(sum_obj, t) == 143, "");
    }
    {
        using Tup = hip::std::pair<int, long>;
        using Fn = int(&)(int, int);
        constexpr Tup t(42, 101);
        static_assert(hip::std::apply(static_cast<Fn>(constexpr_sum_fn), t) == 143, "");
        static_assert(hip::std::apply(sum_obj, t) == 143, "");
    }
    {
        using Tup = hip::std::tuple<int, long, int>;
        using Fn = int(&)(int, int, int);
        constexpr Tup t(42, 101, -1);
        static_assert(hip::std::apply(static_cast<Fn>(constexpr_sum_fn), t) == 142, "");
        static_assert(hip::std::apply(sum_obj, t) == 142, "");
    }
    /* TODO: enable hip::std::array
    {
        using Tup = hip::std::array<int, 3>;
        using Fn = int(&)(int, int, int);
        constexpr Tup t = {42, 101, -1};
        static_assert(hip::std::apply(static_cast<Fn>(constexpr_sum_fn), t) == 142, "");
        static_assert(hip::std::apply(sum_obj, t) == 142, "");
    }
    */
}

enum CallQuals {
    CQ_None,
    CQ_LValue,
    CQ_ConstLValue,
    CQ_RValue,
    CQ_ConstRValue
};

template <class Tuple>
struct CallInfo {
    CallQuals quals;
    TypeID const* arg_types;
    Tuple args;

    template <class ...Args>
    __host__ __device__
    CallInfo(CallQuals q, Args&&... xargs)
        : quals(q), arg_types(&makeArgumentID<Args&&...>()), args(hip::std::forward<Args>(xargs)...)
    {}
};

template <class ...Args>
__host__ __device__
inline CallInfo<decltype(hip::std::forward_as_tuple(hip::std::declval<Args>()...))>
makeCallInfo(CallQuals quals, Args&&... args) {
    return {quals, hip::std::forward<Args>(args)...};
}

struct TrackedCallable {

    TrackedCallable() = default;

    template <class ...Args>
    __host__ __device__
    auto operator()(Args&&... xargs) &
    { return makeCallInfo(CQ_LValue, hip::std::forward<Args>(xargs)...); }

    template <class ...Args>
    __host__ __device__
    auto  operator()(Args&&... xargs) const&
    { return makeCallInfo(CQ_ConstLValue, hip::std::forward<Args>(xargs)...); }

    template <class ...Args>
    __host__ __device__
    auto  operator()(Args&&... xargs) &&
    { return makeCallInfo(CQ_RValue, hip::std::forward<Args>(xargs)...); }

    template <class ...Args>
    __host__ __device__
    auto  operator()(Args&&... xargs) const&&
    { return makeCallInfo(CQ_ConstRValue, hip::std::forward<Args>(xargs)...); }
};

template <class ...ExpectArgs, class Tuple>
__host__ __device__
void check_apply_quals_and_types(Tuple&& t) {
    TypeID const* const expect_args = &makeArgumentID<ExpectArgs...>();
    TrackedCallable obj;
    TrackedCallable const& cobj = obj;
    {
        auto ret = hip::std::apply(obj, hip::std::forward<Tuple>(t));
        assert(ret.quals == CQ_LValue);
        assert(ret.arg_types == expect_args);
        assert(ret.args == t);
    }
    {
        auto ret = hip::std::apply(cobj, hip::std::forward<Tuple>(t));
        assert(ret.quals == CQ_ConstLValue);
        assert(ret.arg_types == expect_args);
        assert(ret.args == t);
    }
    {
        auto ret = hip::std::apply(hip::std::move(obj), hip::std::forward<Tuple>(t));
        assert(ret.quals == CQ_RValue);
        assert(ret.arg_types == expect_args);
        assert(ret.args == t);
    }
    {
        auto ret = hip::std::apply(hip::std::move(cobj), hip::std::forward<Tuple>(t));
        assert(ret.quals == CQ_ConstRValue);
        assert(ret.arg_types == expect_args);
        assert(ret.args == t);
    }
}

__host__ __device__
void test_call_quals_and_arg_types()
{
    using Tup = hip::std::tuple<int, int const&, unsigned&&>;
    const int x = 42;
    unsigned y = 101;
    Tup t(-1, x, hip::std::move(y));
    Tup const& ct = t;
    check_apply_quals_and_types<int&, int const&, unsigned&>(t);
    check_apply_quals_and_types<int const&, int const&, unsigned&>(ct);
    check_apply_quals_and_types<int&&, int const&, unsigned&&>(hip::std::move(t));
    check_apply_quals_and_types<int const&&, int const&, unsigned&&>(hip::std::move(ct));
}


struct NothrowMoveable {
    NothrowMoveable() noexcept = default;
    __host__ __device__ NothrowMoveable(NothrowMoveable const&) noexcept(false) {}
    __host__ __device__ NothrowMoveable(NothrowMoveable&&) noexcept {}
};

template <bool IsNoexcept>
struct TestNoexceptCallable {
  template <class ...Args>
  __host__ __device__ NothrowMoveable operator()(Args...) const noexcept(IsNoexcept) { return {}; }
};

__host__ __device__
void test_noexcept()
{
    TestNoexceptCallable<true> nec;
    TestNoexceptCallable<false> tc;
    {
        // test that the functions noexcept-ness is propagated
        using Tup = hip::std::tuple<int, const char*, long>;
        Tup t;
        LIBCPP_ASSERT_NOEXCEPT(hip::std::apply(nec, t));
        ASSERT_NOT_NOEXCEPT(hip::std::apply(tc, t));
        unused(t);
    }
    {
        // test that the noexcept-ness of the argument conversions is checked.
        using Tup = hip::std::tuple<NothrowMoveable, int>;
        Tup t;
        ASSERT_NOT_NOEXCEPT(hip::std::apply(nec, t));
        LIBCPP_ASSERT_NOEXCEPT(hip::std::apply(nec, hip::std::move(t)));
        unused(t);
    }
}

namespace ReturnTypeTest {
    #if defined(__CUDA_ARCH__) || defined(__HIP_DEVICE_COMPILE__)
    __constant__ int my_int = 42;
    #else
    static int my_int = 42;
    #endif

    template <int N> struct index {};

    __host__ __device__
    void f(index<0>) {}

    __host__ __device__
    int f(index<1>) { return 0; }

    __host__ __device__
    int & f(index<2>) { return static_cast<int &>(my_int); }
    __host__ __device__
    int const & f(index<3>) { return static_cast<int const &>(my_int); }
    __host__ __device__
    int volatile & f(index<4>) { return static_cast<int volatile &>(my_int); }
    __host__ __device__
    int const volatile & f(index<5>) { return static_cast<int const volatile &>(my_int); }

    __host__ __device__
    int && f(index<6>) { return static_cast<int &&>(my_int); }
    __host__ __device__
    int const && f(index<7>) { return static_cast<int const &&>(my_int); }
    __host__ __device__
    int volatile && f(index<8>) { return static_cast<int volatile &&>(my_int); }
    __host__ __device__
    int const volatile && f(index<9>) { return static_cast<int const volatile &&>(my_int); }

    __host__ __device__
    int * f(index<10>) { return static_cast<int *>(&my_int); }
    __host__ __device__
    int const * f(index<11>) { return static_cast<int const *>(&my_int); }
    __host__ __device__
    int volatile * f(index<12>) { return static_cast<int volatile *>(&my_int); }
    __host__ __device__
    int const volatile * f(index<13>) { return static_cast<int const volatile *>(&my_int); }

    template <int Func, class Expect>
    __host__ __device__
    void test()
    {
        using RawInvokeResult = decltype(f(index<Func>{}));
        static_assert(hip::std::is_same<RawInvokeResult, Expect>::value, "");
        using FnType = RawInvokeResult (*) (index<Func>);
        FnType fn = f;
        hip::std::tuple<index<Func>> t;
        using InvokeResult = decltype(hip::std::apply(fn, t));
        static_assert(hip::std::is_same<InvokeResult, Expect>::value, "");
        unused(t); unused(fn);
    }
} // end namespace ReturnTypeTest

__host__ __device__
void test_return_type()
{
    using ReturnTypeTest::test;
    test<0, void>();
    test<1, int>();
    test<2, int &>();
    test<3, int const &>();
    test<4, int volatile &>();
    test<5, int const volatile &>();
    test<6, int &&>();
    test<7, int const &&>();
    test<8, int volatile &&>();
    test<9, int const volatile &&>();
    test<10, int *>();
    test<11, int const *>();
    test<12, int volatile *>();
    test<13, int const volatile *>();
}

int main(int, char**) {
    test_constexpr_evaluation();
    test_call_quals_and_arg_types();
    test_return_type();
    test_noexcept();

  return 0;
}