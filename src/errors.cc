#include "errors.h"

#include "util/v8_helpers.h"

using v8::Function;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

namespace node_libmaxminddb {

void Errors::ImportFromNode(Local<Object> imports_errors) {
    Nan::HandleScope scope;

    Local<Object> errors_handle = Nan::New<Object>();
    errors_handle_.Reset(errors_handle);

#define LIBMAXMINDDB_ERROR_IMPORT(name)                            \
    {                                                              \
        Local<Value> type = V8Helpers::Get(imports_errors, #name); \
        if (!type->IsFunction()) {                                 \
            return Nan::ThrowError(#name " is not defined");       \
        }                                                          \
        V8Helpers::Set(errors_handle, #name, type.As<Function>()); \
    }

    LIBMAXMINDDB_ERROR_IMPORT(MmdbError)
    LIBMAXMINDDB_ERROR_IMPORT(GetAddrError)
#undef LIBMAXMINDDB_ERROR_IMPORT
}

Local<Value> Errors::ConstructError(const char* name, Local<String> message, int code) {
    Nan::EscapableHandleScope scope;

    Local<Function> type = V8Helpers::Get(Nan::New(errors_handle_), name).As<Function>();
    const int argc = 2;
    Local<Value> argv[argc] = {message, Nan::New(code)};
    return scope.Escape(Nan::NewInstance(type, argc, argv).ToLocalChecked());
}

Nan::Persistent<Object> Errors::errors_handle_;

}  // namespace node_libmaxminddb
