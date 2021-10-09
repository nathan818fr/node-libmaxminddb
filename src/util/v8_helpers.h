#ifndef NODE_LIBMAXMINDDB_UTIL_V8_HELPERS_H_
#define NODE_LIBMAXMINDDB_UTIL_V8_HELPERS_H_

#include <cstdint>
#include <limits>

#include "../common.h"

namespace node_libmaxminddb {
namespace V8Helpers {

NAN_METHOD(PrivateConstructor);

v8::Local<v8::Object> CallPrivateConstructor(v8::Local<v8::Function> constructor);

inline v8::Local<v8::String> NewStr(const char *str) {
    return Nan::New(str).ToLocalChecked();
}

inline v8::Local<v8::String> NewStr(const char *str, uint32_t length) {
    if (length > static_cast<uint32_t>(std::numeric_limits<int>::max())) {
        return v8::MaybeLocal<v8::String>().ToLocalChecked();
    }
    return Nan::New(str, static_cast<int>(length)).ToLocalChecked();
}
template <typename T>
v8::Local<v8::Number> NewStr(const char *str, T length) = delete;

inline v8::Local<v8::Number> NewNum(uint16_t value) {
    return Nan::New(static_cast<uint32_t>(value));
}
inline v8::Local<v8::Number> NewNum(uint64_t value) {
    return Nan::New(static_cast<double>(value));
}
inline v8::Local<v8::Number> NewNum(ssize_t value) {
    return Nan::New(static_cast<double>(value));
}
inline v8::Local<v8::Number> NewNum(float value) {
    return Nan::New(static_cast<double>(value));
}
template <typename T>
v8::Local<v8::Number> NewNum(T value) = delete;

inline v8::Maybe<bool> Set(v8::Local<v8::Object> obj, const char *key, v8::Local<v8::Value> value) {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);
    return obj->Set(isolate->GetCurrentContext(), NewStr(key), value);
}

inline v8::Local<v8::Value> Get(v8::Local<v8::Object> obj, const char *key) {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::EscapableHandleScope scope(isolate);
    return scope.Escape(obj->Get(isolate->GetCurrentContext(), NewStr(key)).ToLocalChecked());
}

inline void SetPrototypeAccessor(
    v8::Local<v8::FunctionTemplate> tpl,
    const char *name,
    Nan::GetterCallback getter,
    Nan::SetterCallback setter = 0,
    v8::Local<v8::Value> data = v8::Local<v8::Value>(),
    v8::AccessControl settings = v8::DEFAULT,
    v8::PropertyAttribute attribute = v8::None,
    Nan::imp::Sig signature = Nan::imp::Sig()  //
) {
    Nan::HandleScope scope;
    Nan::SetAccessor(tpl->InstanceTemplate(), NewStr(name), getter, setter, data, settings, attribute, signature);
}

inline void DefineReadOnlyProperty(v8::Local<v8::Object> obj, const char *key, v8::Local<v8::Value> value) {
    Nan::HandleScope scope;
    Nan::DefineOwnProperty(obj, NewStr(key), value, static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
}

inline void FreezeObject(v8::Local<v8::Object> obj) {
    Nan::HandleScope scope;
    obj->SetIntegrityLevel(Nan::GetCurrentContext(), v8::IntegrityLevel::kFrozen);
}

}  // namespace V8Helpers
}  // namespace node_libmaxminddb

/*
 * ObjectWrap helpers
 */

#define V8_HELPERS_CONSTRUCTOR()                                 \
    static inline Nan::Persistent<v8::Function> &constructor() { \
        static Nan::Persistent<v8::Function> constructor;        \
        return constructor;                                      \
    }

#define V8_HELPERS_OBJECTWRAP_INIT_BEGIN(class_name)                                      \
    v8::Local<v8::FunctionTemplate> tpl =                                                 \
        Nan::New<v8::FunctionTemplate>(node_libmaxminddb::V8Helpers::PrivateConstructor); \
    tpl->SetClassName(Nan::New<v8::String>(class_name).ToLocalChecked());                 \
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

#define V8_HELPERS_OBJECTWRAP_INIT_END(class_name)               \
    tpl->ReadOnlyPrototype();                                    \
    constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked()); \
    V8Helpers::Set(target, class_name, Nan::New(constructor()));

/*
 * Arguments helpers
 */

#define __V8_HELPERS_GETARG0(name, pos, v8Type, typeName) \
    __V8_HELPERS_GETARG1(name, pos, v8::v8Type, Is##v8Type, typeName)
#define __V8_HELPERS_GETARG1(name, pos, Type, IsType, typeName)                         \
    v8::Local<Type> name;                                                               \
    {                                                                                   \
        if (!info[pos]->IsType()) {                                                     \
            return Nan::ThrowTypeError(#name " (argument " #pos ") must be " typeName); \
        }                                                                               \
        name = info[pos].As<Type>();                                                    \
    }

#define V8_HELPERS_GETARG_OBJECT(name, pos) __V8_HELPERS_GETARG0(name, pos, Object, "an object")
#define V8_HELPERS_GETARG_FUNCTION(name, pos) __V8_HELPERS_GETARG0(name, pos, Function, "a function")
#define V8_HELPERS_GETARG_STRING(name, pos) __V8_HELPERS_GETARG0(name, pos, String, "a string")

/*
 * Properties helpers
 */

#define __V8_HELPERS_GETPROP0(obj, name, v8Type, typeName) \
    __V8_HELPERS_GETPROP1(obj, name, v8::v8Type, Is##v8Type, typeName)
#define __V8_HELPERS_GETPROP1(obj, name, Type, IsType, typeName)             \
    v8::Local<Type> obj##_##name;                                            \
    {                                                                        \
        Local<Value> v = V8Helpers::Get(options, #name);                     \
        if (!v->IsType()) {                                                  \
            return Nan::ThrowTypeError(#obj "." #name " must be " typeName); \
        }                                                                    \
        obj##_##name = v.As<Type>();                                         \
    }

#define V8_HELPERS_GETPROP_STRING(obj, name) __V8_HELPERS_GETPROP0(obj, name, String, "a string")

#endif  // NODE_LIBMAXMINDDB_UTIL_V8_HELPERS_H_
