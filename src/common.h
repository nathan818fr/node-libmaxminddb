#ifndef NODE_LIBMAXMINDDB_COMMON_H_
#define NODE_LIBMAXMINDDB_COMMON_H_

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#endif

// IWYU pragma: begin_exports
#include <maxminddb.h>
#include <nan.h>
#include <node.h>
#include <uv.h>
#include <v8.h>
// IWYU pragma: end_exports

#ifdef _WIN32
#include <windows.h>
#if REG_DWORD == REG_DWORD_LITTLE_ENDIAN
#define NODE_LIBMAXMINDDB_IS_LITTLE_ENDIAN
#endif
#elif defined(__APPLE__)
#include <machine/endian.h>
#if BYTE_ORDER == LITTLE_ENDIAN
#define NODE_LIBMAXMINDDB_IS_LITTLE_ENDIAN
#endif
#else
#include <endian.h>
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define NODE_LIBMAXMINDDB_IS_LITTLE_ENDIAN
#endif
#endif

#endif  // NODE_LIBMAXMINDDB_COMMON_H_
