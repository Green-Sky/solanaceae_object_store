#pragma once

#include "../types.hpp"
#include "../object_store.hpp"

#include <string>

namespace Backends {

// TODO: rename to atomic filesystem store?
// provides meta and atomic read/write on your filesystem
struct FilesystemStorageAtomic : public StorageBackendIMeta, public StorageBackendIAtomic {
	ObjectStore2& _os;

	FilesystemStorageAtomic(
		ObjectStore2& os,
		std::string_view storage_path = "test_obj_store",
		MetaFileType mft_new = MetaFileType::BINARY_MSGPACK
	);
	~FilesystemStorageAtomic(void);

	// TODO: fix the path for this specific fs?
	// for now we assume a single storage path per backend (there can be multiple per type)
	std::string _storage_path;

	// meta file type for new objects
	MetaFileType _mft_new {MetaFileType::BINARY_MSGPACK};

	ObjectHandle newObject(ByteSpan id, bool throw_construct = true) override;

	bool write(Object o, std::function<write_to_storage_fetch_data_cb>& data_cb) override;
	bool read(Object o, std::function<read_from_storage_put_data_cb>& data_cb) override;

	void scanAsync(void);

	private:
		size_t scanPath(std::string_view path);
		void scanPathAsync(std::string path);
};

} // Backends

