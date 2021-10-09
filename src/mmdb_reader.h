#ifndef NODE_LIBMAXMINDDB_MMDB_READER_H_
#define NODE_LIBMAXMINDDB_MMDB_READER_H_

#include "common.h"
#include "util/mmdb_entry_data_list_holder.h"  // IWYU pragma: keep
#include "util/uv_locks.h"
#include "util/v8_helpers.h"

namespace node_libmaxminddb {

class MmdbReader : public Nan::ObjectWrap {
 public:
    static NAN_MODULE_INIT(Init);

    static v8::Local<v8::Object> NewInstance(MMDB_s *mmdb);

 protected:
    static NAN_METHOD(Open);

    static NAN_METHOD(Close);

    static NAN_METHOD(Lookup);

 private:
    V8_HELPERS_CONSTRUCTOR();

    explicit MmdbReader(MMDB_s *mmdb);
    virtual ~MmdbReader();

    // NOLINTNEXTLINE(runtime/references)
    static v8::Local<v8::Value> EntryDataListToValue(MmdbEntryDataListHolder &entry_data_list, int *status);

    UvSharedMutex mmdb_mutex_;
    MMDB_s *mmdb_;
};

}  // namespace node_libmaxminddb

#endif  // NODE_LIBMAXMINDDB_MMDB_READER_H_
