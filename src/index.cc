#include "common.h"
#include "errors.h"
#include "mmdb_reader.h"
#include "util/v8_helpers.h"

using node_libmaxminddb::Errors;
using node_libmaxminddb::MmdbReader;
using node_libmaxminddb::V8Helpers::DefineReadOnlyProperty;
using node_libmaxminddb::V8Helpers::NewStr;
using node_libmaxminddb::V8Helpers::Set;

NAN_MODULE_INIT(InitModule);
NAN_METHOD(ImportFromNode);
NODE_MODULE(libmaxminddb, InitModule);

NAN_MODULE_INIT(InitModule) {
    Nan::HandleScope scope;

    Set(target, "_ImportFromNode", Nan::GetFunction(Nan::New<v8::FunctionTemplate>(ImportFromNode)).ToLocalChecked());
    MmdbReader::Init(target);

    DefineReadOnlyProperty(target, "MMDB_LIB_VERSION", NewStr(MMDB_lib_version()));

    NODE_DEFINE_CONSTANT(target, MMDB_FILE_OPEN_ERROR);
    NODE_DEFINE_CONSTANT(target, MMDB_CORRUPT_SEARCH_TREE_ERROR);
    NODE_DEFINE_CONSTANT(target, MMDB_INVALID_METADATA_ERROR);
    NODE_DEFINE_CONSTANT(target, MMDB_IO_ERROR);
    NODE_DEFINE_CONSTANT(target, MMDB_OUT_OF_MEMORY_ERROR);
    NODE_DEFINE_CONSTANT(target, MMDB_UNKNOWN_DATABASE_FORMAT_ERROR);
    NODE_DEFINE_CONSTANT(target, MMDB_INVALID_DATA_ERROR);
    NODE_DEFINE_CONSTANT(target, MMDB_INVALID_LOOKUP_PATH_ERROR);
    NODE_DEFINE_CONSTANT(target, MMDB_LOOKUP_PATH_DOES_NOT_MATCH_DATA_ERROR);
    NODE_DEFINE_CONSTANT(target, MMDB_INVALID_NODE_NUMBER_ERROR);
    NODE_DEFINE_CONSTANT(target, MMDB_IPV6_LOOKUP_IN_IPV4_DATABASE_ERROR);
}

NAN_METHOD(ImportFromNode) {
    Nan::HandleScope scope;

    V8_HELPERS_GETARG_OBJECT(errors, 0);

    Errors::ImportFromNode(errors);
}
