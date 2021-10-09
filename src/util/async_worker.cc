#include "async_worker.h"

#include "../common.h"

using v8::Exception;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

namespace node_libmaxminddb {

AsyncWorker::AsyncWorker(Nan::Callback* callback, const char* resource_name)
    : callback_(callback), error_constructor_(nullptr) {
    request_.data = this;
    Nan::HandleScope scope;

    Local<Object> obj = Nan::New<Object>();
    persistent_handle_.Reset(obj);
    async_resource_ = new Nan::AsyncResource(resource_name, obj);
}

AsyncWorker::~AsyncWorker() {
    Nan::HandleScope scope;

    if (!persistent_handle_.IsEmpty()) {
        persistent_handle_.Reset();
    }
    delete callback_;
    delete async_resource_;
}

void AsyncWorker::UvWork(uv_work_t* req) {
    AsyncWorker* worker = static_cast<AsyncWorker*>(req->data);
    worker->Execute();
}

void AsyncWorker::UvAfterWork(uv_work_t* req, int status) {
    AsyncWorker* worker = static_cast<AsyncWorker*>(req->data);
    worker->AfterWork();
    worker->Cleanup();
    delete worker;
}

void AsyncWorker::AfterWork() {
    Nan::HandleScope scope;

    if (!error_constructor_) {
        SyncSuccess();
    }

    if (callback_) {
        if (!error_constructor_) {
            const int argc = 2;
            Local<Value> argv[argc] = {Nan::Undefined(), GetResult()};
            callback_->Call(argc, argv, async_resource_);
        } else {
            Local<String> message = V8Helpers::NewStr(error_message_.c_str());
            Local<Value> err = error_constructor_(message, error_code_);
            if (!err->IsObject()) {
                err = Exception::Error(message);
            }

            const int argc = 1;
            Local<Value> argv[argc] = {err};
            callback_->Call(argc, argv, async_resource_);
        }
    }
}

}  // namespace node_libmaxminddb
