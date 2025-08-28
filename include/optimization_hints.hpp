/*
 * Copyright (c) 2024 Chiradip Mandal
 * Author: Chiradip Mandal
 * Organization: Space-RF.org
 * 
 * This file is part of DB25 SQL Tokenizer.
 * 
 * Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

// Branch prediction hints for optimizing hot paths
#ifdef __cplusplus
  #if __cplusplus >= 202002L
    // C++20 and later: use standard attributes
    #define LIKELY [[likely]]
    #define UNLIKELY [[unlikely]]
  #else
    // Pre-C++20: use compiler-specific hints
    #ifdef __GNUC__
      #define LIKELY [[gnu::hot]]
      #define UNLIKELY [[gnu::cold]]
      #define EXPECT(x, val) __builtin_expect((x), (val))
      #define EXPECT_TRUE(x) __builtin_expect(!!(x), 1)
      #define EXPECT_FALSE(x) __builtin_expect(!!(x), 0)
    #else
      #define LIKELY
      #define UNLIKELY
      #define EXPECT(x, val) (x)
      #define EXPECT_TRUE(x) (x)
      #define EXPECT_FALSE(x) (x)
    #endif
  #endif
#endif

// Cache prefetch hints
#ifdef __GNUC__
  #define PREFETCH(addr, rw, locality) __builtin_prefetch(addr, rw, locality)
#else
  #define PREFETCH(addr, rw, locality) ((void)0)
#endif

// Function attributes for optimization
#ifdef __GNUC__
  #define HOT_FUNCTION __attribute__((hot))
  #define COLD_FUNCTION __attribute__((cold))
  #define PURE_FUNCTION __attribute__((pure))
  #define CONST_FUNCTION __attribute__((const))
  #define FLATTEN __attribute__((flatten))
  #define ALWAYS_INLINE __attribute__((always_inline)) inline
  #define NEVER_INLINE __attribute__((noinline))
#else
  #define HOT_FUNCTION
  #define COLD_FUNCTION
  #define PURE_FUNCTION
  #define CONST_FUNCTION
  #define FLATTEN
  #define ALWAYS_INLINE inline
  #define NEVER_INLINE
#endif

// Restrict pointer aliasing
#ifdef __GNUC__
  #define RESTRICT __restrict
#else
  #define RESTRICT
#endif