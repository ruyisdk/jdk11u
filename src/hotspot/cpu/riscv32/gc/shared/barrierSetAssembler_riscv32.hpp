/*
 * Copyright (c) 2018, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2020, Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2021, Institute of Software, Chinese Academy of Sciences. All rights reserved.
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

#ifndef CPU_RISCV32_GC_SHARED_BARRIERSETASSEMBLER_RISCV32_HPP
#define CPU_RISCV32_GC_SHARED_BARRIERSETASSEMBLER_RISCV32_HPP

#include "asm/macroAssembler.hpp"
#include "memory/allocation.hpp"
#include "oops/access.hpp"

class BarrierSetAssembler: public CHeapObj<mtGC> {
private:
  void incr_allocated_bytes(MacroAssembler* masm,
                            Register var_size_in_bytes, int con_size_in_bytes,
                            Register t1 = noreg);

public:
  virtual void arraycopy_prologue(MacroAssembler* masm, DecoratorSet decorators, bool is_oop,
                                  Register src, Register dst, Register count, RegSet saved_regs) {}
  virtual void arraycopy_epilogue(MacroAssembler* masm, DecoratorSet decorators, bool is_oop,
                                  Register start, Register end, Register tmp, RegSet saved_regs) {}
  virtual void load_at(MacroAssembler* masm, DecoratorSet decorators, BasicType type,
                       Register dst, Address src, Register tmp1, Register tmp_thread);
  virtual void store_at(MacroAssembler* masm, DecoratorSet decorators, BasicType type,
                        Address dst, Register val, Register tmp1, Register tmp2);
  virtual void obj_equals(MacroAssembler* masm, Register obj1, Register obj2, Label& equal, bool is_far = false);
  virtual void obj_nequals(MacroAssembler* masm, Register obj1, Register obj2, Label& nequal, bool is_far = false);
  virtual void try_resolve_jobject_in_native(MacroAssembler* masm, Register jni_env,
                                             Register obj, Register tmp, Label& slowpath);

  virtual void tlab_allocate(MacroAssembler* masm,
    Register obj,                      // result: pointer to object after successful allocation
    Register var_size_in_bytes,        // object size in bytes if unknown at compile time; invalid otherwise
    int      con_size_in_bytes,        // object size in bytes if   known at compile time
    Register tmp1,                     // temp register
    Register tmp2,                     // temp register
    Label&   slow_case,                // continuation point if fast allocation fails
    bool is_far = false                // the distance of label slowcase could be more than 12KiB in C1
  );

  void eden_allocate(MacroAssembler* masm,
    Register obj,                      // result: pointer to object after successful allocation
    Register var_size_in_bytes,        // object size in bytes if unknown at compile time; invalid otherwise
    int      con_size_in_bytes,        // object size in bytes if   known at compile time
    Register tmp1,                     // temp register
    Label&   slow_case,                // continuation point if fast allocation fails
    bool is_far = false                // the distance of label slowcase could be more than 12KiB in C1
  );
  virtual void barrier_stubs_init() {}
  virtual ~BarrierSetAssembler() {}
};

#endif // CPU_RISCV32_GC_SHARED_BARRIERSETASSEMBLER_RISCV32_HPP
