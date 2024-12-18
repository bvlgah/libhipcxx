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

// cuda::mr::async_resource

#define LIBCUDACXX_ENABLE_EXPERIMENTAL_MEMORY_RESOURCE

#include <hip/memory_resource>

#include <hip/std/cstdint>

#include "test_macros.h"

struct invalid_argument {};

struct valid_resource {
  void* allocate(std::size_t, std::size_t) { return nullptr; }
  void deallocate(void*, std::size_t, std::size_t) {}
  void* allocate_async(std::size_t, std::size_t, cuda::stream_ref) {
    return nullptr;
  }
  void deallocate_async(void*, std::size_t, std::size_t, cuda::stream_ref) {}
  bool operator==(const valid_resource&) const { return true; }
  bool operator!=(const valid_resource&) const { return false; }
};
static_assert(cuda::mr::async_resource<valid_resource>, "");

struct invalid_allocate_missing {
  void deallocate(void*, std::size_t, std::size_t) {}
  void* allocate_async(std::size_t, std::size_t, cuda::stream_ref) {
    return nullptr;
  }
  void deallocate_async(void*, std::size_t, std::size_t, cuda::stream_ref) {}
  bool operator==(const invalid_allocate_missing&) const { return true; }
  bool operator!=(const invalid_allocate_missing&) const { return false; }
};
static_assert(!cuda::mr::async_resource<invalid_allocate_missing>, "");

struct invalid_deallocate_missing {
  void* allocate(std::size_t, std::size_t) { return nullptr; }
  void* allocate_async(std::size_t, std::size_t, cuda::stream_ref) {
    return nullptr;
  }
  void deallocate_async(void*, std::size_t, std::size_t, cuda::stream_ref) {}
  bool operator==(const invalid_allocate_missing&) const { return true; }
  bool operator!=(const invalid_allocate_missing&) const { return false; }
};
static_assert(!cuda::mr::async_resource<invalid_deallocate_missing>, "");

struct invalid_allocate_async_argument {
  void* allocate(std::size_t, std::size_t) { return nullptr; }
  void deallocate(void*, std::size_t, std::size_t) {}
  void* allocate_async(invalid_argument, std::size_t) { return nullptr; }
  void deallocate_async(void*, std::size_t, std::size_t, cuda::stream_ref) {}
  bool operator==(const invalid_allocate_async_argument&) const { return true; }
  bool operator!=(const invalid_allocate_async_argument&) const { return false; }
};
static_assert(!cuda::mr::async_resource<invalid_allocate_async_argument>, "");

struct invalid_allocate_async_return {
  void* allocate(std::size_t, std::size_t) { return nullptr; }
  void deallocate(void*, std::size_t, std::size_t) {}
  int allocate_async(std::size_t, std::size_t, cuda::stream_ref) { return 42; }
  void deallocate_async(void*, std::size_t, std::size_t, cuda::stream_ref) {}
  bool operator==(const invalid_allocate_async_return&) const { return true; }
  bool operator!=(const invalid_allocate_async_return&) const { return false; }
};
static_assert(!cuda::mr::async_resource<invalid_allocate_async_return>, "");

struct invalid_deallocate_async_argument {
  void* allocate(std::size_t, std::size_t) { return nullptr; }
  void deallocate(void*, std::size_t, std::size_t) {}
  void* allocate_async(std::size_t, std::size_t, cuda::stream_ref) {
    return nullptr;
  }
  void deallocate_async(void*, invalid_argument, std::size_t) {}
  bool operator==(const invalid_deallocate_async_argument&) const { return true; }
  bool operator!=(const invalid_deallocate_async_argument&) const { return false; }
};
static_assert(!cuda::mr::async_resource<invalid_deallocate_async_argument>, "");

struct non_comparable {
  void* allocate(std::size_t, std::size_t) { return nullptr; }
  void deallocate(void*, std::size_t, std::size_t) {}
  void* allocate_async(std::size_t, std::size_t, cuda::stream_ref) {
    return nullptr;
  }
  void deallocate_async(void*, std::size_t, std::size_t, cuda::stream_ref) {}
};
static_assert(!cuda::mr::async_resource<non_comparable>, "");

struct non_eq_comparable {
  void* allocate(std::size_t, std::size_t) { return nullptr; }
  void deallocate(void*, std::size_t, std::size_t) {}
  void* allocate_async(std::size_t, std::size_t, cuda::stream_ref) {
    return nullptr;
  }
  void deallocate_async(void*, std::size_t, std::size_t, cuda::stream_ref) {}
  bool operator!=(const non_eq_comparable&) const { return false; }
};
static_assert(!cuda::mr::async_resource<non_eq_comparable>, "");

#if TEST_STD_VER < 20
struct non_neq_comparable {
  void* allocate(std::size_t, std::size_t) { return nullptr; }
  void deallocate(void*, std::size_t, std::size_t) {}
  void* allocate_async(std::size_t, std::size_t, cuda::stream_ref) {
    return nullptr;
  }
  void deallocate_async(void*, std::size_t, std::size_t, cuda::stream_ref) {}
  bool operator==(const non_neq_comparable&) const { return true; }
};
static_assert(!cuda::mr::async_resource<non_neq_comparable>, "");
#endif // TEST_STD_VER < 20

int main(int, char**) { return 0; }
