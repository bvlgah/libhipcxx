//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES.
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

// UNSUPPORTED: c++03, c++11
// UNSUPPORTED: nvrtc
// UNSUPPORTED: windows

// cuda::mr::resource_ref properties

#define LIBCUDACXX_ENABLE_EXPERIMENTAL_MEMORY_RESOURCE

#include <hip/memory_resource>

#include <hip/std/cassert>
#include <hip/std/cstdint>

template <class T>
struct property_with_value {
  using value_type = T;
};

template <class T>
struct property_without_value {};

template <class... Properties>
struct resource_base {
  virtual void* allocate(std::size_t, std::size_t) = 0;

  virtual void deallocate(void* ptr, std::size_t, std::size_t) = 0;

  bool operator==(const resource_base& other) const { return true; }
  bool operator!=(const resource_base& other) const { return false; }

  _LIBCUDACXX_TEMPLATE(class Property)
    (requires (!cuda::property_with_value<Property>) && _CUDA_VSTD::_One_of<Property, Properties...>) //
  friend void get_property(const resource_base&, Property) noexcept {}

  _LIBCUDACXX_TEMPLATE(class Property)
    (requires cuda::property_with_value<Property> && _CUDA_VSTD::_One_of<Property, Properties...>) //
  friend typename Property::value_type get_property(const resource_base& res, Property) noexcept {
    return 42;
  }
};

template <class... Properties>
struct resource_derived_first : public resource_base<Properties...> {
  using super_t = resource_base<Properties...>;

  resource_derived_first(const int val) : _val(val) {}

  void* allocate(std::size_t, std::size_t) override { return &_val; }

  void deallocate(void* ptr, std::size_t, std::size_t) override {}

  bool operator==(const resource_derived_first& other) const { return true; }
  bool operator!=(const resource_derived_first& other) const { return false; }

  int _val = 0;
};
static_assert(cuda::mr::resource<resource_derived_first<> >, "");

struct some_data {
  int _val;
};

template <class... Properties>
struct resource_derived_second : public resource_base<Properties...> {
  using super_t = resource_base<Properties...>;

  resource_derived_second(some_data* val) : _val(val) {}

  void* allocate(std::size_t, std::size_t) override { return &_val->_val; }

  void deallocate(void* ptr, std::size_t, std::size_t) override {}

  bool operator==(const resource_derived_second& other) const { return true; }
  bool operator!=(const resource_derived_second& other) const { return false; }

  some_data* _val = 0;
};

template <class... Properties>
void test_resource_ref() {
  some_data input{1337};
  resource_derived_first<Properties...> first{42};
  resource_derived_second<Properties...> second{&input};

  cuda::mr::resource_ref<Properties...> ref_first{first};
  cuda::mr::resource_ref<Properties...> ref_second{second};

  // Ensure that we properly pass on the allocate function
  assert(ref_first.allocate(0, 0) == first.allocate(0, 0));
  assert(ref_second.allocate(0, 0) == second.allocate(0, 0));

  // Ensure that assignment still works
  ref_second = ref_first;
  assert(ref_second.allocate(0, 0) == first.allocate(0, 0));
}

template <class... Properties>
cuda::mr::resource_ref<Properties...>
indirection(resource_base<Properties...>* res) {
  return {res};
}

template <class... Properties>
void test_resource_ref_from_pointer() {
  some_data input{1337};
  resource_derived_first<Properties...> first{42};
  resource_derived_second<Properties...> second{&input};

  cuda::mr::resource_ref<Properties...> ref_first = indirection(&first);
  cuda::mr::resource_ref<Properties...> ref_second = indirection(&second);

  // Ensure that we properly pass on the allocate function
  assert(ref_first.allocate(0, 0) == first.allocate(0, 0));
  assert(ref_second.allocate(0, 0) == second.allocate(0, 0));

  // Ensure that assignment still works
  ref_second = ref_first;
  assert(ref_second.allocate(0, 0) == first.allocate(0, 0));
}

// clang complains about pure virtual functions being called, so ensure that we properly crash if so
extern "C" void __cxa_pure_virtual() {
  while (1)
    ;
}

int main(int, char**) {
    NV_IF_TARGET(NV_IS_HOST,(
        // Test some basic combinations of properties w/o state
        test_resource_ref<property_with_value<short>, property_with_value<int> >();
        test_resource_ref<property_with_value<short>, property_without_value<int> >();
        test_resource_ref<property_without_value<short>,
                          property_without_value<int> >();

        test_resource_ref_from_pointer<property_with_value<short>,
                                      property_with_value<int> >();
        test_resource_ref_from_pointer<property_with_value<short>,
                                      property_without_value<int> >();
        test_resource_ref_from_pointer<property_without_value<short>,
                                      property_without_value<int> >();
    ))

    return 0;
}
