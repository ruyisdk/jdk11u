/*
 * Copyright (c) 2000, 2010, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2014, Red Hat Inc. All rights reserved.
 * Copyright (c) 2020, Huawei Technologies Co., Ltd. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#include "precompiled.hpp"
#include "register_riscv32.hpp"

const int ConcreteRegisterImpl::max_gpr = RegisterImpl::number_of_registers;

const int ConcreteRegisterImpl::max_fpr
  = ConcreteRegisterImpl::max_gpr + (FloatRegisterImpl::number_of_registers << 1);

const char* RegisterImpl::name() const {
  const char* names[number_of_registers] = {
    "zr", "ra", "sp", "gp", "tp", "lr", "x6", "x7", "fp", "x9",
    "c_rarg0", "c_rarg1", "c_rarg2", "c_rarg3", "c_rarg4", "c_rarg5", "c_rarg6", "c_rarg7",
    "x18", "x19", "esp", "xdispatch", "xbcp", "xthread", "xlocals",
    "xmonitors", "xcpool", "xheapbase", "x28", "x29", "x30", "xmethod"
  };
  return is_valid() ? names[encoding()] : "noreg";
}

const char* FloatRegisterImpl::name() const {
  const char* names[number_of_registers] = {
    "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
    "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15",
    "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",
    "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31"
  };
  return is_valid() ? names[encoding()] : "noreg";
}
