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

#endif  // NODE_LIBMAXMINDDB_COMMON_H_
