#ifndef NODE_LIBMAXMINDDB_UTIL_ASYNC_WORKER_H_
#define NODE_LIBMAXMINDDB_UTIL_ASYNC_WORKER_H_

#include <cassert>
#include <string>

#include "../common.h"
#include "../errors.h"
#include "v8_helpers.h"

namespace node_libmaxminddb {

class AsyncWorker {
 public:
    explicit AsyncWorker(Nan::Callback* callback, const char* resource_name);
    virtual ~AsyncWorker();

    virtual void Execute() = 0;

    virtual void SyncSuccess() {}

    virtual void Cleanup() {}

    inline void SaveToPersistent(const char* key, const v8::Local<v8::Value>& value) {
        Nan::HandleScope scope;
        V8Helpers::Set(Nan::New(persistent_handle_), key, value);
    }

    inline v8::Local<v8::Value> GetFromPersistent(const char* key) const {
        Nan::EscapableHandleScope scope;
        return scope.Escape(V8Helpers::Get(Nan::New(persistent_handle_), key));
    }

    inline void SetError(ErrorConstructor error_constructor, int code, const std::string& message) {
        assert(error_constructor != nullptr);
        error_constructor_ = error_constructor;
        error_code_ = code;
        error_message_ = message;
    }

    inline void SetResult(const v8::Local<v8::Value>& value) {
        SaveToPersistent("_result", value);
    }

    inline v8::Local<v8::Value> GetResult() {
        return GetFromPersistent("_result");
    }

    inline void Queue() {
        uv_queue_work(Nan::GetCurrentEventLoop(), &request_, UvWork, UvAfterWork);
    }

 private:
    static void UvWork(uv_work_t* req);

    static void UvAfterWork(uv_work_t* req, int status);

    void AfterWork();

    NAN_DISALLOW_ASSIGN_COPY_MOVE(AsyncWorker);

    uv_work_t request_;

    Nan::Persistent<v8::Object> persistent_handle_;
    Nan::Callback* callback_;
    Nan::AsyncResource* async_resource_;

    ErrorConstructor error_constructor_;
    int error_code_;
    std::string error_message_;
};

}  // namespace node_libmaxminddb

#endif  // NODE_LIBMAXMINDDB_UTIL_ASYNC_WORKER_H_
