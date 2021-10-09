#include "string_copy.h"

#include <cstring>

#include "../common.h"

using v8::Local;
using v8::Value;

namespace node_libmaxminddb {

StringCopy::StringCopy(const char *value, size_t length) : str_(nullptr) {
    Set(value, length);
}

StringCopy::StringCopy(const Nan::Utf8String &value) : str_(nullptr) {
    Set(value);
}

StringCopy::StringCopy(const Local<Value> &value) : str_(nullptr) {
    Set(value);
}

void StringCopy::Set(const char *value, size_t length) {
    delete[] str_;

    str_ = new char[length + 1];
    strncpy(str_, value, length);
    str_[length] = '\0';
}

void StringCopy::Set(const Local<Value> &value) {
    Set(Nan::Utf8String(value));
}

void StringCopy::Set(const Nan::Utf8String &value) {
    Set(*value, value.length());
}

StringCopy::~StringCopy() {
    delete[] str_;
}

}  // namespace node_libmaxminddb
