#ifndef NODE_LIBMAXMINDDB_UTIL_STRING_COPY_H_
#define NODE_LIBMAXMINDDB_UTIL_STRING_COPY_H_

#include <cstddef>

#include "../common.h"

namespace node_libmaxminddb {

class StringCopy {
 public:
    StringCopy(const char *, size_t length);
    explicit StringCopy(const Nan::Utf8String &);
    explicit StringCopy(const v8::Local<v8::Value> &);
    ~StringCopy();

    void Set(const char *, size_t length);
    void Set(const Nan::Utf8String &);
    void Set(const v8::Local<v8::Value> &);

    inline char *operator*() const {
        return str_;
    }

 private:
    StringCopy(const StringCopy &);
    const StringCopy &operator=(const StringCopy &);

    char *str_;
};

}  // namespace node_libmaxminddb

#endif  // NODE_LIBMAXMINDDB_UTIL_STRING_COPY_H_
