// Copyright (c) 2011 Google, Inc.

// CityHash, by Geoff Pike and Jyrki Alakuijala
//
// http://code.google.com/p/cityhash/
//
// This file provides a few functions for hashing strings.  All of them are
// high-quality functions in the sense that they pass standard tests such
// as Austin Appleby's SMHasher.  They are also fast.
//
// For 64-bit x86 code, on short strings, we don't know of anything faster than
// CityHash64 that is of comparable quality.  We believe our nearest competitor
// is Murmur3.  For 64-bit x86 code, CityHash64 is an excellent choice for hash
// tables and most other hashing (excluding cryptography).
//
// For 64-bit x86 code, on long strings, the picture is more complicated.
// On many recent Intel CPUs, such as Nehalem, Westmere, Sandy Bridge, etc.,
// CityHashCrc128 appears to be faster than all competitors of comparable
// quality.  CityHash128 is also good but not quite as fast.  We believe our
// nearest competitor is Bob Jenkins' Spooky.  We don't have great data for
// other 64-bit CPUs, but for long strings we know that Spooky is slightly
// faster than CityHash on some relatively recent AMD x86-64 CPUs, for example.
//
// For 32-bit x86 code, we don't know of anything faster than CityHash32 that
// is of comparable quality.  We believe our nearest competitor is Murmur3A.
// (On 64-bit CPUs, it is typically faster to use the other CityHash variants.)
//
// Functions in the CityHash family are not suitable for cryptography.
//
// WARNING: This code has been only lightly tested on big-endian platforms!
// It is known to work well on little-endian platforms that have a small penalty
// for unaligned reads, such as current Intel and AMD moderate-to-high-end CPUs.
// It should work on all 32-bit and 64-bit platforms that allow unaligned reads;
// bug reports are welcome.
//
// By the way, for some hash functions, given strings a and b, the hash
// of a+b is easily derived from the hashes of a and b.  This property
// doesn't hold for any hash functions in this file.


#ifndef __sc_city_hash_h_
#define __sc_city_hash_h_

#include <stdlib.h>  // for size_t.
#include <stdint.h>

namespace sc {

#define Uint128Low64(x)         (x).first
#define Uint128High64(x)        (x).second

typedef struct _uint128 {
  uint64_t first;
  uint64_t second;
} uint128_t;


inline uint64_t _cityhash128to64(const uint128_t x) {
  // Murmur-inspired hashing.
  const uint64_t kMul = 0x9ddfea08eb382d69ULL;
  uint64_t a = (Uint128Low64(x) ^ Uint128High64(x)) * kMul;
  a ^= (a >> 47);
  uint64_t b = (Uint128High64(x) ^ a) * kMul;
  b ^= (b >> 47);
  b *= kMul;
  return b;
}


class cityhash
{
public:
    // Hash function for a byte array.  Most useful in 32-bit binaries.
    static uint32_t hash32(const char *buf, size_t len);

    // Hash function for a byte array.
    static uint64_t hash64(const char *buf, size_t len);

    // Hash function for a byte array.
    static uint128_t hash128(const char *buf, size_t len);


    // Hash function for a byte array.  For convenience, a 64-bit seed is also
    // hashed into the result.
    static uint64_t hash64withseed(const char *buf, size_t len, uint64_t seed);

    // Hash function for a byte array.  For convenience, two seeds are also
    // hashed into the result.
    static uint64_t hash64withseeds(const char *buf, size_t len,
                               uint64_t seed0, uint64_t seed1);
    
    // Hash function for a byte array.  For convenience, a 128-bit seed is also
    // hashed into the result.
    static uint128_t hash128withseed(const char *buf, size_t len, uint128_t seed);
   
    // Hash 128 input bits down to 64 bits of output.
    // This is intended to be a reasonably good hash function.
    static uint64_t hash128to64(const uint128_t x); 

    static uint128_t crc128(const char *s, size_t len);

    static uint128_t crc128withseed(const char *s, size_t len, uint128_t seed);

    static void crc256(const char *s, size_t len, uint64_t *result);
} ; /* end of class cityhash */


}

#endif  /* _sc_city_hash_h */
