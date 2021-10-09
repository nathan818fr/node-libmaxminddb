#ifndef NODE_LIBMAXMINDDB_UTIL_MMDB_ENTRY_DATA_LIST_HOLDER_H_
#define NODE_LIBMAXMINDDB_UTIL_MMDB_ENTRY_DATA_LIST_HOLDER_H_

#include "../common.h"

namespace node_libmaxminddb {

class MmdbEntryDataListHolder {
 public:
    explicit MmdbEntryDataListHolder(MMDB_entry_data_list_s *handle) : handle_(handle) {}

    inline MMDB_entry_data_list_s *get() const noexcept {
        return handle_;
    }

    inline MMDB_entry_data_list_s operator*() const noexcept {
        return *get();
    }

    inline MMDB_entry_data_list_s *operator->() const noexcept {
        return get();
    }

    void next() {
        if (handle_) {
            handle_ = handle_->next;
        }
    }

 private:
    MMDB_entry_data_list_s *handle_;
};

inline bool operator==(const MmdbEntryDataListHolder &a, const MmdbEntryDataListHolder &b) {
    return a.get() == b.get();
}

inline bool operator==(const MmdbEntryDataListHolder &a, std::nullptr_t) {
    return a.get() == nullptr;
}

inline bool operator!=(const MmdbEntryDataListHolder &a, const MmdbEntryDataListHolder &b) {
    return a.get() == b.get();
}

inline bool operator!=(const MmdbEntryDataListHolder &a, std::nullptr_t) {
    return a.get() != nullptr;
}

}  // namespace node_libmaxminddb

#endif  // NODE_LIBMAXMINDDB_UTIL_MMDB_ENTRY_DATA_LIST_HOLDER_H_
