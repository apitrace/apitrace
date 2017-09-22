/**************************************************************************
 *
 * Copyright 2008-2009 VMware, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/

/*
 * MinGW compatability macros to allow using recent's DXSDK headers.
 */

#ifdef __MINGW32__


#include <windows.h>


/*
 * Several of the defines below clash with libstdc++ internal variables,
 * so include the headers now, before they are re-defined.
 */
#ifdef __cplusplus
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
#endif


/*
 * Dummy definitions Standard Annotation Language (SAL) definitions
 *
 * These are typically defined in sal.h.
 *
 * http://svn.reactos.org/svn/reactos/trunk/reactos/include/psdk/sal.h?view=markup
 */

#ifndef _Always_
#define _Always_(annos)
#endif

#ifndef _Analysis_noreturn_
#define _Analysis_noreturn_
#endif

#ifndef _Analysis_assume_
#define _Analysis_assume_(expr)
#endif

#ifndef _At_
#define _At_(target, annos)
#endif

#ifndef _At_buffer_
#define _At_buffer_(target, iter, bound, annos)
#endif

#ifndef _COM_Outptr_
#define _COM_Outptr_
#endif

#ifndef _COM_Outptr_opt_
#define _COM_Outptr_opt_
#endif

#ifndef _COM_Outptr_opt_result_maybenull_
#define _COM_Outptr_opt_result_maybenull_
#endif

#ifndef _COM_Outptr_result_maybenull_
#define _COM_Outptr_result_maybenull_
#endif

#ifndef _Check_return_
#define _Check_return_
#endif

#ifndef _Const_
#define _Const_
#endif

#ifndef _Deref2_pre_readonly_
#define _Deref2_pre_readonly_
#endif

#ifndef _Deref_in_bound_
#define _Deref_in_bound_
#endif

#ifndef _Deref_in_range_
#define _Deref_in_range_(lb,ub)
#endif

#ifndef _Deref_inout_bound_
#define _Deref_inout_bound_
#endif

#ifndef _Deref_inout_z_
#define _Deref_inout_z_
#endif

#ifndef _Deref_inout_z_bytecap_c_
#define _Deref_inout_z_bytecap_c_(size)
#endif

#ifndef _Deref_inout_z_cap_c_
#define _Deref_inout_z_cap_c_(size)
#endif

#ifndef _Deref_opt_out_
#define _Deref_opt_out_
#endif

#ifndef _Deref_opt_out_opt_
#define _Deref_opt_out_opt_
#endif

#ifndef _Deref_opt_out_opt_z_
#define _Deref_opt_out_opt_z_
#endif

#ifndef _Deref_opt_out_z_
#define _Deref_opt_out_z_
#endif

#ifndef _Deref_out_
#define _Deref_out_
#endif

#ifndef _Deref_out_bound_
#define _Deref_out_bound_
#endif

#ifndef _Deref_out_opt_
#define _Deref_out_opt_
#endif

#ifndef _Deref_out_opt_z_
#define _Deref_out_opt_z_
#endif

#ifndef _Deref_out_range_
#define _Deref_out_range_(lb,ub)
#endif

#ifndef _Deref_out_z_
#define _Deref_out_z_
#endif

#ifndef _Deref_out_z_bytecap_c_
#define _Deref_out_z_bytecap_c_(size)
#endif

#ifndef _Deref_out_z_cap_c_
#define _Deref_out_z_cap_c_(size)
#endif

#ifndef _Deref_post_bytecap_
#define _Deref_post_bytecap_(size)
#endif

#ifndef _Deref_post_bytecap_c_
#define _Deref_post_bytecap_c_(size)
#endif

#ifndef _Deref_post_bytecap_x_
#define _Deref_post_bytecap_x_(size)
#endif

#ifndef _Deref_post_bytecount_
#define _Deref_post_bytecount_(size)
#endif

#ifndef _Deref_post_bytecount_c_
#define _Deref_post_bytecount_c_(size)
#endif

#ifndef _Deref_post_bytecount_x_
#define _Deref_post_bytecount_x_(size)
#endif

#ifndef _Deref_post_cap_
#define _Deref_post_cap_(size)
#endif

#ifndef _Deref_post_cap_c_
#define _Deref_post_cap_c_(size)
#endif

#ifndef _Deref_post_cap_x_
#define _Deref_post_cap_x_(size)
#endif

#ifndef _Deref_post_count_
#define _Deref_post_count_(size)
#endif

#ifndef _Deref_post_count_c_
#define _Deref_post_count_c_(size)
#endif

#ifndef _Deref_post_count_x_
#define _Deref_post_count_x_(size)
#endif

#ifndef _Deref_post_maybenull_
#define _Deref_post_maybenull_
#endif

#ifndef _Deref_post_notnull_
#define _Deref_post_notnull_
#endif

#ifndef _Deref_post_null_
#define _Deref_post_null_
#endif

#ifndef _Deref_post_opt_bytecap_
#define _Deref_post_opt_bytecap_(size)
#endif

#ifndef _Deref_post_opt_bytecap_c_
#define _Deref_post_opt_bytecap_c_(size)
#endif

#ifndef _Deref_post_opt_bytecap_x_
#define _Deref_post_opt_bytecap_x_(size)
#endif

#ifndef _Deref_post_opt_bytecount_
#define _Deref_post_opt_bytecount_(size)
#endif

#ifndef _Deref_post_opt_bytecount_c_
#define _Deref_post_opt_bytecount_c_(size)
#endif

#ifndef _Deref_post_opt_bytecount_x_
#define _Deref_post_opt_bytecount_x_(size)
#endif

#ifndef _Deref_post_opt_cap_
#define _Deref_post_opt_cap_(size)
#endif

#ifndef _Deref_post_opt_cap_c_
#define _Deref_post_opt_cap_c_(size)
#endif

#ifndef _Deref_post_opt_cap_x_
#define _Deref_post_opt_cap_x_(size)
#endif

#ifndef _Deref_post_opt_count_
#define _Deref_post_opt_count_(size)
#endif

#ifndef _Deref_post_opt_count_c_
#define _Deref_post_opt_count_c_(size)
#endif

#ifndef _Deref_post_opt_count_x_
#define _Deref_post_opt_count_x_(size)
#endif

#ifndef _Deref_post_opt_valid_
#define _Deref_post_opt_valid_
#endif

#ifndef _Deref_post_opt_valid_bytecap_
#define _Deref_post_opt_valid_bytecap_(size)
#endif

#ifndef _Deref_post_opt_valid_bytecap_c_
#define _Deref_post_opt_valid_bytecap_c_(size)
#endif

#ifndef _Deref_post_opt_valid_bytecap_x_
#define _Deref_post_opt_valid_bytecap_x_(size)
#endif

#ifndef _Deref_post_opt_valid_cap_
#define _Deref_post_opt_valid_cap_(size)
#endif

#ifndef _Deref_post_opt_valid_cap_c_
#define _Deref_post_opt_valid_cap_c_(size)
#endif

#ifndef _Deref_post_opt_valid_cap_x_
#define _Deref_post_opt_valid_cap_x_(size)
#endif

#ifndef _Deref_post_opt_z_
#define _Deref_post_opt_z_
#endif

#ifndef _Deref_post_opt_z_bytecap_
#define _Deref_post_opt_z_bytecap_(size)
#endif

#ifndef _Deref_post_opt_z_bytecap_c_
#define _Deref_post_opt_z_bytecap_c_(size)
#endif

#ifndef _Deref_post_opt_z_bytecap_x_
#define _Deref_post_opt_z_bytecap_x_(size)
#endif

#ifndef _Deref_post_opt_z_cap_
#define _Deref_post_opt_z_cap_(size)
#endif

#ifndef _Deref_post_opt_z_cap_c_
#define _Deref_post_opt_z_cap_c_(size)
#endif

#ifndef _Deref_post_opt_z_cap_x_
#define _Deref_post_opt_z_cap_x_(size)
#endif

#ifndef _Deref_post_valid_
#define _Deref_post_valid_
#endif

#ifndef _Deref_post_valid_bytecap_
#define _Deref_post_valid_bytecap_(size)
#endif

#ifndef _Deref_post_valid_bytecap_c_
#define _Deref_post_valid_bytecap_c_(size)
#endif

#ifndef _Deref_post_valid_bytecap_x_
#define _Deref_post_valid_bytecap_x_(size)
#endif

#ifndef _Deref_post_valid_cap_
#define _Deref_post_valid_cap_(size)
#endif

#ifndef _Deref_post_valid_cap_c_
#define _Deref_post_valid_cap_c_(size)
#endif

#ifndef _Deref_post_valid_cap_x_
#define _Deref_post_valid_cap_x_(size)
#endif

#ifndef _Deref_post_z_
#define _Deref_post_z_
#endif

#ifndef _Deref_post_z_bytecap_
#define _Deref_post_z_bytecap_(size)
#endif

#ifndef _Deref_post_z_bytecap_c_
#define _Deref_post_z_bytecap_c_(size)
#endif

#ifndef _Deref_post_z_bytecap_x_
#define _Deref_post_z_bytecap_x_(size)
#endif

#ifndef _Deref_post_z_cap_
#define _Deref_post_z_cap_(size)
#endif

#ifndef _Deref_post_z_cap_c_
#define _Deref_post_z_cap_c_(size)
#endif

#ifndef _Deref_post_z_cap_x_
#define _Deref_post_z_cap_x_(size)
#endif

#ifndef _Deref_pre_bytecap_
#define _Deref_pre_bytecap_(size)
#endif

#ifndef _Deref_pre_bytecap_c_
#define _Deref_pre_bytecap_c_(size)
#endif

#ifndef _Deref_pre_bytecap_x_
#define _Deref_pre_bytecap_x_(size)
#endif

#ifndef _Deref_pre_bytecount_
#define _Deref_pre_bytecount_(size)
#endif

#ifndef _Deref_pre_bytecount_c_
#define _Deref_pre_bytecount_c_(size)
#endif

#ifndef _Deref_pre_bytecount_x_
#define _Deref_pre_bytecount_x_(size)
#endif

#ifndef _Deref_pre_cap_
#define _Deref_pre_cap_(size)
#endif

#ifndef _Deref_pre_cap_c_
#define _Deref_pre_cap_c_(size)
#endif

#ifndef _Deref_pre_cap_x_
#define _Deref_pre_cap_x_(size)
#endif

#ifndef _Deref_pre_count_
#define _Deref_pre_count_(size)
#endif

#ifndef _Deref_pre_count_c_
#define _Deref_pre_count_c_(size)
#endif

#ifndef _Deref_pre_count_x_
#define _Deref_pre_count_x_(size)
#endif

#ifndef _Deref_pre_invalid_
#define _Deref_pre_invalid_
#endif

#ifndef _Deref_pre_maybenull_
#define _Deref_pre_maybenull_
#endif

#ifndef _Deref_pre_notnull_
#define _Deref_pre_notnull_
#endif

#ifndef _Deref_pre_null_
#define _Deref_pre_null_
#endif

#ifndef _Deref_pre_opt_bytecap_
#define _Deref_pre_opt_bytecap_(size)
#endif

#ifndef _Deref_pre_opt_bytecap_c_
#define _Deref_pre_opt_bytecap_c_(size)
#endif

#ifndef _Deref_pre_opt_bytecap_x_
#define _Deref_pre_opt_bytecap_x_(size)
#endif

#ifndef _Deref_pre_opt_bytecount_
#define _Deref_pre_opt_bytecount_(size)
#endif

#ifndef _Deref_pre_opt_bytecount_c_
#define _Deref_pre_opt_bytecount_c_(size)
#endif

#ifndef _Deref_pre_opt_bytecount_x_
#define _Deref_pre_opt_bytecount_x_(size)
#endif

#ifndef _Deref_pre_opt_cap_
#define _Deref_pre_opt_cap_(size)
#endif

#ifndef _Deref_pre_opt_cap_c_
#define _Deref_pre_opt_cap_c_(size)
#endif

#ifndef _Deref_pre_opt_cap_x_
#define _Deref_pre_opt_cap_x_(size)
#endif

#ifndef _Deref_pre_opt_count_
#define _Deref_pre_opt_count_(size)
#endif

#ifndef _Deref_pre_opt_count_c_
#define _Deref_pre_opt_count_c_(size)
#endif

#ifndef _Deref_pre_opt_count_x_
#define _Deref_pre_opt_count_x_(size)
#endif

#ifndef _Deref_pre_opt_valid_
#define _Deref_pre_opt_valid_
#endif

#ifndef _Deref_pre_opt_valid_bytecap_
#define _Deref_pre_opt_valid_bytecap_(size)
#endif

#ifndef _Deref_pre_opt_valid_bytecap_c_
#define _Deref_pre_opt_valid_bytecap_c_(size)
#endif

#ifndef _Deref_pre_opt_valid_bytecap_x_
#define _Deref_pre_opt_valid_bytecap_x_(size)
#endif

#ifndef _Deref_pre_opt_valid_cap_
#define _Deref_pre_opt_valid_cap_(size)
#endif

#ifndef _Deref_pre_opt_valid_cap_c_
#define _Deref_pre_opt_valid_cap_c_(size)
#endif

#ifndef _Deref_pre_opt_valid_cap_x_
#define _Deref_pre_opt_valid_cap_x_(size)
#endif

#ifndef _Deref_pre_opt_z_
#define _Deref_pre_opt_z_
#endif

#ifndef _Deref_pre_opt_z_bytecap_
#define _Deref_pre_opt_z_bytecap_(size)
#endif

#ifndef _Deref_pre_opt_z_bytecap_c_
#define _Deref_pre_opt_z_bytecap_c_(size)
#endif

#ifndef _Deref_pre_opt_z_bytecap_x_
#define _Deref_pre_opt_z_bytecap_x_(size)
#endif

#ifndef _Deref_pre_opt_z_cap_
#define _Deref_pre_opt_z_cap_(size)
#endif

#ifndef _Deref_pre_opt_z_cap_c_
#define _Deref_pre_opt_z_cap_c_(size)
#endif

#ifndef _Deref_pre_opt_z_cap_x_
#define _Deref_pre_opt_z_cap_x_(size)
#endif

#ifndef _Deref_pre_readonly_
#define _Deref_pre_readonly_
#endif

#ifndef _Deref_pre_valid_
#define _Deref_pre_valid_
#endif

#ifndef _Deref_pre_valid_bytecap_
#define _Deref_pre_valid_bytecap_(size)
#endif

#ifndef _Deref_pre_valid_bytecap_c_
#define _Deref_pre_valid_bytecap_c_(size)
#endif

#ifndef _Deref_pre_valid_bytecap_x_
#define _Deref_pre_valid_bytecap_x_(size)
#endif

#ifndef _Deref_pre_valid_cap_
#define _Deref_pre_valid_cap_(size)
#endif

#ifndef _Deref_pre_valid_cap_c_
#define _Deref_pre_valid_cap_c_(size)
#endif

#ifndef _Deref_pre_valid_cap_x_
#define _Deref_pre_valid_cap_x_(size)
#endif

#ifndef _Deref_pre_writeonly_
#define _Deref_pre_writeonly_
#endif

#ifndef _Deref_pre_z_
#define _Deref_pre_z_
#endif

#ifndef _Deref_pre_z_bytecap_
#define _Deref_pre_z_bytecap_(size)
#endif

#ifndef _Deref_pre_z_bytecap_c_
#define _Deref_pre_z_bytecap_c_(size)
#endif

#ifndef _Deref_pre_z_bytecap_x_
#define _Deref_pre_z_bytecap_x_(size)
#endif

#ifndef _Deref_pre_z_cap_
#define _Deref_pre_z_cap_(size)
#endif

#ifndef _Deref_pre_z_cap_c_
#define _Deref_pre_z_cap_c_(size)
#endif

#ifndef _Deref_pre_z_cap_x_
#define _Deref_pre_z_cap_x_(size)
#endif

#ifndef _Deref_prepost_bytecap_
#define _Deref_prepost_bytecap_(size)
#endif

#ifndef _Deref_prepost_bytecap_x_
#define _Deref_prepost_bytecap_x_(size)
#endif

#ifndef _Deref_prepost_bytecount_
#define _Deref_prepost_bytecount_(size)
#endif

#ifndef _Deref_prepost_bytecount_x_
#define _Deref_prepost_bytecount_x_(size)
#endif

#ifndef _Deref_prepost_cap_
#define _Deref_prepost_cap_(size)
#endif

#ifndef _Deref_prepost_cap_x_
#define _Deref_prepost_cap_x_(size)
#endif

#ifndef _Deref_prepost_count_
#define _Deref_prepost_count_(size)
#endif

#ifndef _Deref_prepost_count_x_
#define _Deref_prepost_count_x_(size)
#endif

#ifndef _Deref_prepost_opt_bytecap_
#define _Deref_prepost_opt_bytecap_(size)
#endif

#ifndef _Deref_prepost_opt_bytecap_x_
#define _Deref_prepost_opt_bytecap_x_(size)
#endif

#ifndef _Deref_prepost_opt_bytecount_
#define _Deref_prepost_opt_bytecount_(size)
#endif

#ifndef _Deref_prepost_opt_bytecount_x_
#define _Deref_prepost_opt_bytecount_x_(size)
#endif

#ifndef _Deref_prepost_opt_cap_
#define _Deref_prepost_opt_cap_(size)
#endif

#ifndef _Deref_prepost_opt_cap_x_
#define _Deref_prepost_opt_cap_x_(size)
#endif

#ifndef _Deref_prepost_opt_count_
#define _Deref_prepost_opt_count_(size)
#endif

#ifndef _Deref_prepost_opt_count_x_
#define _Deref_prepost_opt_count_x_(size)
#endif

#ifndef _Deref_prepost_opt_valid_
#define _Deref_prepost_opt_valid_
#endif

#ifndef _Deref_prepost_opt_valid_bytecap_
#define _Deref_prepost_opt_valid_bytecap_(size)
#endif

#ifndef _Deref_prepost_opt_valid_bytecap_x_
#define _Deref_prepost_opt_valid_bytecap_x_(size)
#endif

#ifndef _Deref_prepost_opt_valid_cap_
#define _Deref_prepost_opt_valid_cap_(size)
#endif

#ifndef _Deref_prepost_opt_valid_cap_x_
#define _Deref_prepost_opt_valid_cap_x_(size)
#endif

#ifndef _Deref_prepost_opt_z_
#define _Deref_prepost_opt_z_
#endif

#ifndef _Deref_prepost_opt_z_bytecap_
#define _Deref_prepost_opt_z_bytecap_(size)
#endif

#ifndef _Deref_prepost_opt_z_cap_
#define _Deref_prepost_opt_z_cap_(size)
#endif

#ifndef _Deref_prepost_valid_
#define _Deref_prepost_valid_
#endif

#ifndef _Deref_prepost_valid_bytecap_
#define _Deref_prepost_valid_bytecap_(size)
#endif

#ifndef _Deref_prepost_valid_bytecap_x_
#define _Deref_prepost_valid_bytecap_x_(size)
#endif

#ifndef _Deref_prepost_valid_cap_
#define _Deref_prepost_valid_cap_(size)
#endif

#ifndef _Deref_prepost_valid_cap_x_
#define _Deref_prepost_valid_cap_x_(size)
#endif

#ifndef _Deref_prepost_z_
#define _Deref_prepost_z_
#endif

#ifndef _Deref_prepost_z_bytecap_
#define _Deref_prepost_z_bytecap_(size)
#endif

#ifndef _Deref_prepost_z_cap_
#define _Deref_prepost_z_cap_(size)
#endif

#ifndef _Deref_ret_bound_
#define _Deref_ret_bound_
#endif

#ifndef _Deref_ret_opt_z_
#define _Deref_ret_opt_z_
#endif

#ifndef _Deref_ret_range_
#define _Deref_ret_range_(lb,ub)
#endif

#ifndef _Deref_ret_z_
#define _Deref_ret_z_
#endif

#ifndef _Field_range_
#define _Field_range_(min,max)
#endif

#ifndef _Field_size_
#define _Field_size_(size)
#endif

#ifndef _Field_size_bytes_
#define _Field_size_bytes_(size)
#endif

#ifndef _Field_size_bytes_full_
#define _Field_size_bytes_full_(size)
#endif

#ifndef _Field_size_bytes_full_opt_
#define _Field_size_bytes_full_opt_(size)
#endif

#ifndef _Field_size_bytes_opt_
#define _Field_size_bytes_opt_(size)
#endif

#ifndef _Field_size_bytes_part_
#define _Field_size_bytes_part_(size, count)
#endif

#ifndef _Field_size_bytes_part_opt_
#define _Field_size_bytes_part_opt_(size, count)
#endif

#ifndef _Field_size_full_
#define _Field_size_full_(size)
#endif

#ifndef _Field_size_full_opt_
#define _Field_size_full_opt_(size)
#endif

#ifndef _Field_size_opt_
#define _Field_size_opt_(size)
#endif

#ifndef _Field_size_part_
#define _Field_size_part_(size, count)
#endif

#ifndef _Field_size_part_opt_
#define _Field_size_part_opt_(size, count)
#endif

#ifndef _Field_z_
#define _Field_z_
#endif

#ifndef _Function_class_
#define _Function_class_(x)
#endif

#ifndef _Group_
#define _Group_(annos)
#endif

#ifndef _In_
#define _In_
#endif

#ifndef _In_bound_
#define _In_bound_
#endif

#ifndef _In_bytecount_
#define _In_bytecount_(size)
#endif

#ifndef _In_bytecount_c_
#define _In_bytecount_c_(size)
#endif

#ifndef _In_bytecount_x_
#define _In_bytecount_x_(size)
#endif

#ifndef _In_count_
#define _In_count_(size)
#endif

#ifndef _In_count_c_
#define _In_count_c_(size)
#endif

#ifndef _In_count_x_
#define _In_count_x_(size)
#endif

#ifndef _In_defensive_
#define _In_defensive_(annotes)
#endif

#ifndef _In_opt_
#define _In_opt_
#endif

#ifndef _In_opt_bytecount_
#define _In_opt_bytecount_(size)
#endif

#ifndef _In_opt_bytecount_c_
#define _In_opt_bytecount_c_(size)
#endif

#ifndef _In_opt_bytecount_x_
#define _In_opt_bytecount_x_(size)
#endif

#ifndef _In_opt_count_
#define _In_opt_count_(size)
#endif

#ifndef _In_opt_count_c_
#define _In_opt_count_c_(size)
#endif

#ifndef _In_opt_count_x_
#define _In_opt_count_x_(size)
#endif

#ifndef _In_opt_ptrdiff_count_
#define _In_opt_ptrdiff_count_(size)
#endif

#ifndef _In_opt_z_
#define _In_opt_z_
#endif

#ifndef _In_opt_z_bytecount_
#define _In_opt_z_bytecount_(size)
#endif

#ifndef _In_opt_z_bytecount_c_
#define _In_opt_z_bytecount_c_(size)
#endif

#ifndef _In_opt_z_count_
#define _In_opt_z_count_(size)
#endif

#ifndef _In_opt_z_count_c_
#define _In_opt_z_count_c_(size)
#endif

#ifndef _In_ptrdiff_count_
#define _In_ptrdiff_count_(size)
#endif

#ifndef _In_range_
#define _In_range_(lb,ub)
#endif

#ifndef _In_reads_
#define _In_reads_(size)
#endif

#ifndef _In_reads_bytes_
#define _In_reads_bytes_(size)
#endif

#ifndef _In_reads_bytes_opt_
#define _In_reads_bytes_opt_(size)
#endif

#ifndef _In_reads_opt_
#define _In_reads_opt_(size)
#endif

#ifndef _In_reads_opt_z_
#define _In_reads_opt_z_(size)
#endif

#ifndef _In_reads_or_z_
#define _In_reads_or_z_(size)
#endif

#ifndef _In_reads_to_ptr_
#define _In_reads_to_ptr_(ptr)
#endif

#ifndef _In_reads_to_ptr_opt_
#define _In_reads_to_ptr_opt_(ptr)
#endif

#ifndef _In_reads_to_ptr_opt_z_
#define _In_reads_to_ptr_opt_z_(ptr)
#endif

#ifndef _In_reads_to_ptr_z_
#define _In_reads_to_ptr_z_(ptr)
#endif

#ifndef _In_reads_z_
#define _In_reads_z_(size)
#endif

#ifndef _In_z_
#define _In_z_
#endif

#ifndef _In_z_bytecount_
#define _In_z_bytecount_(size)
#endif

#ifndef _In_z_bytecount_c_
#define _In_z_bytecount_c_(size)
#endif

#ifndef _In_z_count_
#define _In_z_count_(size)
#endif

#ifndef _In_z_count_c_
#define _In_z_count_c_(size)
#endif

#ifndef _Inout_
#define _Inout_
#endif

#ifndef _Inout_bytecap_
#define _Inout_bytecap_(size)
#endif

#ifndef _Inout_bytecap_c_
#define _Inout_bytecap_c_(size)
#endif

#ifndef _Inout_bytecap_x_
#define _Inout_bytecap_x_(size)
#endif

#ifndef _Inout_bytecount_
#define _Inout_bytecount_(size)
#endif

#ifndef _Inout_bytecount_c_
#define _Inout_bytecount_c_(size)
#endif

#ifndef _Inout_bytecount_x_
#define _Inout_bytecount_x_(size)
#endif

#ifndef _Inout_cap_
#define _Inout_cap_(size)
#endif

#ifndef _Inout_cap_c_
#define _Inout_cap_c_(size)
#endif

#ifndef _Inout_cap_x_
#define _Inout_cap_x_(size)
#endif

#ifndef _Inout_count_
#define _Inout_count_(size)
#endif

#ifndef _Inout_count_c_
#define _Inout_count_c_(size)
#endif

#ifndef _Inout_count_x_
#define _Inout_count_x_(size)
#endif

#ifndef _Inout_defensive_
#define _Inout_defensive_(annotes)
#endif

#ifndef _Inout_opt_
#define _Inout_opt_
#endif

#ifndef _Inout_opt_bytecap_
#define _Inout_opt_bytecap_(size)
#endif

#ifndef _Inout_opt_bytecap_c_
#define _Inout_opt_bytecap_c_(size)
#endif

#ifndef _Inout_opt_bytecap_x_
#define _Inout_opt_bytecap_x_(size)
#endif

#ifndef _Inout_opt_bytecount_
#define _Inout_opt_bytecount_(size)
#endif

#ifndef _Inout_opt_bytecount_c_
#define _Inout_opt_bytecount_c_(size)
#endif

#ifndef _Inout_opt_bytecount_x_
#define _Inout_opt_bytecount_x_(size)
#endif

#ifndef _Inout_opt_cap_
#define _Inout_opt_cap_(size)
#endif

#ifndef _Inout_opt_cap_c_
#define _Inout_opt_cap_c_(size)
#endif

#ifndef _Inout_opt_cap_x_
#define _Inout_opt_cap_x_(size)
#endif

#ifndef _Inout_opt_count_
#define _Inout_opt_count_(size)
#endif

#ifndef _Inout_opt_count_c_
#define _Inout_opt_count_c_(size)
#endif

#ifndef _Inout_opt_count_x_
#define _Inout_opt_count_x_(size)
#endif

#ifndef _Inout_opt_ptrdiff_count_
#define _Inout_opt_ptrdiff_count_(size)
#endif

#ifndef _Inout_opt_z_
#define _Inout_opt_z_
#endif

#ifndef _Inout_opt_z_bytecap_
#define _Inout_opt_z_bytecap_(size)
#endif

#ifndef _Inout_opt_z_bytecap_c_
#define _Inout_opt_z_bytecap_c_(size)
#endif

#ifndef _Inout_opt_z_bytecap_x_
#define _Inout_opt_z_bytecap_x_(size)
#endif

#ifndef _Inout_opt_z_bytecount_
#define _Inout_opt_z_bytecount_(size)
#endif

#ifndef _Inout_opt_z_bytecount_c_
#define _Inout_opt_z_bytecount_c_(size)
#endif

#ifndef _Inout_opt_z_cap_
#define _Inout_opt_z_cap_(size)
#endif

#ifndef _Inout_opt_z_cap_c_
#define _Inout_opt_z_cap_c_(size)
#endif

#ifndef _Inout_opt_z_cap_x_
#define _Inout_opt_z_cap_x_(size)
#endif

#ifndef _Inout_opt_z_count_
#define _Inout_opt_z_count_(size)
#endif

#ifndef _Inout_opt_z_count_c_
#define _Inout_opt_z_count_c_(size)
#endif

#ifndef _Inout_ptrdiff_count_
#define _Inout_ptrdiff_count_(size)
#endif

#ifndef _Inout_updates_
#define _Inout_updates_(size)
#endif

#ifndef _Inout_updates_all_
#define _Inout_updates_all_(size)
#endif

#ifndef _Inout_updates_all_opt_
#define _Inout_updates_all_opt_(size)
#endif

#ifndef _Inout_updates_bytes_
#define _Inout_updates_bytes_(size)
#endif

#ifndef _Inout_updates_bytes_all_
#define _Inout_updates_bytes_all_(size)
#endif

#ifndef _Inout_updates_bytes_all_opt_
#define _Inout_updates_bytes_all_opt_(size)
#endif

#ifndef _Inout_updates_bytes_opt_
#define _Inout_updates_bytes_opt_(size)
#endif

#ifndef _Inout_updates_bytes_to_
#define _Inout_updates_bytes_to_(size,count)
#endif

#ifndef _Inout_updates_bytes_to_opt_
#define _Inout_updates_bytes_to_opt_(size,count)
#endif

#ifndef _Inout_updates_opt_
#define _Inout_updates_opt_(size)
#endif

#ifndef _Inout_updates_opt_z_
#define _Inout_updates_opt_z_(size)
#endif

#ifndef _Inout_updates_to_
#define _Inout_updates_to_(size,count)
#endif

#ifndef _Inout_updates_to_opt_
#define _Inout_updates_to_opt_(size,count)
#endif

#ifndef _Inout_updates_z_
#define _Inout_updates_z_(size)
#endif

#ifndef _Inout_z_
#define _Inout_z_
#endif

#ifndef _Inout_z_bytecap_
#define _Inout_z_bytecap_(size)
#endif

#ifndef _Inout_z_bytecap_c_
#define _Inout_z_bytecap_c_(size)
#endif

#ifndef _Inout_z_bytecap_x_
#define _Inout_z_bytecap_x_(size)
#endif

#ifndef _Inout_z_bytecount_
#define _Inout_z_bytecount_(size)
#endif

#ifndef _Inout_z_bytecount_c_
#define _Inout_z_bytecount_c_(size)
#endif

#ifndef _Inout_z_cap_
#define _Inout_z_cap_(size)
#endif

#ifndef _Inout_z_cap_c_
#define _Inout_z_cap_c_(size)
#endif

#ifndef _Inout_z_cap_x_
#define _Inout_z_cap_x_(size)
#endif

#ifndef _Inout_z_count_
#define _Inout_z_count_(size)
#endif

#ifndef _Inout_z_count_c_
#define _Inout_z_count_c_(size)
#endif

#ifndef _Interlocked_operand_
#define _Interlocked_operand_
#endif

#ifndef _Literal_
#define _Literal_
#endif

#ifndef _Maybe_raises_SEH_exception
#define _Maybe_raises_SEH_exception
#endif

#ifndef _Maybe_raises_SEH_exception_
#define _Maybe_raises_SEH_exception_
#endif

#ifndef _Maybenull_
#define _Maybenull_
#endif

#ifndef _Maybevalid_
#define _Maybevalid_
#endif

#ifndef _Must_inspect_result_
#define _Must_inspect_result_
#endif

#ifndef _Notliteral_
#define _Notliteral_
#endif

#ifndef _Notnull_
#define _Notnull_
#endif

#ifndef _Notref_
#define _Notref_
#endif

#ifndef _Notvalid_
#define _Notvalid_
#endif

#ifndef _NullNull_terminated_
#define _NullNull_terminated_
#endif

#ifndef _Null_
#define _Null_
#endif

#ifndef _Null_terminated_
#define _Null_terminated_
#endif

#ifndef _On_failure_
#define _On_failure_(annos)
#endif

#ifndef _Out_
#define _Out_
#endif

#ifndef _Out_bound_
#define _Out_bound_
#endif

#ifndef _Out_bytecap_
#define _Out_bytecap_(size)
#endif

#ifndef _Out_bytecap_c_
#define _Out_bytecap_c_(size)
#endif

#ifndef _Out_bytecap_post_bytecount_
#define _Out_bytecap_post_bytecount_(cap,count)
#endif

#ifndef _Out_bytecap_x_
#define _Out_bytecap_x_(size)
#endif

#ifndef _Out_bytecapcount_
#define _Out_bytecapcount_(capcount)
#endif

#ifndef _Out_bytecapcount_x_
#define _Out_bytecapcount_x_(capcount)
#endif

#ifndef _Out_cap_
#define _Out_cap_(size)
#endif

#ifndef _Out_cap_c_
#define _Out_cap_c_(size)
#endif

#ifndef _Out_cap_m_
#define _Out_cap_m_(mult,size)
#endif

#ifndef _Out_cap_post_count_
#define _Out_cap_post_count_(cap,count)
#endif

#ifndef _Out_cap_x_
#define _Out_cap_x_(size)
#endif

#ifndef _Out_capcount_
#define _Out_capcount_(capcount)
#endif

#ifndef _Out_capcount_x_
#define _Out_capcount_x_(capcount)
#endif

#ifndef _Out_defensive_
#define _Out_defensive_(annotes)
#endif

#ifndef _Out_opt_
#define _Out_opt_
#endif

#ifndef _Out_opt_bytecap_
#define _Out_opt_bytecap_(size)
#endif

#ifndef _Out_opt_bytecap_c_
#define _Out_opt_bytecap_c_(size)
#endif

#ifndef _Out_opt_bytecap_post_bytecount_
#define _Out_opt_bytecap_post_bytecount_(cap,count)
#endif

#ifndef _Out_opt_bytecap_x_
#define _Out_opt_bytecap_x_(size)
#endif

#ifndef _Out_opt_bytecapcount_
#define _Out_opt_bytecapcount_(capcount)
#endif

#ifndef _Out_opt_bytecapcount_x_
#define _Out_opt_bytecapcount_x_(capcount)
#endif

#ifndef _Out_opt_cap_
#define _Out_opt_cap_(size)
#endif

#ifndef _Out_opt_cap_c_
#define _Out_opt_cap_c_(size)
#endif

#ifndef _Out_opt_cap_m_
#define _Out_opt_cap_m_(mult,size)
#endif

#ifndef _Out_opt_cap_post_count_
#define _Out_opt_cap_post_count_(cap,count)
#endif

#ifndef _Out_opt_cap_x_
#define _Out_opt_cap_x_(size)
#endif

#ifndef _Out_opt_capcount_
#define _Out_opt_capcount_(capcount)
#endif

#ifndef _Out_opt_capcount_x_
#define _Out_opt_capcount_x_(capcount)
#endif

#ifndef _Out_opt_ptrdiff_cap_
#define _Out_opt_ptrdiff_cap_(size)
#endif

#ifndef _Out_opt_z_bytecap_
#define _Out_opt_z_bytecap_(size)
#endif

#ifndef _Out_opt_z_bytecap_c_
#define _Out_opt_z_bytecap_c_(size)
#endif

#ifndef _Out_opt_z_bytecap_post_bytecount_
#define _Out_opt_z_bytecap_post_bytecount_(cap,count)
#endif

#ifndef _Out_opt_z_bytecap_x_
#define _Out_opt_z_bytecap_x_(size)
#endif

#ifndef _Out_opt_z_bytecapcount_
#define _Out_opt_z_bytecapcount_(capcount)
#endif

#ifndef _Out_opt_z_cap_
#define _Out_opt_z_cap_(size)
#endif

#ifndef _Out_opt_z_cap_c_
#define _Out_opt_z_cap_c_(size)
#endif

#ifndef _Out_opt_z_cap_m_
#define _Out_opt_z_cap_m_(mult,size)
#endif

#ifndef _Out_opt_z_cap_post_count_
#define _Out_opt_z_cap_post_count_(cap,count)
#endif

#ifndef _Out_opt_z_cap_x_
#define _Out_opt_z_cap_x_(size)
#endif

#ifndef _Out_opt_z_capcount_
#define _Out_opt_z_capcount_(capcount)
#endif

#ifndef _Out_ptrdiff_cap_
#define _Out_ptrdiff_cap_(size)
#endif

#ifndef _Out_range_
#define _Out_range_(lb,ub)
#endif

#ifndef _Out_writes_
#define _Out_writes_(size)
#endif

#ifndef _Out_writes_all_
#define _Out_writes_all_(size)
#endif

#ifndef _Out_writes_all_opt_
#define _Out_writes_all_opt_(size)
#endif

#ifndef _Out_writes_bytes_
#define _Out_writes_bytes_(size)
#endif

#ifndef _Out_writes_bytes_all_
#define _Out_writes_bytes_all_(size)
#endif

#ifndef _Out_writes_bytes_all_opt_
#define _Out_writes_bytes_all_opt_(size)
#endif

#ifndef _Out_writes_bytes_opt_
#define _Out_writes_bytes_opt_(size)
#endif

#ifndef _Out_writes_bytes_to_
#define _Out_writes_bytes_to_(size,count)
#endif

#ifndef _Out_writes_bytes_to_opt_
#define _Out_writes_bytes_to_opt_(size,count)
#endif

#ifndef _Out_writes_opt_
#define _Out_writes_opt_(size)
#endif

#ifndef _Out_writes_opt_z_
#define _Out_writes_opt_z_(size)
#endif

#ifndef _Out_writes_to_
#define _Out_writes_to_(size,count)
#endif

#ifndef _Out_writes_to_opt_
#define _Out_writes_to_opt_(size,count)
#endif

#ifndef _Out_writes_to_ptr_
#define _Out_writes_to_ptr_(ptr)
#endif

#ifndef _Out_writes_to_ptr_opt_
#define _Out_writes_to_ptr_opt_(ptr)
#endif

#ifndef _Out_writes_to_ptr_opt_z_
#define _Out_writes_to_ptr_opt_z_(ptr)
#endif

#ifndef _Out_writes_to_ptr_z_
#define _Out_writes_to_ptr_z_(ptr)
#endif

#ifndef _Out_writes_z_
#define _Out_writes_z_(size)
#endif

#ifndef _Out_z_bytecap_
#define _Out_z_bytecap_(size)
#endif

#ifndef _Out_z_bytecap_c_
#define _Out_z_bytecap_c_(size)
#endif

#ifndef _Out_z_bytecap_post_bytecount_
#define _Out_z_bytecap_post_bytecount_(cap,count)
#endif

#ifndef _Out_z_bytecap_x_
#define _Out_z_bytecap_x_(size)
#endif

#ifndef _Out_z_bytecapcount_
#define _Out_z_bytecapcount_(capcount)
#endif

#ifndef _Out_z_cap_
#define _Out_z_cap_(size)
#endif

#ifndef _Out_z_cap_c_
#define _Out_z_cap_c_(size)
#endif

#ifndef _Out_z_cap_m_
#define _Out_z_cap_m_(mult,size)
#endif

#ifndef _Out_z_cap_post_count_
#define _Out_z_cap_post_count_(cap,count)
#endif

#ifndef _Out_z_cap_x_
#define _Out_z_cap_x_(size)
#endif

#ifndef _Out_z_capcount_
#define _Out_z_capcount_(capcount)
#endif

#ifndef _Outptr_
#define _Outptr_
#endif

#ifndef _Outptr_opt_
#define _Outptr_opt_
#endif

#ifndef _Outptr_opt_result_buffer_
#define _Outptr_opt_result_buffer_(size)
#endif

#ifndef _Outptr_opt_result_buffer_all_
#define _Outptr_opt_result_buffer_all_(size)
#endif

#ifndef _Outptr_opt_result_buffer_all_maybenull_
#define _Outptr_opt_result_buffer_all_maybenull_(size)
#endif

#ifndef _Outptr_opt_result_buffer_maybenull_
#define _Outptr_opt_result_buffer_maybenull_(size)
#endif

#ifndef _Outptr_opt_result_buffer_to_
#define _Outptr_opt_result_buffer_to_(size, count)
#endif

#ifndef _Outptr_opt_result_buffer_to_maybenull_
#define _Outptr_opt_result_buffer_to_maybenull_(size, count)
#endif

#ifndef _Outptr_opt_result_bytebuffer_
#define _Outptr_opt_result_bytebuffer_(size)
#endif

#ifndef _Outptr_opt_result_bytebuffer_all_
#define _Outptr_opt_result_bytebuffer_all_(size)
#endif

#ifndef _Outptr_opt_result_bytebuffer_all_maybenull_
#define _Outptr_opt_result_bytebuffer_all_maybenull_(size)
#endif

#ifndef _Outptr_opt_result_bytebuffer_maybenull_
#define _Outptr_opt_result_bytebuffer_maybenull_(size)
#endif

#ifndef _Outptr_opt_result_bytebuffer_to_
#define _Outptr_opt_result_bytebuffer_to_(size, count)
#endif

#ifndef _Outptr_opt_result_bytebuffer_to_maybenull_
#define _Outptr_opt_result_bytebuffer_to_maybenull_(size, count)
#endif

#ifndef _Outptr_opt_result_maybenull_
#define _Outptr_opt_result_maybenull_
#endif

#ifndef _Outptr_opt_result_maybenull_z_
#define _Outptr_opt_result_maybenull_z_
#endif

#ifndef _Outptr_opt_result_nullonfailure_
#define _Outptr_opt_result_nullonfailure_
#endif

#ifndef _Outptr_opt_result_z_
#define _Outptr_opt_result_z_
#endif

#ifndef _Outptr_result_buffer_
#define _Outptr_result_buffer_(size)
#endif

#ifndef _Outptr_result_buffer_all_
#define _Outptr_result_buffer_all_(size)
#endif

#ifndef _Outptr_result_buffer_all_maybenull_
#define _Outptr_result_buffer_all_maybenull_(size)
#endif

#ifndef _Outptr_result_buffer_maybenull_
#define _Outptr_result_buffer_maybenull_(size)
#endif

#ifndef _Outptr_result_buffer_to_
#define _Outptr_result_buffer_to_(size, count)
#endif

#ifndef _Outptr_result_buffer_to_maybenull_
#define _Outptr_result_buffer_to_maybenull_(size, count)
#endif

#ifndef _Outptr_result_bytebuffer_
#define _Outptr_result_bytebuffer_(size)
#endif

#ifndef _Outptr_result_bytebuffer_all_
#define _Outptr_result_bytebuffer_all_(size)
#endif

#ifndef _Outptr_result_bytebuffer_all_maybenull_
#define _Outptr_result_bytebuffer_all_maybenull_(size)
#endif

#ifndef _Outptr_result_bytebuffer_maybenull_
#define _Outptr_result_bytebuffer_maybenull_(size)
#endif

#ifndef _Outptr_result_bytebuffer_to_
#define _Outptr_result_bytebuffer_to_(size, count)
#endif

#ifndef _Outptr_result_bytebuffer_to_maybenull_
#define _Outptr_result_bytebuffer_to_maybenull_(size, count)
#endif

#ifndef _Outptr_result_maybenull_
#define _Outptr_result_maybenull_
#endif

#ifndef _Outptr_result_maybenull_z_
#define _Outptr_result_maybenull_z_
#endif

#ifndef _Outptr_result_nullonfailure_
#define _Outptr_result_nullonfailure_
#endif

#ifndef _Outptr_result_z_
#define _Outptr_result_z_
#endif

#ifndef _Outref_
#define _Outref_
#endif

#ifndef _Outref_result_buffer_
#define _Outref_result_buffer_(size)
#endif

#ifndef _Outref_result_buffer_all_
#define _Outref_result_buffer_all_(size)
#endif

#ifndef _Outref_result_buffer_all_maybenull_
#define _Outref_result_buffer_all_maybenull_(size)
#endif

#ifndef _Outref_result_buffer_maybenull_
#define _Outref_result_buffer_maybenull_(size)
#endif

#ifndef _Outref_result_buffer_to_
#define _Outref_result_buffer_to_(size, count)
#endif

#ifndef _Outref_result_buffer_to_maybenull_
#define _Outref_result_buffer_to_maybenull_(size, count)
#endif

#ifndef _Outref_result_bytebuffer_
#define _Outref_result_bytebuffer_(size)
#endif

#ifndef _Outref_result_bytebuffer_all_
#define _Outref_result_bytebuffer_all_(size)
#endif

#ifndef _Outref_result_bytebuffer_all_maybenull_
#define _Outref_result_bytebuffer_all_maybenull_(size)
#endif

#ifndef _Outref_result_bytebuffer_maybenull_
#define _Outref_result_bytebuffer_maybenull_(size)
#endif

#ifndef _Outref_result_bytebuffer_to_
#define _Outref_result_bytebuffer_to_(size, count)
#endif

#ifndef _Outref_result_bytebuffer_to_maybenull_
#define _Outref_result_bytebuffer_to_maybenull_(size, count)
#endif

#ifndef _Outref_result_maybenull_
#define _Outref_result_maybenull_
#endif

#ifndef _Outref_result_nullonfailure_
#define _Outref_result_nullonfailure_
#endif

#ifndef _Points_to_data_
#define _Points_to_data_
#endif

#ifndef _Post_
#define _Post_
#endif

#ifndef _Post_bytecap_
#define _Post_bytecap_(size)
#endif

#ifndef _Post_bytecount_
#define _Post_bytecount_(size)
#endif

#ifndef _Post_bytecount_c_
#define _Post_bytecount_c_(size)
#endif

#ifndef _Post_bytecount_x_
#define _Post_bytecount_x_(size)
#endif

#ifndef _Post_cap_
#define _Post_cap_(size)
#endif

#ifndef _Post_count_
#define _Post_count_(size)
#endif

#ifndef _Post_count_c_
#define _Post_count_c_(size)
#endif

#ifndef _Post_count_x_
#define _Post_count_x_(size)
#endif

#ifndef _Post_defensive_
#define _Post_defensive_
#endif

#ifndef _Post_equal_to_
#define _Post_equal_to_(expr)
#endif

#ifndef _Post_invalid_
#define _Post_invalid_
#endif

#ifndef _Post_maybenull_
#define _Post_maybenull_
#endif

#ifndef _Post_maybez_
#define _Post_maybez_
#endif

#ifndef _Post_notnull_
#define _Post_notnull_
#endif

#ifndef _Post_null_
#define _Post_null_
#endif

#ifndef _Post_ptr_invalid_
#define _Post_ptr_invalid_
#endif

#ifndef _Post_readable_byte_size_
#define _Post_readable_byte_size_(size)
#endif

#ifndef _Post_readable_size_
#define _Post_readable_size_(size)
#endif

#ifndef _Post_satisfies_
#define _Post_satisfies_(cond)
#endif

#ifndef _Post_valid_
#define _Post_valid_
#endif

#ifndef _Post_writable_byte_size_
#define _Post_writable_byte_size_(size)
#endif

#ifndef _Post_writable_size_
#define _Post_writable_size_(size)
#endif

#ifndef _Post_z_
#define _Post_z_
#endif

#ifndef _Post_z_bytecount_
#define _Post_z_bytecount_(size)
#endif

#ifndef _Post_z_bytecount_c_
#define _Post_z_bytecount_c_(size)
#endif

#ifndef _Post_z_bytecount_x_
#define _Post_z_bytecount_x_(size)
#endif

#ifndef _Post_z_count_
#define _Post_z_count_(size)
#endif

#ifndef _Post_z_count_c_
#define _Post_z_count_c_(size)
#endif

#ifndef _Post_z_count_x_
#define _Post_z_count_x_(size)
#endif

#ifndef _Pre_
#define _Pre_
#endif

#ifndef _Pre_bytecap_
#define _Pre_bytecap_(size)
#endif

#ifndef _Pre_bytecap_c_
#define _Pre_bytecap_c_(size)
#endif

#ifndef _Pre_bytecap_x_
#define _Pre_bytecap_x_(size)
#endif

#ifndef _Pre_bytecount_
#define _Pre_bytecount_(size)
#endif

#ifndef _Pre_bytecount_c_
#define _Pre_bytecount_c_(size)
#endif

#ifndef _Pre_bytecount_x_
#define _Pre_bytecount_x_(size)
#endif

#ifndef _Pre_cap_
#define _Pre_cap_(size)
#endif

#ifndef _Pre_cap_c_
#define _Pre_cap_c_(size)
#endif

#ifndef _Pre_cap_c_one_
#define _Pre_cap_c_one_
#endif

#ifndef _Pre_cap_for_
#define _Pre_cap_for_(param)
#endif

#ifndef _Pre_cap_m_
#define _Pre_cap_m_(mult,size)
#endif

#ifndef _Pre_cap_x_
#define _Pre_cap_x_(size)
#endif

#ifndef _Pre_count_
#define _Pre_count_(size)
#endif

#ifndef _Pre_count_c_
#define _Pre_count_c_(size)
#endif

#ifndef _Pre_count_x_
#define _Pre_count_x_(size)
#endif

#ifndef _Pre_defensive_
#define _Pre_defensive_
#endif

#ifndef _Pre_equal_to_
#define _Pre_equal_to_(expr)
#endif

#ifndef _Pre_invalid_
#define _Pre_invalid_
#endif

#ifndef _Pre_maybenull_
#define _Pre_maybenull_
#endif

#ifndef _Pre_notnull_
#define _Pre_notnull_
#endif

#ifndef _Pre_null_
#define _Pre_null_
#endif

#ifndef _Pre_opt_bytecap_
#define _Pre_opt_bytecap_(size)
#endif

#ifndef _Pre_opt_bytecap_c_
#define _Pre_opt_bytecap_c_(size)
#endif

#ifndef _Pre_opt_bytecap_x_
#define _Pre_opt_bytecap_x_(size)
#endif

#ifndef _Pre_opt_bytecount_
#define _Pre_opt_bytecount_(size)
#endif

#ifndef _Pre_opt_bytecount_c_
#define _Pre_opt_bytecount_c_(size)
#endif

#ifndef _Pre_opt_bytecount_x_
#define _Pre_opt_bytecount_x_(size)
#endif

#ifndef _Pre_opt_cap_
#define _Pre_opt_cap_(size)
#endif

#ifndef _Pre_opt_cap_c_
#define _Pre_opt_cap_c_(size)
#endif

#ifndef _Pre_opt_cap_c_one_
#define _Pre_opt_cap_c_one_
#endif

#ifndef _Pre_opt_cap_for_
#define _Pre_opt_cap_for_(param)
#endif

#ifndef _Pre_opt_cap_m_
#define _Pre_opt_cap_m_(mult,size)
#endif

#ifndef _Pre_opt_cap_x_
#define _Pre_opt_cap_x_(size)
#endif

#ifndef _Pre_opt_count_
#define _Pre_opt_count_(size)
#endif

#ifndef _Pre_opt_count_c_
#define _Pre_opt_count_c_(size)
#endif

#ifndef _Pre_opt_count_x_
#define _Pre_opt_count_x_(size)
#endif

#ifndef _Pre_opt_ptrdiff_cap_
#define _Pre_opt_ptrdiff_cap_(ptr)
#endif

#ifndef _Pre_opt_ptrdiff_count_
#define _Pre_opt_ptrdiff_count_(ptr)
#endif

#ifndef _Pre_opt_valid_
#define _Pre_opt_valid_
#endif

#ifndef _Pre_opt_valid_bytecap_
#define _Pre_opt_valid_bytecap_(size)
#endif

#ifndef _Pre_opt_valid_bytecap_c_
#define _Pre_opt_valid_bytecap_c_(size)
#endif

#ifndef _Pre_opt_valid_bytecap_x_
#define _Pre_opt_valid_bytecap_x_(size)
#endif

#ifndef _Pre_opt_valid_cap_
#define _Pre_opt_valid_cap_(size)
#endif

#ifndef _Pre_opt_valid_cap_c_
#define _Pre_opt_valid_cap_c_(size)
#endif

#ifndef _Pre_opt_valid_cap_x_
#define _Pre_opt_valid_cap_x_(size)
#endif

#ifndef _Pre_opt_z_
#define _Pre_opt_z_
#endif

#ifndef _Pre_opt_z_bytecap_
#define _Pre_opt_z_bytecap_(size)
#endif

#ifndef _Pre_opt_z_bytecap_c_
#define _Pre_opt_z_bytecap_c_(size)
#endif

#ifndef _Pre_opt_z_bytecap_x_
#define _Pre_opt_z_bytecap_x_(size)
#endif

#ifndef _Pre_opt_z_cap_
#define _Pre_opt_z_cap_(size)
#endif

#ifndef _Pre_opt_z_cap_c_
#define _Pre_opt_z_cap_c_(size)
#endif

#ifndef _Pre_opt_z_cap_x_
#define _Pre_opt_z_cap_x_(size)
#endif

#ifndef _Pre_ptrdiff_cap_
#define _Pre_ptrdiff_cap_(ptr)
#endif

#ifndef _Pre_ptrdiff_count_
#define _Pre_ptrdiff_count_(ptr)
#endif

#ifndef _Pre_readable_byte_size_
#define _Pre_readable_byte_size_(size)
#endif

#ifndef _Pre_readable_size_
#define _Pre_readable_size_(size)
#endif

#ifndef _Pre_readonly_
#define _Pre_readonly_
#endif

#ifndef _Pre_satisfies_
#define _Pre_satisfies_(cond)
#endif

#ifndef _Pre_valid_
#define _Pre_valid_
#endif

#ifndef _Pre_valid_bytecap_
#define _Pre_valid_bytecap_(size)
#endif

#ifndef _Pre_valid_bytecap_c_
#define _Pre_valid_bytecap_c_(size)
#endif

#ifndef _Pre_valid_bytecap_x_
#define _Pre_valid_bytecap_x_(size)
#endif

#ifndef _Pre_valid_cap_
#define _Pre_valid_cap_(size)
#endif

#ifndef _Pre_valid_cap_c_
#define _Pre_valid_cap_c_(size)
#endif

#ifndef _Pre_valid_cap_x_
#define _Pre_valid_cap_x_(size)
#endif

#ifndef _Pre_writable_byte_size_
#define _Pre_writable_byte_size_(size)
#endif

#ifndef _Pre_writable_size_
#define _Pre_writable_size_(size)
#endif

#ifndef _Pre_writeonly_
#define _Pre_writeonly_
#endif

#ifndef _Pre_z_
#define _Pre_z_
#endif

#ifndef _Pre_z_bytecap_
#define _Pre_z_bytecap_(size)
#endif

#ifndef _Pre_z_bytecap_c_
#define _Pre_z_bytecap_c_(size)
#endif

#ifndef _Pre_z_bytecap_x_
#define _Pre_z_bytecap_x_(size)
#endif

#ifndef _Pre_z_cap_
#define _Pre_z_cap_(size)
#endif

#ifndef _Pre_z_cap_c_
#define _Pre_z_cap_c_(size)
#endif

#ifndef _Pre_z_cap_x_
#define _Pre_z_cap_x_(size)
#endif

#ifndef _Prepost_bytecount_
#define _Prepost_bytecount_(size)
#endif

#ifndef _Prepost_bytecount_c_
#define _Prepost_bytecount_c_(size)
#endif

#ifndef _Prepost_bytecount_x_
#define _Prepost_bytecount_x_(size)
#endif

#ifndef _Prepost_count_
#define _Prepost_count_(size)
#endif

#ifndef _Prepost_count_c_
#define _Prepost_count_c_(size)
#endif

#ifndef _Prepost_count_x_
#define _Prepost_count_x_(size)
#endif

#ifndef _Prepost_opt_bytecount_
#define _Prepost_opt_bytecount_(size)
#endif

#ifndef _Prepost_opt_bytecount_c_
#define _Prepost_opt_bytecount_c_(size)
#endif

#ifndef _Prepost_opt_bytecount_x_
#define _Prepost_opt_bytecount_x_(size)
#endif

#ifndef _Prepost_opt_count_
#define _Prepost_opt_count_(size)
#endif

#ifndef _Prepost_opt_count_c_
#define _Prepost_opt_count_c_(size)
#endif

#ifndef _Prepost_opt_count_x_
#define _Prepost_opt_count_x_(size)
#endif

#ifndef _Prepost_opt_valid_
#define _Prepost_opt_valid_
#endif

#ifndef _Prepost_opt_z_
#define _Prepost_opt_z_
#endif

#ifndef _Prepost_valid_
#define _Prepost_valid_
#endif

#ifndef _Prepost_z_
#define _Prepost_z_
#endif

#ifndef _Printf_format_string_
#define _Printf_format_string_
#endif

#ifndef _Raises_SEH_exception_
#define _Raises_SEH_exception_
#endif

#ifndef _Readable_bytes_
#define _Readable_bytes_(size)
#endif

#ifndef _Readable_elements_
#define _Readable_elements_(size)
#endif

#ifndef _Reserved_
#define _Reserved_
#endif

#ifndef _Result_nullonfailure_
#define _Result_nullonfailure_
#endif

#ifndef _Result_zeroonfailure_
#define _Result_zeroonfailure_
#endif

#ifndef _Ret_
#define _Ret_
#endif

#ifndef _Ret_bound_
#define _Ret_bound_
#endif

#ifndef _Ret_bytecap_
#define _Ret_bytecap_(size)
#endif

#ifndef _Ret_bytecap_c_
#define _Ret_bytecap_c_(size)
#endif

#ifndef _Ret_bytecap_x_
#define _Ret_bytecap_x_(size)
#endif

#ifndef _Ret_bytecount_
#define _Ret_bytecount_(size)
#endif

#ifndef _Ret_bytecount_c_
#define _Ret_bytecount_c_(size)
#endif

#ifndef _Ret_bytecount_x_
#define _Ret_bytecount_x_(size)
#endif

#ifndef _Ret_cap_
#define _Ret_cap_(size)
#endif

#ifndef _Ret_cap_c_
#define _Ret_cap_c_(size)
#endif

#ifndef _Ret_cap_x_
#define _Ret_cap_x_(size)
#endif

#ifndef _Ret_count_
#define _Ret_count_(size)
#endif

#ifndef _Ret_count_c_
#define _Ret_count_c_(size)
#endif

#ifndef _Ret_count_x_
#define _Ret_count_x_(size)
#endif

#ifndef _Ret_maybenull_
#define _Ret_maybenull_
#endif

#ifndef _Ret_maybenull_z_
#define _Ret_maybenull_z_
#endif

#ifndef _Ret_notnull_
#define _Ret_notnull_
#endif

#ifndef _Ret_null_
#define _Ret_null_
#endif

#ifndef _Ret_opt_
#define _Ret_opt_
#endif

#ifndef _Ret_opt_bytecap_
#define _Ret_opt_bytecap_(size)
#endif

#ifndef _Ret_opt_bytecap_c_
#define _Ret_opt_bytecap_c_(size)
#endif

#ifndef _Ret_opt_bytecap_x_
#define _Ret_opt_bytecap_x_(size)
#endif

#ifndef _Ret_opt_bytecount_
#define _Ret_opt_bytecount_(size)
#endif

#ifndef _Ret_opt_bytecount_c_
#define _Ret_opt_bytecount_c_(size)
#endif

#ifndef _Ret_opt_bytecount_x_
#define _Ret_opt_bytecount_x_(size)
#endif

#ifndef _Ret_opt_cap_
#define _Ret_opt_cap_(size)
#endif

#ifndef _Ret_opt_cap_c_
#define _Ret_opt_cap_c_(size)
#endif

#ifndef _Ret_opt_cap_x_
#define _Ret_opt_cap_x_(size)
#endif

#ifndef _Ret_opt_count_
#define _Ret_opt_count_(size)
#endif

#ifndef _Ret_opt_count_c_
#define _Ret_opt_count_c_(size)
#endif

#ifndef _Ret_opt_count_x_
#define _Ret_opt_count_x_(size)
#endif

#ifndef _Ret_opt_valid_
#define _Ret_opt_valid_
#endif

#ifndef _Ret_opt_z_
#define _Ret_opt_z_
#endif

#ifndef _Ret_opt_z_bytecap_
#define _Ret_opt_z_bytecap_(size)
#endif

#ifndef _Ret_opt_z_bytecount_
#define _Ret_opt_z_bytecount_(size)
#endif

#ifndef _Ret_opt_z_cap_
#define _Ret_opt_z_cap_(size)
#endif

#ifndef _Ret_opt_z_count_
#define _Ret_opt_z_count_(size)
#endif

#ifndef _Ret_range_
#define _Ret_range_(lb,ub)
#endif

#ifndef _Ret_valid_
#define _Ret_valid_
#endif

#ifndef _Ret_writes_
#define _Ret_writes_(size)
#endif

#ifndef _Ret_writes_bytes_
#define _Ret_writes_bytes_(size)
#endif

#ifndef _Ret_writes_bytes_maybenull_
#define _Ret_writes_bytes_maybenull_(size)
#endif

#ifndef _Ret_writes_bytes_to_
#define _Ret_writes_bytes_to_(size,count)
#endif

#ifndef _Ret_writes_bytes_to_maybenull_
#define _Ret_writes_bytes_to_maybenull_(size,count)
#endif

#ifndef _Ret_writes_maybenull_
#define _Ret_writes_maybenull_(size)
#endif

#ifndef _Ret_writes_maybenull_z_
#define _Ret_writes_maybenull_z_(size)
#endif

#ifndef _Ret_writes_to_
#define _Ret_writes_to_(size,count)
#endif

#ifndef _Ret_writes_to_maybenull_
#define _Ret_writes_to_maybenull_(size,count)
#endif

#ifndef _Ret_writes_z_
#define _Ret_writes_z_(size)
#endif

#ifndef _Ret_z_
#define _Ret_z_
#endif

#ifndef _Ret_z_bytecap_
#define _Ret_z_bytecap_(size)
#endif

#ifndef _Ret_z_bytecount_
#define _Ret_z_bytecount_(size)
#endif

#ifndef _Ret_z_cap_
#define _Ret_z_cap_(size)
#endif

#ifndef _Ret_z_count_
#define _Ret_z_count_(size)
#endif

#ifndef _Return_type_success_
#define _Return_type_success_(expr)
#endif

#ifndef _Scanf_format_string_
#define _Scanf_format_string_
#endif

#ifndef _Scanf_s_format_string_
#define _Scanf_s_format_string_
#endif

#ifndef _Struct_size_bytes_
#define _Struct_size_bytes_(size)
#endif

#ifndef _Success_
#define _Success_(expr)
#endif

#ifndef _Unchanged_
#define _Unchanged_(e)
#endif

#ifndef _Use_decl_annotations_
#define _Use_decl_annotations_
#endif

#ifndef _Valid_
#define _Valid_
#endif

#ifndef _When_
#define _When_(expr, annos)
#endif

#ifndef _Writable_bytes_
#define _Writable_bytes_(size)
#endif

#ifndef _Writable_elements_
#define _Writable_elements_(size)
#endif

#ifndef __inner_callback
#define __inner_callback
#endif

#ifndef __inner_exceptthat
#define __inner_exceptthat
#endif

#ifndef __inner_typefix
#define __inner_typefix(ctype)
#endif



/*
 * Dummy header annotations.
 *
 * These are typically defined in specstrings.h.
 *
 * Unfortunately these defines tend to clash with libstdc++ internal symbols.  A solution is to ensure
 * that all needed libstdc++ headers are included before this one.
 *
 * See also:
 * - http://msdn.microsoft.com/en-us/library/aa383701.aspx
 * - http://svn.reactos.org/svn/reactos/trunk/reactos/include/psdk/specstrings.h?view=markup
 */

#ifndef __deref_out
#define __deref_out
#endif

#ifndef __deref_out_bcount
#define __deref_out_bcount(x)
#endif

#ifndef __deref_out_opt
#define __deref_out_opt
#endif

#ifndef __ecount
#define __ecount(x)
#endif

#ifndef __field_bcount
#define __field_bcount(x)
#endif

#ifndef __field_ecount
#define __field_ecount(x)
#endif

#ifndef __field_ecount_full
#define __field_ecount_full(x)
#endif

#ifndef __field_ecount_opt
#define __field_ecount_opt(x)
#endif

#ifndef __in
#define __in
#endif

#ifndef __in_bcount
#define __in_bcount(x)
#endif

#ifndef __in_bcount_opt
#define __in_bcount_opt(x)
#endif

#ifndef __in_ecount
#define __in_ecount(x)
#endif

#ifndef __in_ecount_opt
#define __in_ecount_opt(x)
#endif

#ifndef __in_opt
#define __in_opt
#endif

#ifndef __in_range
#define __in_range(x,y)
#endif

#ifndef __in_z
#define __in_z
#endif

#ifndef __in_z_opt
#define __in_z_opt
#endif

#ifndef __inout
#define __inout
#endif

#ifndef __inout_opt
#define __inout_opt
#endif

#ifndef __maybenull
#define __maybenull
#endif

#ifndef __notnull
#define __notnull
#endif

#ifndef __nullterminated
#define __nullterminated
#endif

#ifndef __out
#define __out
#endif

#ifndef __out_bcount
#define __out_bcount(x)
#endif

#ifndef __out_bcount_full_opt
#define __out_bcount_full_opt(x)
#endif

#ifndef __out_bcount_opt
#define __out_bcount_opt(x)
#endif

#ifndef __out_ecount
#define __out_ecount(x)
#endif

#ifndef __out_ecount_opt
#define __out_ecount_opt(x)
#endif

#ifndef __out_ecount_part_opt
#define __out_ecount_part_opt(x,y)
#endif

#ifndef __out_ecount_part_z_opt
#define __out_ecount_part_z_opt(x,y)
#endif

#ifndef __out_ecount_z
#define __out_ecount_z(x)
#endif

#ifndef __out_opt
#define __out_opt
#endif

#ifndef __out_z
#define __out_z
#endif

#ifndef __range
#define __range(x,y)
#endif

#ifndef __success
#define __success(x)
#endif



#ifndef __inline
#define __inline __inline__
#endif

#ifndef DECLSPEC_DEPRECATED
#define DECLSPEC_DEPRECATED
#endif

#ifndef DECLSPEC_NOVTABLE
#define DECLSPEC_NOVTABLE
#endif

#ifndef DECLSPEC_NOTHROW
#define DECLSPEC_NOTHROW
#endif

#ifndef COM_DECLSPEC_NOTHROW
#define COM_DECLSPEC_NOTHROW DECLSPEC_NOTHROW
#endif

#ifndef __MINGW64_VERSION_MAJOR
#define INT8 signed char
#define UINT8 unsigned char
#define INT16 signed short
#define UINT16 unsigned short
#endif


#ifndef _WIN32_WINNT_WINBLUE
#define _WIN32_WINNT_WINBLUE 0x0603
#endif

#ifndef _WIN32_WINNT_WINTHRESHOLD
#define _WIN32_WINNT_WINTHRESHOLD 0x0A00
#endif

#ifndef _WIN32_WINNT_WIN10
#define _WIN32_WINNT_WIN10 0x0A00
#endif

#ifndef NTDDI_WINBLUE
#define NTDDI_WINBLUE 0x06030000
#endif

#ifndef NTDDI_WINTHRESHOLD
#define NTDDI_WINTHRESHOLD 0x0A000000
#endif

#ifndef NTDDI_WIN10
#define NTDDI_WIN10 0x0A000000
#endif


/*
 * Direct3D error codes.
 *
 * These were defined in d3dx10.h/d3dx11.h on DirectX SDK, and moved to
 * winerror.h on Windows 8 SDK.
 *
 * See also:
 * - http://msdn.microsoft.com/en-us/library/windows/desktop/ff476174.aspx
 * - http://msdn.microsoft.com/en-us/library/windows/desktop/bb205278.aspx
 * - http://msdn.microsoft.com/en-us/library/windows/desktop/bb172554.aspx
 */

#ifndef DXGI_STATUS_OCCLUDED
#define DXGI_STATUS_OCCLUDED                                     0x087A0001L
#endif
#ifndef DXGI_STATUS_CLIPPED
#define DXGI_STATUS_CLIPPED                                      0x087A0002L
#endif
#ifndef DXGI_STATUS_NO_REDIRECTION
#define DXGI_STATUS_NO_REDIRECTION                               0x087A0004L
#endif
#ifndef DXGI_STATUS_NO_DESKTOP_ACCESS
#define DXGI_STATUS_NO_DESKTOP_ACCESS                            0x087A0005L
#endif
#ifndef DXGI_STATUS_GRAPHICS_VIDPN_SOURCE_IN_USE
#define DXGI_STATUS_GRAPHICS_VIDPN_SOURCE_IN_USE                 0x087A0006L
#endif
#ifndef DXGI_STATUS_MODE_CHANGED
#define DXGI_STATUS_MODE_CHANGED                                 0x087A0007L
#endif
#ifndef DXGI_STATUS_MODE_CHANGE_IN_PROGRESS
#define DXGI_STATUS_MODE_CHANGE_IN_PROGRESS                      0x087A0008L
#endif
#ifndef DXGI_ERROR_INVALID_CALL
#define DXGI_ERROR_INVALID_CALL                                  0x887A0001L
#endif
#ifndef DXGI_ERROR_NOT_FOUND
#define DXGI_ERROR_NOT_FOUND                                     0x887A0002L
#endif
#ifndef DXGI_ERROR_MORE_DATA
#define DXGI_ERROR_MORE_DATA                                     0x887A0003L
#endif
#ifndef DXGI_ERROR_UNSUPPORTED
#define DXGI_ERROR_UNSUPPORTED                                   0x887A0004L
#endif
#ifndef DXGI_ERROR_DEVICE_REMOVED
#define DXGI_ERROR_DEVICE_REMOVED                                0x887A0005L
#endif
#ifndef DXGI_ERROR_DEVICE_HUNG
#define DXGI_ERROR_DEVICE_HUNG                                   0x887A0006L
#endif
#ifndef DXGI_ERROR_DEVICE_RESET
#define DXGI_ERROR_DEVICE_RESET                                  0x887A0007L
#endif
#ifndef DXGI_ERROR_WAS_STILL_DRAWING
#define DXGI_ERROR_WAS_STILL_DRAWING                             0x887A000AL
#endif
#ifndef DXGI_ERROR_FRAME_STATISTICS_DISJOINT
#define DXGI_ERROR_FRAME_STATISTICS_DISJOINT                     0x887A000BL
#endif
#ifndef DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE
#define DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE                  0x887A000CL
#endif
#ifndef DXGI_ERROR_DRIVER_INTERNAL_ERROR
#define DXGI_ERROR_DRIVER_INTERNAL_ERROR                         0x887A0020L
#endif
#ifndef DXGI_ERROR_NONEXCLUSIVE
#define DXGI_ERROR_NONEXCLUSIVE                                  0x887A0021L
#endif
#ifndef DXGI_ERROR_NOT_CURRENTLY_AVAILABLE
#define DXGI_ERROR_NOT_CURRENTLY_AVAILABLE                       0x887A0022L
#endif
#ifndef DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED
#define DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED                    0x887A0023L
#endif
#ifndef DXGI_ERROR_REMOTE_OUTOFMEMORY
#define DXGI_ERROR_REMOTE_OUTOFMEMORY                            0x887A0024L
#endif
#ifndef DXGI_ERROR_ACCESS_LOST
#define DXGI_ERROR_ACCESS_LOST                                   0x887A0026L
#endif
#ifndef DXGI_ERROR_WAIT_TIMEOUT
#define DXGI_ERROR_WAIT_TIMEOUT                                  0x887A0027L
#endif
#ifndef DXGI_ERROR_SESSION_DISCONNECTED
#define DXGI_ERROR_SESSION_DISCONNECTED                          0x887A0028L
#endif
#ifndef DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE
#define DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE                      0x887A0029L
#endif
#ifndef DXGI_ERROR_CANNOT_PROTECT_CONTENT
#define DXGI_ERROR_CANNOT_PROTECT_CONTENT                        0x887A002AL
#endif
#ifndef DXGI_ERROR_ACCESS_DENIED
#define DXGI_ERROR_ACCESS_DENIED                                 0x887A002BL
#endif
#ifndef DXGI_ERROR_NAME_ALREADY_EXISTS
#define DXGI_ERROR_NAME_ALREADY_EXISTS                           0x887A002CL
#endif
#ifndef DXGI_ERROR_SDK_COMPONENT_MISSING
#define DXGI_ERROR_SDK_COMPONENT_MISSING                         0x887A002DL
#endif
#ifndef DXGI_ERROR_NOT_CURRENT
#define DXGI_ERROR_NOT_CURRENT                                   0x887A002EL
#endif
#ifndef DXGI_ERROR_HW_PROTECTION_OUTOFMEMORY
#define DXGI_ERROR_HW_PROTECTION_OUTOFMEMORY                     0x887A0030L
#endif
#ifndef DXGI_STATUS_UNOCCLUDED
#define DXGI_STATUS_UNOCCLUDED                                   0x087A0009L
#endif
#ifndef DXGI_STATUS_DDA_WAS_STILL_DRAWING
#define DXGI_STATUS_DDA_WAS_STILL_DRAWING                        0x087A000AL
#endif
#ifndef DXGI_ERROR_MODE_CHANGE_IN_PROGRESS
#define DXGI_ERROR_MODE_CHANGE_IN_PROGRESS                       0x887A0025L
#endif
#ifndef DXGI_STATUS_PRESENT_REQUIRED
#define DXGI_STATUS_PRESENT_REQUIRED                             0x087A002FL
#endif
#ifndef DXGI_DDI_ERR_WASSTILLDRAWING
#define DXGI_DDI_ERR_WASSTILLDRAWING                             0x887B0001L
#endif
#ifndef DXGI_DDI_ERR_UNSUPPORTED
#define DXGI_DDI_ERR_UNSUPPORTED                                 0x887B0002L
#endif
#ifndef DXGI_DDI_ERR_NONEXCLUSIVE
#define DXGI_DDI_ERR_NONEXCLUSIVE                                0x887B0003L
#endif
#ifndef D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS
#define D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS                0x88790001L
#endif
#ifndef D3D10_ERROR_FILE_NOT_FOUND
#define D3D10_ERROR_FILE_NOT_FOUND                               0x88790002L
#endif
#ifndef D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS
#define D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS                0x887C0001L
#endif
#ifndef D3D11_ERROR_FILE_NOT_FOUND
#define D3D11_ERROR_FILE_NOT_FOUND                               0x887C0002L
#endif
#ifndef D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS
#define D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS                 0x887C0003L
#endif
#ifndef D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD
#define D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD 0x887C0004L
#endif
#ifndef D3D12_ERROR_ADAPTER_NOT_FOUND
#define D3D12_ERROR_ADAPTER_NOT_FOUND                            0x887E0001L
#endif
#ifndef D3D12_ERROR_DRIVER_VERSION_MISMATCH
#define D3D12_ERROR_DRIVER_VERSION_MISMATCH                      0x887E0002L
#endif
#ifndef D2DERR_WRONG_STATE
#define D2DERR_WRONG_STATE                                       0x88990001L
#endif
#ifndef D2DERR_NOT_INITIALIZED
#define D2DERR_NOT_INITIALIZED                                   0x88990002L
#endif
#ifndef D2DERR_UNSUPPORTED_OPERATION
#define D2DERR_UNSUPPORTED_OPERATION                             0x88990003L
#endif
#ifndef D2DERR_SCANNER_FAILED
#define D2DERR_SCANNER_FAILED                                    0x88990004L
#endif
#ifndef D2DERR_SCREEN_ACCESS_DENIED
#define D2DERR_SCREEN_ACCESS_DENIED                              0x88990005L
#endif
#ifndef D2DERR_DISPLAY_STATE_INVALID
#define D2DERR_DISPLAY_STATE_INVALID                             0x88990006L
#endif
#ifndef D2DERR_ZERO_VECTOR
#define D2DERR_ZERO_VECTOR                                       0x88990007L
#endif
#ifndef D2DERR_INTERNAL_ERROR
#define D2DERR_INTERNAL_ERROR                                    0x88990008L
#endif
#ifndef D2DERR_DISPLAY_FORMAT_NOT_SUPPORTED
#define D2DERR_DISPLAY_FORMAT_NOT_SUPPORTED                      0x88990009L
#endif
#ifndef D2DERR_INVALID_CALL
#define D2DERR_INVALID_CALL                                      0x8899000AL
#endif
#ifndef D2DERR_NO_HARDWARE_DEVICE
#define D2DERR_NO_HARDWARE_DEVICE                                0x8899000BL
#endif
#ifndef D2DERR_RECREATE_TARGET
#define D2DERR_RECREATE_TARGET                                   0x8899000CL
#endif
#ifndef D2DERR_TOO_MANY_SHADER_ELEMENTS
#define D2DERR_TOO_MANY_SHADER_ELEMENTS                          0x8899000DL
#endif
#ifndef D2DERR_SHADER_COMPILE_FAILED
#define D2DERR_SHADER_COMPILE_FAILED                             0x8899000EL
#endif
#ifndef D2DERR_MAX_TEXTURE_SIZE_EXCEEDED
#define D2DERR_MAX_TEXTURE_SIZE_EXCEEDED                         0x8899000FL
#endif
#ifndef D2DERR_UNSUPPORTED_VERSION
#define D2DERR_UNSUPPORTED_VERSION                               0x88990010L
#endif
#ifndef D2DERR_BAD_NUMBER
#define D2DERR_BAD_NUMBER                                        0x88990011L
#endif
#ifndef D2DERR_WRONG_FACTORY
#define D2DERR_WRONG_FACTORY                                     0x88990012L
#endif
#ifndef D2DERR_LAYER_ALREADY_IN_USE
#define D2DERR_LAYER_ALREADY_IN_USE                              0x88990013L
#endif
#ifndef D2DERR_POP_CALL_DID_NOT_MATCH_PUSH
#define D2DERR_POP_CALL_DID_NOT_MATCH_PUSH                       0x88990014L
#endif
#ifndef D2DERR_WRONG_RESOURCE_DOMAIN
#define D2DERR_WRONG_RESOURCE_DOMAIN                             0x88990015L
#endif
#ifndef D2DERR_PUSH_POP_UNBALANCED
#define D2DERR_PUSH_POP_UNBALANCED                               0x88990016L
#endif
#ifndef D2DERR_RENDER_TARGET_HAS_LAYER_OR_CLIPRECT
#define D2DERR_RENDER_TARGET_HAS_LAYER_OR_CLIPRECT               0x88990017L
#endif
#ifndef D2DERR_INCOMPATIBLE_BRUSH_TYPES
#define D2DERR_INCOMPATIBLE_BRUSH_TYPES                          0x88990018L
#endif
#ifndef D2DERR_WIN32_ERROR
#define D2DERR_WIN32_ERROR                                       0x88990019L
#endif
#ifndef D2DERR_TARGET_NOT_GDI_COMPATIBLE
#define D2DERR_TARGET_NOT_GDI_COMPATIBLE                         0x8899001AL
#endif
#ifndef D2DERR_TEXT_EFFECT_IS_WRONG_TYPE
#define D2DERR_TEXT_EFFECT_IS_WRONG_TYPE                         0x8899001BL
#endif
#ifndef D2DERR_TEXT_RENDERER_NOT_RELEASED
#define D2DERR_TEXT_RENDERER_NOT_RELEASED                        0x8899001CL
#endif
#ifndef D2DERR_EXCEEDS_MAX_BITMAP_SIZE
#define D2DERR_EXCEEDS_MAX_BITMAP_SIZE                           0x8899001DL
#endif


#endif /* __MINGW32__ */

