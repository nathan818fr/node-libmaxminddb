#ifndef NODE_LIBMAXMINDDB_UTIL_UV_LOCKS_H_
#define NODE_LIBMAXMINDDB_UTIL_UV_LOCKS_H_

#include "../common.h"

namespace node_libmaxminddb {

class UvSharedMutex {
 public:
    UvSharedMutex() {
        if (uv_rwlock_init(&rwlock_)) {
            abort();
        }
    }

    ~UvSharedMutex() {
        uv_rwlock_destroy(&rwlock_);
    }

    void ReadLock() {
        uv_rwlock_rdlock(&rwlock_);
    }

    void ReadUnlock() {
        uv_rwlock_rdunlock(&rwlock_);
    }

    void WriteLock() {
        uv_rwlock_wrlock(&rwlock_);
    }

    void WriteUnlock() {
        uv_rwlock_wrunlock(&rwlock_);
    }

 private:
    NAN_DISALLOW_ASSIGN_COPY_MOVE(UvSharedMutex);

    mutable uv_rwlock_t rwlock_;
};

class UvReadLock {
 public:
    explicit UvReadLock(UvSharedMutex *mutex) : mutex_(mutex) {
        mutex_->ReadLock();
    }

    ~UvReadLock() {
        mutex_->ReadUnlock();
    }

 private:
    UvSharedMutex *mutex_;
};

class UvWriteLock {
 public:
    explicit UvWriteLock(UvSharedMutex *mutex) : mutex_(mutex) {
        mutex_->WriteLock();
    }

    ~UvWriteLock() {
        mutex_->WriteUnlock();
    }

 private:
    UvSharedMutex *mutex_;
};

}  // namespace node_libmaxminddb

#endif  // NODE_LIBMAXMINDDB_UTIL_UV_LOCKS_H_
