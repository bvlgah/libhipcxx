#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

# Modifications Copyright (c) 2024 Advanced Micro Devices, Inc.
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import ast
import distutils.spawn
import sys
import re
import libhipcxx.util
from pprint import pformat


def read_syms_from_list(slist):
    """
    Read a list of symbols from a list of strings.
    Each string is one symbol.
    """
    return [ast.literal_eval(l) for l in slist]


def read_syms_from_file(filename):
    """
    Read a list of symbols in from a file.
    """
    with open(filename, 'r') as f:
        data = f.read()
    return read_syms_from_list(data.splitlines())


def read_blacklist(filename):
    with open(filename, 'r') as f:
        data = f.read()
    lines = [l.strip() for l in data.splitlines() if l.strip()]
    lines = [l for l in lines if not l.startswith('#')]
    return lines


def write_syms(sym_list, out=None, names_only=False, filter=None):
    """
    Write a list of symbols to the file named by out.
    """
    out_str = ''
    out_list = sym_list
    out_list.sort(key=lambda x: x['name'])
    if filter is not None:
        out_list = filter(out_list)
    if names_only:
        out_list = [sym['name'] for sym in out_list]
    for sym in out_list:
        # Use pformat for consistent ordering of keys.
        out_str += pformat(sym, width=100000) + '\n'
    if out is None:
        sys.stdout.write(out_str)
    else:
        with open(out, 'w') as f:
            f.write(out_str)


_cppfilt_exe = distutils.spawn.find_executable('c++filt')


def demangle_symbol(symbol):
    if _cppfilt_exe is None:
        return symbol
    out, _, exit_code = libhipcxx.util.executeCommandVerbose(
        [_cppfilt_exe], input=symbol)
    if exit_code != 0:
        return symbol
    return out


def is_elf(filename):
    with open(filename, 'rb') as f:
        magic_bytes = f.read(4)
    return magic_bytes == b'\x7fELF'


def is_mach_o(filename):
    with open(filename, 'rb') as f:
        magic_bytes = f.read(4)
    return magic_bytes in [
        '\xfe\xed\xfa\xce',  # MH_MAGIC
        '\xce\xfa\xed\xfe',  # MH_CIGAM
        '\xfe\xed\xfa\xcf',  # MH_MAGIC_64
        '\xcf\xfa\xed\xfe',  # MH_CIGAM_64
        '\xca\xfe\xba\xbe',  # FAT_MAGIC
        '\xbe\xba\xfe\xca'   # FAT_CIGAM
    ]


def is_library_file(filename):
    if sys.platform == 'darwin':
        return is_mach_o(filename)
    else:
        return is_elf(filename)


def extract_or_load(filename):
    import libhipcxx.sym_check.extract
    if is_library_file(filename):
        return libhipcxx.sym_check.extract.extract_symbols(filename)
    return read_syms_from_file(filename)

def adjust_mangled_name(name):
    if not name.startswith('__Z'):
        return name
    return name[1:]

new_delete_std_symbols = [
    '_Znam',
    '_Znwm',
    '_ZdaPv',
    '_ZdaPvm',
    '_ZdlPv',
    '_ZdlPvm'
]

cxxabi_symbols = [
    '___dynamic_cast',
    '___gxx_personality_v0',
    '_ZTIDi',
    '_ZTIDn',
    '_ZTIDs',
    '_ZTIPDi',
    '_ZTIPDn',
    '_ZTIPDs',
    '_ZTIPKDi',
    '_ZTIPKDn',
    '_ZTIPKDs',
    '_ZTIPKa',
    '_ZTIPKb',
    '_ZTIPKc',
    '_ZTIPKd',
    '_ZTIPKe',
    '_ZTIPKf',
    '_ZTIPKh',
    '_ZTIPKi',
    '_ZTIPKj',
    '_ZTIPKl',
    '_ZTIPKm',
    '_ZTIPKs',
    '_ZTIPKt',
    '_ZTIPKv',
    '_ZTIPKw',
    '_ZTIPKx',
    '_ZTIPKy',
    '_ZTIPa',
    '_ZTIPb',
    '_ZTIPc',
    '_ZTIPd',
    '_ZTIPe',
    '_ZTIPf',
    '_ZTIPh',
    '_ZTIPi',
    '_ZTIPj',
    '_ZTIPl',
    '_ZTIPm',
    '_ZTIPs',
    '_ZTIPt',
    '_ZTIPv',
    '_ZTIPw',
    '_ZTIPx',
    '_ZTIPy',
    '_ZTIa',
    '_ZTIb',
    '_ZTIc',
    '_ZTId',
    '_ZTIe',
    '_ZTIf',
    '_ZTIh',
    '_ZTIi',
    '_ZTIj',
    '_ZTIl',
    '_ZTIm',
    '_ZTIs',
    '_ZTIt',
    '_ZTIv',
    '_ZTIw',
    '_ZTIx',
    '_ZTIy',
    '_ZTSDi',
    '_ZTSDn',
    '_ZTSDs',
    '_ZTSPDi',
    '_ZTSPDn',
    '_ZTSPDs',
    '_ZTSPKDi',
    '_ZTSPKDn',
    '_ZTSPKDs',
    '_ZTSPKa',
    '_ZTSPKb',
    '_ZTSPKc',
    '_ZTSPKd',
    '_ZTSPKe',
    '_ZTSPKf',
    '_ZTSPKh',
    '_ZTSPKi',
    '_ZTSPKj',
    '_ZTSPKl',
    '_ZTSPKm',
    '_ZTSPKs',
    '_ZTSPKt',
    '_ZTSPKv',
    '_ZTSPKw',
    '_ZTSPKx',
    '_ZTSPKy',
    '_ZTSPa',
    '_ZTSPb',
    '_ZTSPc',
    '_ZTSPd',
    '_ZTSPe',
    '_ZTSPf',
    '_ZTSPh',
    '_ZTSPi',
    '_ZTSPj',
    '_ZTSPl',
    '_ZTSPm',
    '_ZTSPs',
    '_ZTSPt',
    '_ZTSPv',
    '_ZTSPw',
    '_ZTSPx',
    '_ZTSPy',
    '_ZTSa',
    '_ZTSb',
    '_ZTSc',
    '_ZTSd',
    '_ZTSe',
    '_ZTSf',
    '_ZTSh',
    '_ZTSi',
    '_ZTSj',
    '_ZTSl',
    '_ZTSm',
    '_ZTSs',
    '_ZTSt',
    '_ZTSv',
    '_ZTSw',
    '_ZTSx',
    '_ZTSy'
]

def is_stdlib_symbol_name(name, sym):
    name = adjust_mangled_name(name)
    if re.search("@GLIBC|@GCC", name):
        # Only when symbol is defined do we consider it ours
        return sym['is_defined']
    if re.search('(St[0-9])|(__cxa)|(__cxxabi)', name):
        return True
    if name in new_delete_std_symbols:
        return True
    if name in cxxabi_symbols:
        return True
    if name.startswith('_Z'):
        return True
    return False

def filter_stdlib_symbols(syms):
    stdlib_symbols = []
    other_symbols = []
    for s in syms:
        canon_name = adjust_mangled_name(s['name'])
        if not is_stdlib_symbol_name(canon_name, s):
            other_symbols += [s]
        else:
            stdlib_symbols += [s]
    return stdlib_symbols, other_symbols
