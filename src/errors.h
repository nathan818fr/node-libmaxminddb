#ifndef NODE_LIBMAXMINDDB_ERRORS_H_
#define NODE_LIBMAXMINDDB_ERRORS_H_

#include "common.h"

namespace node_libmaxminddb {

typedef v8::Local<v8::Value> (*ErrorConstructor)(v8::Local<v8::String> message, int code);

class Errors {
 public:
    static void ImportFromNode(v8::Local<v8::Object> imports_errors);

#define LIBMAXMINDDB_ERROR_CONSTRUCTOR(name)                                    \
    static v8::Local<v8::Value> name(v8::Local<v8::String> message, int code) { \
        return ConstructError(#name, message, code);                            \
    }

    LIBMAXMINDDB_ERROR_CONSTRUCTOR(MmdbError);
    LIBMAXMINDDB_ERROR_CONSTRUCTOR(GetAddrError);
#undef LIBMAXMINDDB_ERROR_CONSTRUCTOR

 private:
    static v8::Local<v8::Value> ConstructError(const char* name, v8::Local<v8::String> message, int code);

    static Nan::Persistent<v8::Object> errors_handle_;
};

}  // namespace node_libmaxminddb

#endif  // NODE_LIBMAXMINDDB_ERRORS_H_
