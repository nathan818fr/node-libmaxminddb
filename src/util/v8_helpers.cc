#include "v8_helpers.h"

#include "../common.h"

using v8::External;
using v8::Function;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::Value;

namespace node_libmaxminddb {
namespace V8Helpers {

NAN_METHOD(PrivateConstructor) {
    Nan::HandleScope scope;

    if (info.IsConstructCall() && info.Length() == 1 && info[0]->IsExternal() &&
        info[0].As<External>()->Value() == nullptr) {
        return info.GetReturnValue().Set(info.This());
    }

    return Nan::ThrowError("You cannot call this constructor directly");
}

Local<Object> CallPrivateConstructor(Local<Function> constructor) {
    Nan::EscapableHandleScope scope;

    Local<Value> info[] = {External::New(Isolate::GetCurrent(), nullptr)};
    Local<Object> that = Nan::NewInstance(constructor, 1, info).ToLocalChecked();

    return scope.Escape(that);
}

}  // namespace V8Helpers
}  // namespace node_libmaxminddb
