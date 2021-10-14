#ifndef NODE_LIBMAXMINDDB_UTIL_MMDB_HELPERS_H_
#define NODE_LIBMAXMINDDB_UTIL_MMDB_HELPERS_H_

#include <cstdint>

#include "../common.h"

namespace node_libmaxminddb {
namespace MmdbHelpers {

inline uint64_t Uint8ArrayToUint64(uint8_t *arr) {
    // clang-format off
#ifdef NODE_LIBMAXMINDDB_IS_LITTLE_ENDIAN
    return (static_cast<uint64_t>(arr[0]) << 56) |
           (static_cast<uint64_t>(arr[1]) << 48) |
           (static_cast<uint64_t>(arr[2]) << 40) |
           (static_cast<uint64_t>(arr[3]) << 32) |
           (static_cast<uint64_t>(arr[4]) << 24) |
           (static_cast<uint64_t>(arr[5]) << 16) |
           (static_cast<uint64_t>(arr[6]) << 8)  |
           (static_cast<uint64_t>(arr[7]) << 0);
#else
    return (static_cast<uint64_t>(arr[0]) << 0)  |
           (static_cast<uint64_t>(arr[1]) << 8)  |
           (static_cast<uint64_t>(arr[2]) << 16) |
           (static_cast<uint64_t>(arr[3]) << 24) |
           (static_cast<uint64_t>(arr[4]) << 32) |
           (static_cast<uint64_t>(arr[5]) << 40) |
           (static_cast<uint64_t>(arr[6]) << 48) |
           (static_cast<uint64_t>(arr[7]) << 56);
#endif
    // clang-format on
}

}  // namespace MmdbHelpers
}  // namespace node_libmaxminddb

#endif  // NODE_LIBMAXMINDDB_UTIL_MMDB_HELPERS_H_
