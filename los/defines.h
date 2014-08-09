/*
 * LOS: Libre Octave Studio
 *
 * Copyright (C) 1999-2004 Werner Schweer <ws@seh.de>
 * Copyright (C) 2006-2011 Remon Sijrier
 * Copyright (C) 2011-2012 Andrew Williams
 * Copyright (C) 2011-2012 Christopher Cherrett
 * Copyright (C) 2012-2014 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the COPYING file.
 */

#ifndef LOS_DEFINES_H_INCLUDED
#define LOS_DEFINES_H_INCLUDED

#include "config.h"

/* Compatibility with non-clang compilers */
#ifndef __has_feature
# define __has_feature(x) 0
#endif
#ifndef __has_extension
# define __has_extension __has_feature
#endif

/* Check OS */
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
# define LOS_OS_WIN64
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
# define LOS_OS_WIN32
#elif defined(__APPLE__)
# define LOS_OS_MAC
#elif defined(__HAIKU__)
# define LOS_OS_HAIKU
#elif defined(__linux__) || defined(__linux)
# define LOS_OS_LINUX
#else
# warning Unsupported platform!
#endif

#if defined(LOS_OS_WIN32) || defined(LOS_OS_WIN64)
# define LOS_OS_WIN
#elif defined(LOS_OS_LINUX) || defined(LOS_OS_MAC)
# define LOS_OS_UNIX
#endif

/* Check for C++11 support */
#if defined(HAVE_CPP11_SUPPORT)
# define LOS_PROPER_CPP11_SUPPORT
#elif defined(__cplusplus)
# if __cplusplus >= 201103L || (defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 405 && defined(__GXX_EXPERIMENTAL_CXX0X__)) || __has_extension(cxx_noexcept)
#  define LOS_PROPER_CPP11_SUPPORT
#  if (defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) < 407 && ! defined(__clang__)) || (defined(__clang__) && ! __has_extension(cxx_override_control))
#   define override // gcc4.7+ only
#   define final    // gcc4.7+ only
#  endif
# endif
#endif

#if defined(__cplusplus) && ! defined(LOS_PROPER_CPP11_SUPPORT)
# define noexcept throw()
# define override
# define final
# define nullptr NULL
#endif

/* Common includes */
#ifdef __cplusplus
# include <cstddef>
#else
# include <stdbool.h>
# include <stddef.h>
#endif

/* Define various string format types */
#if defined(LOS_OS_WIN64)
# define P_INT64   "%I64i"
# define P_UINT64  "%I64u"
# define P_INTPTR  "%I64i"
# define P_UINTPTR "%I64x"
# define P_SIZE    "%I64u"
#elif defined(LOS_OS_WIN32)
# define P_INT64   "%I64i"
# define P_UINT64  "%I64u"
# define P_INTPTR  "%i"
# define P_UINTPTR "%x"
# define P_SIZE    "%u"
#elif defined(LOS_OS_MAC) && defined(__LP64__)
# define P_INT64   "%lli"
# define P_UINT64  "%llu"
# define P_INTPTR  "%li"
# define P_UINTPTR "%lx"
# define P_SIZE    "%lu"
#elif defined(__WORDSIZE) && __WORDSIZE == 64
# define P_INT64   "%li"
# define P_UINT64  "%lu"
# define P_INTPTR  "%li"
# define P_UINTPTR "%lx"
# define P_SIZE    "%lu"
#else
# define P_INT64   "%lli"
# define P_UINT64  "%llu"
# define P_INTPTR  "%i"
# define P_UINTPTR "%x"
# define P_SIZE    "%u"
#endif

/* Define LOS_ASSERT* */
#if defined(LOS_NO_ASSERTS)
# define LOS_ASSERT(cond)
# define LOS_ASSERT_INT(cond, value)
# define LOS_ASSERT_INT2(cond, v1, v2)
#elif defined(NDEBUG)
# define LOS_ASSERT      LOS_SAFE_ASSERT
# define LOS_ASSERT_INT  LOS_SAFE_ASSERT_INT
# define LOS_ASSERT_INT2 LOS_SAFE_ASSERT_INT2
#else
# define LOS_ASSERT(cond)              assert(cond)
# define LOS_ASSERT_INT(cond, value)   assert(cond)
# define LOS_ASSERT_INT2(cond, v1, v2) assert(cond)
#endif

/* Define LOS_SAFE_ASSERT* */
#define LOS_SAFE_ASSERT(cond)               if (! (cond)) los_safe_assert      (#cond, __FILE__, __LINE__);
#define LOS_SAFE_ASSERT_INT(cond, value)    if (! (cond)) los_safe_assert_int  (#cond, __FILE__, __LINE__, static_cast<int>(value));
#define LOS_SAFE_ASSERT_INT2(cond, v1, v2)  if (! (cond)) los_safe_assert_int2 (#cond, __FILE__, __LINE__, static_cast<int>(v1), static_cast<int>(v2));
#define LOS_SAFE_ASSERT_UINT(cond, value)   if (! (cond)) los_safe_assert_uint (#cond, __FILE__, __LINE__, static_cast<uint>(value));
#define LOS_SAFE_ASSERT_UINT2(cond, v1, v2) if (! (cond)) los_safe_assert_uint2(#cond, __FILE__, __LINE__, static_cast<uint>(v1), static_cast<uint>(v2));

#define LOS_SAFE_ASSERT_BREAK(cond)         if (! (cond)) { los_safe_assert(#cond, __FILE__, __LINE__); break; }
#define LOS_SAFE_ASSERT_CONTINUE(cond)      if (! (cond)) { los_safe_assert(#cond, __FILE__, __LINE__); continue; }
#define LOS_SAFE_ASSERT_RETURN(cond, ret)   if (! (cond)) { los_safe_assert(#cond, __FILE__, __LINE__); return ret; }

/* Define LOS_SAFE_EXCEPTION */
#define LOS_SAFE_EXCEPTION(msg)             catch(...) { los_safe_exception(msg, __FILE__, __LINE__); }

#define LOS_SAFE_EXCEPTION_BREAK(msg)       catch(...) { los_safe_exception(msg, __FILE__, __LINE__); break; }
#define LOS_SAFE_EXCEPTION_CONTINUE(msg)    catch(...) { los_safe_exception(msg, __FILE__, __LINE__); continue; }
#define LOS_SAFE_EXCEPTION_RETURN(msg, ret) catch(...) { los_safe_exception(msg, __FILE__, __LINE__); return ret; }

/* Define LOS_DECLARE_NON_COPY_CLASS */
#ifdef LOS_PROPER_CPP11_SUPPORT
# define LOS_DECLARE_NON_COPY_CLASS(ClassName) \
private:                                         \
    ClassName(ClassName&) = delete;              \
    ClassName(const ClassName&) = delete;        \
    ClassName& operator=(ClassName&) = delete;   \
    ClassName& operator=(const ClassName&) = delete;
#else
# define LOS_DECLARE_NON_COPY_CLASS(ClassName) \
private:                                         \
    ClassName(ClassName&);                       \
    ClassName(const ClassName&);                 \
    ClassName& operator=(ClassName&);            \
    ClassName& operator=(const ClassName&);
#endif

/* Define LOS_DECLARE_NON_COPY_STRUCT */
#ifdef LOS_PROPER_CPP11_SUPPORT
# define LOS_DECLARE_NON_COPY_STRUCT(StructName) \
    StructName(StructName&) = delete;              \
    StructName(const StructName&) = delete;        \
    StructName& operator=(StructName&) = delete;   \
    StructName& operator=(const StructName&) = delete;
#else
# define LOS_DECLARE_NON_COPY_STRUCT(StructName)
#endif

/* Define LOS_PREVENT_HEAP_ALLOCATION */
#ifdef LOS_PROPER_CPP11_SUPPORT
# define LOS_PREVENT_HEAP_ALLOCATION \
private:                               \
    static void* operator new(std::size_t) = delete; \
    static void operator delete(void*) = delete;
#else
# define LOS_PREVENT_HEAP_ALLOCATION \
private:                               \
    static void* operator new(std::size_t); \
    static void operator delete(void*);
#endif

/* Define LOS_PREVENT_VIRTUAL_HEAP_ALLOCATION */
#ifdef LOS_PROPER_CPP11_SUPPORT
# define LOS_PREVENT_VIRTUAL_HEAP_ALLOCATION  \
private:                                        \
    static void* operator new(std::size_t) = delete;
#else
# define LOS_PREVENT_VIRTUAL_HEAP_ALLOCATION  \
private:                                        \
    static void* operator new(std::size_t);
#endif

/* Define LOS_EXTERN_C */
#ifdef __cplusplus
# define LOS_EXTERN_C extern "C"
#else
# define LOS_EXTERN_C
#endif

/* Define LOS_EXPORT */
#if defined(LOS_OS_WIN) && ! defined(__WINE__)
# define LOS_EXPORT LOS_EXTERN_C __declspec (dllexport)
#else
# define LOS_EXPORT LOS_EXTERN_C __attribute__ ((visibility("default")))
#endif

/* Define LOS_OS_SEP */
#ifdef LOS_OS_WIN
# define LOS_OS_SEP     '\\'
# define LOS_OS_SEP_STR "\\"
#else
# define LOS_OS_SEP     '/'
# define LOS_OS_SEP_STR "/"
#endif

/* Useful typedefs */
typedef unsigned char uchar;
typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;

#endif /* LOS_DEFINES_H_INCLUDED */
