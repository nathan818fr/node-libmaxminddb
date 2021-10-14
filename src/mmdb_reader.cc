#include "mmdb_reader.h"

#include <cstdint>
#include <cstring>
#include <string>

#include "errors.h"
#include "util/async_worker.h"
#include "util/mmdb_entry_data_list_holder.h"
#include "util/mmdb_helpers.h"
#include "util/string_copy.h"

#ifdef _WIN32
#include <ws2tcpip.h>  // for gai_strerror
#else
#include <netdb.h>  // for gai_strerror
#endif

using v8::Array;
using v8::BigInt;
using v8::Boolean;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

namespace node_libmaxminddb {

NAN_MODULE_INIT(MmdbReader::Init) {
    Nan::HandleScope scope;
    V8_HELPERS_OBJECTWRAP_INIT_BEGIN("MmdbReader");

    Nan::SetMethod(tpl, "open", Open);
    Nan::SetPrototypeMethod(tpl, "close", Close);
    Nan::SetPrototypeMethod(tpl, "lookup", Lookup);

    V8_HELPERS_OBJECTWRAP_INIT_END("MmdbReader");
}

Local<Object> MmdbReader::NewInstance(MMDB_s *mmdb) {
    Nan::EscapableHandleScope scope;

    Local<Object> that = V8Helpers::CallPrivateConstructor(Nan::New(constructor()));
    (new MmdbReader(mmdb))->Wrap(that);

    // Define immutable accessors
    V8Helpers::DefineReadOnlyProperty(that, "fileName", V8Helpers::NewStr(mmdb->filename));
    V8Helpers::DefineReadOnlyProperty(that, "fileSize", V8Helpers::NewNum(mmdb->file_size));
    V8Helpers::DefineReadOnlyProperty(that, "depth", V8Helpers::NewNum(mmdb->depth));

    Local<Object> metadata = Nan::New<Object>();
    auto m = mmdb->metadata;
    V8Helpers::DefineReadOnlyProperty(that, "metadata", metadata);
    V8Helpers::DefineReadOnlyProperty(metadata, "nodeCount", Nan::New(m.node_count));
    V8Helpers::DefineReadOnlyProperty(metadata, "recordSize", V8Helpers::NewNum(m.record_size));
    V8Helpers::DefineReadOnlyProperty(metadata, "ipVersion", V8Helpers::NewNum(m.ip_version));
    V8Helpers::DefineReadOnlyProperty(metadata, "databaseType", V8Helpers::NewStr(m.database_type));
    V8Helpers::DefineReadOnlyProperty(metadata, "buildEpoch", V8Helpers::NewNum(m.build_epoch));

    V8Helpers::DefineReadOnlyProperty(
        metadata, "binaryFormatMajorVersion", V8Helpers::NewNum(m.binary_format_major_version));
    V8Helpers::DefineReadOnlyProperty(
        metadata, "binaryFormatMinorVersion", V8Helpers::NewNum(m.binary_format_minor_version));

    Local<Array> languages = Nan::New<Array>(m.languages.count);
    for (size_t i = 0; i < m.languages.count; ++i) {
        Nan::Set(languages, i, V8Helpers::NewStr(m.languages.names[i]));
    }
    V8Helpers::FreezeObject(languages);
    V8Helpers::DefineReadOnlyProperty(metadata, "languages", languages);

    Local<Array> descriptions = Nan::New<Array>(m.description.count);
    for (size_t i = 0; i < m.description.count; ++i) {
        MMDB_description_s *d = m.description.descriptions[i];
        Local<Object> description = Nan::New<Object>();
        V8Helpers::DefineReadOnlyProperty(description, "language", V8Helpers::NewStr(d->language));
        V8Helpers::DefineReadOnlyProperty(description, "description", V8Helpers::NewStr(d->description));
        Nan::Set(descriptions, i, description);
    }
    V8Helpers::FreezeObject(descriptions);
    V8Helpers::DefineReadOnlyProperty(metadata, "description", descriptions);

    return scope.Escape(that);
}

MmdbReader::MmdbReader(MMDB_s *mmdb) : mmdb_(mmdb) {}

MmdbReader::~MmdbReader() {
    // Free mmdb if Close was not called by the user
    MMDB_close(mmdb_);
    delete mmdb_;
}

NAN_METHOD(MmdbReader::Open) {
    class Worker : public AsyncWorker {
     public:
        Worker(Nan::Callback *callback, const Local<String> &filename)
            : AsyncWorker(callback, "node_libmaxminddb_MmdbReader_Open"), filename_(filename) {}

        void Execute() override {
            int mmdb_status = MMDB_open(*filename_, MMDB_MODE_MMAP, &mmdb_);
            if (mmdb_status != MMDB_SUCCESS) {
                return SetError(
                    Errors::MmdbError, mmdb_status, std::string("MMDB_open failed: ") + MMDB_strerror(mmdb_status));
            }
        }

        void SyncSuccess() override {
            Nan::HandleScope scope;

            MMDB_s *mmdb = new MMDB_s;
            memcpy(mmdb, &mmdb_, sizeof(MMDB_s));
            SetResult(NewInstance(mmdb));
        }

     private:
        StringCopy filename_;
        MMDB_s mmdb_;
    };

    Nan::HandleScope scope;

    V8_HELPERS_GETARG_OBJECT(options, 0);
    V8_HELPERS_GETPROP_STRING(options, filename);
    V8_HELPERS_GETARG_FUNCTION(callback, 1);

    (new Worker(new Nan::Callback(callback), options_filename))->Queue();
    info.GetReturnValue().Set(info.This());
}

NAN_METHOD(MmdbReader::Close) {
    class Worker : public AsyncWorker {
     public:
        Worker(Nan::Callback *callback, MmdbReader *mmdb_reader)
            : AsyncWorker(callback, "node_libmaxminddb_MmdbReader_Close"), mmdb_reader_(mmdb_reader) {
            mmdb_reader_->Ref();
        }

        void Execute() override {
            MMDB_s *mmdb;
            {
                UvWriteLock lock(&mmdb_reader_->mmdb_mutex_);
                mmdb = mmdb_reader_->mmdb_;
                mmdb_reader_->mmdb_ = nullptr;
            }

            if (!mmdb) {
                closed_ = false;
            } else {
                closed_ = true;
                MMDB_close(mmdb);
                delete mmdb;
            }
        }

        void SyncSuccess() override {
            SetResult(Nan::New(closed_));
        }

        void Cleanup() override {
            mmdb_reader_->Unref();
        }

     private:
        MmdbReader *mmdb_reader_;

        bool closed_;
    };

    Nan::HandleScope scope;

    V8_HELPERS_GETARG_FUNCTION(callback, 0);

    MmdbReader *mmdb_reader = Nan::ObjectWrap::Unwrap<MmdbReader>(info.Holder());  // Safe unwrap using Holder

    (new Worker(new Nan::Callback(callback), mmdb_reader))->Queue();
    info.GetReturnValue().Set(info.This());
}

#define MMDB_ASYNC_READ_EXECUTE(mmdb_reader)                                              \
    mmdb_reader->mmdb_mutex_.ReadLock();                                                  \
    if (!mmdb_reader->mmdb_) {                                                            \
        return SetError(Errors::MmdbError, MMDB_FILE_OPEN_ERROR, "MmdbReader is closed"); \
    }                                                                                     \
    MMDB_s *mmdb = mmdb_reader->mmdb_;

#define MMDB_ASYNC_READ_CLEANUP(mmdb_reader) mmdb_reader->mmdb_mutex_.ReadUnlock();

NAN_METHOD(MmdbReader::Lookup) {
    class Worker : public AsyncWorker {
     public:
        Worker(Nan::Callback *callback, MmdbReader *mmdb_reader, const Local<String> &ip)
            : AsyncWorker(callback, "node_libmaxminddb_MmdbReader_Lookup"),
              mmdb_reader_(mmdb_reader),
              ip_(ip),
              entry_data_list_(nullptr) {
            mmdb_reader_->Ref();
        }

        void Execute() override {
            MMDB_ASYNC_READ_EXECUTE(mmdb_reader_);

            int gai_status, mmdb_status;
            lookup_result_ = MMDB_lookup_string(mmdb, *ip_, &gai_status, &mmdb_status);
            if (gai_status != 0) {
                return SetError(
                    Errors::GetAddrError, gai_status,
                    std::string("MMDB_lookup_string failed: getaddrinfo failed: ") + gai_strerror(gai_status));
            }
            if (mmdb_status != MMDB_SUCCESS) {
                return SetError(
                    Errors::MmdbError, mmdb_status,
                    std::string("MMDB_lookup_string failed: ") + MMDB_strerror(mmdb_status));
            }
            if (lookup_result_.found_entry) {
                mmdb_status = MMDB_get_entry_data_list(&lookup_result_.entry, &entry_data_list_);
                if (mmdb_status != MMDB_SUCCESS) {
                    return SetError(
                        Errors::MmdbError, mmdb_status,
                        std::string("MMDB_get_entry_data_list failed: ") + MMDB_strerror(mmdb_status));
                }
                if (!entry_data_list_) {
                    return SetError(
                        Errors::MmdbError, MMDB_INVALID_DATA_ERROR,
                        "MMDB_get_entry_data_list failed: Returned a null entry");
                }
            }
        }

        void SyncSuccess() override {
            Nan::HandleScope scope;

            Local<Value> entry;
            if (!lookup_result_.found_entry) {
                entry = Nan::Undefined();
            } else {
                MmdbEntryDataListHolder entry_data_list_holder(entry_data_list_);
                int mmdb_status = MMDB_SUCCESS;
                entry = EntryDataListToValue(entry_data_list_holder, &mmdb_status);
                if (mmdb_status != MMDB_SUCCESS) {
                    return SetError(
                        Errors::MmdbError, mmdb_status,
                        std::string("EntryDataListToValue failed: ") + MMDB_strerror(mmdb_status));
                }
            }

            Local<Object> res = Nan::New<Object>();
            V8Helpers::Set(res, "netmask", V8Helpers::NewNum(lookup_result_.netmask));
            V8Helpers::Set(res, "entry", entry);
            SetResult(res);
        }

        void Cleanup() override {
            MMDB_free_entry_data_list(entry_data_list_);
            MMDB_ASYNC_READ_CLEANUP(mmdb_reader_);
            mmdb_reader_->Unref();
        }

     private:
        MmdbReader *mmdb_reader_;
        StringCopy ip_;

        MMDB_lookup_result_s lookup_result_;
        MMDB_entry_data_list_s *entry_data_list_;
    };

    Nan::HandleScope scope;

    V8_HELPERS_GETARG_STRING(ip, 0);
    V8_HELPERS_GETARG_FUNCTION(callback, 1);

    MmdbReader *mmdb_reader = Nan::ObjectWrap::Unwrap<MmdbReader>(info.Holder());  // Safe unwrap using Holder

    (new Worker(new Nan::Callback(callback), mmdb_reader, ip))->Queue();
    info.GetReturnValue().Set(info.This());
}

Local<Value> MmdbReader::EntryDataListToValue(MmdbEntryDataListHolder &list, int *status) {
    Nan::EscapableHandleScope scope;
    switch (list->entry_data.type) {
        case MMDB_DATA_TYPE_MAP: {
            uint32_t size = list->entry_data.data_size;
            Local<Object> ret = Object::New(Nan::GetCurrentContext()->GetIsolate(), Nan::Null(), nullptr, nullptr, 0);
            for (list.next(); size && list != nullptr; --size) {
                if (list->entry_data.type != MMDB_DATA_TYPE_UTF8_STRING) {
                    *status = MMDB_INVALID_DATA_ERROR;
                    break;
                }

                Local<String> key = V8Helpers::NewStr(list->entry_data.utf8_string, list->entry_data.data_size);
                list.next();

                Local<Value> value = EntryDataListToValue(list, status);
                if (*status != MMDB_SUCCESS) {
                    break;
                }

                Nan::Set(ret, key, value);
            }
            return scope.Escape(ret);
        }
        case MMDB_DATA_TYPE_ARRAY: {
            uint32_t size = list->entry_data.data_size;
            Local<Array> ret = Nan::New<Array>(size);
            uint32_t i = 0;
            for (list.next(); i < size && list != nullptr; ++i) {
                Local<Value> value = EntryDataListToValue(list, status);
                if (*status != MMDB_SUCCESS) {
                    break;
                }

                Nan::Set(ret, i, value);
            }
            return scope.Escape(ret);
        }
        case MMDB_DATA_TYPE_UTF8_STRING: {
            Local<String> value = V8Helpers::NewStr(list->entry_data.utf8_string, list->entry_data.data_size);
            list.next();
            return scope.Escape(value);
        }
        case MMDB_DATA_TYPE_BYTES: {
            MaybeLocal<Object> value =
                Nan::CopyBuffer(reinterpret_cast<const char *>(list->entry_data.bytes), list->entry_data.data_size);
            list.next();
            return scope.Escape(value.ToLocalChecked());
        }
        case MMDB_DATA_TYPE_DOUBLE: {
            Local<Number> value = Nan::New(list->entry_data.double_value);
            list.next();
            return scope.Escape(value);
        }
        case MMDB_DATA_TYPE_FLOAT: {
            Local<Number> value = V8Helpers::NewNum(list->entry_data.float_value);
            list.next();
            return scope.Escape(value);
        }
        case MMDB_DATA_TYPE_UINT16: {
            Local<Number> value = V8Helpers::NewNum(list->entry_data.uint16);
            list.next();
            return scope.Escape(value);
        }
        case MMDB_DATA_TYPE_UINT32: {
            Local<Number> value = Nan::New(list->entry_data.uint32);
            list.next();
            return scope.Escape(value);
        }
        case MMDB_DATA_TYPE_BOOLEAN: {
            Local<Boolean> value = Nan::New(list->entry_data.boolean);
            list.next();
            return scope.Escape(value);
        }
        case MMDB_DATA_TYPE_UINT64: {
            Local<BigInt> value = BigInt::NewFromUnsigned(Isolate::GetCurrent(), list->entry_data.uint64);
            list.next();
            return scope.Escape(value);
        }
        case MMDB_DATA_TYPE_UINT128: {
#if MMDB_UINT128_IS_BYTE_ARRAY
            // TODO: Test this code w/ different endianess, etc
            uint64_t high = MmdbHelpers::Uint8ArrayToUint64(list->entry_data.uint128 + 8);
            uint64_t low = MmdbHelpers::Uint8ArrayToUint64(list->entry_data.uint128);
#else
            uint64_t high = static_cast<uint64_t>(list->entry_data.uint128 >> 64);
            uint64_t low = static_cast<uint64_t>(list->entry_data.uint128);
#endif
            uint64_t words[] = {low, high};
            Local<BigInt> value = BigInt::NewFromWords(Nan::GetCurrentContext(), 0, 2, words).ToLocalChecked();
            list.next();
            return scope.Escape(value);
        }
        case MMDB_DATA_TYPE_INT32: {
            Local<Number> value = Nan::New(list->entry_data.int32);
            list.next();
            return scope.Escape(value);
        }
        default: {
            *status = MMDB_INVALID_DATA_ERROR;
            return scope.Escape(Nan::Null());
        }
    }
}

}  // namespace node_libmaxminddb
