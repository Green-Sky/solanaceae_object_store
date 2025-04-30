#include <solanaceae/object_store/object_store.hpp>
#include <solanaceae/object_store/backends/filesystem_storage_atomic.hpp>

#include <solanaceae/object_store/meta_components.hpp>
#include <solanaceae/object_store/meta_components_file.hpp>

#include <filesystem>

#include <cassert>
#include <vector>

int main(void) {
	const auto temp_dir_str = (std::filesystem::temp_directory_path() / "test_obj_store_tests").u8string();

	assert(!std::filesystem::exists(temp_dir_str));

	// test1, create store with 2 objects
	{
		ObjectStore2 os;
		Backends::FilesystemStorageAtomic fsa{os, temp_dir_str};

		// empty, does nothing
		fsa.scanAsync();
		assert(!std::filesystem::exists(temp_dir_str));

		{ // o1
			// id1
			std::vector<uint8_t> id{0x00, 0x13, 0x37, 0x01};
			auto o = fsa.newObject(ByteSpan{id});
			std::vector<uint8_t> data{0x01, 0x01, 0x11, 0xf1, 0xae, 0x0b, 0x33};
			static_cast<StorageBackendIAtomic&>(fsa).write(o, ByteSpan{data});
		}

		{ // o2
			// id2
			std::vector<uint8_t> id{0x00, 0x13, 0x37, 0x02};
			auto o = fsa.newObject(ByteSpan{id});
			std::vector<uint8_t> data{0x11, 0x11, 0x11, 0xff, 0xff, 0xee, 0xee};
			static_cast<StorageBackendIAtomic&>(fsa).write(o, ByteSpan{data});
		}
	}

	assert(std::filesystem::exists(temp_dir_str));

	// test2, load store created in test1
	{
		ObjectStore2 os;

		Backends::FilesystemStorageAtomic fsa{os, temp_dir_str};

		fsa.scanAsync();

		assert(os.registry().storage<Object>().size() == 2);

		{ // o1
			std::vector<uint8_t> id{0x00, 0x13, 0x37, 0x01};
			auto o = os.getOneObjectByID(ByteSpan{id});
			assert(static_cast<bool>(o));

			std::vector<uint8_t> orig_data{0x01, 0x01, 0x11, 0xf1, 0xae, 0x0b, 0x33};
			// TODO: read and test content

			//static_cast<StorageBackendIAtomic&>(fsa).write(o, ByteSpan{data});
		}

		{ // o2
			std::vector<uint8_t> id{0x00, 0x13, 0x37, 0x02};
			auto o = os.getOneObjectByID(ByteSpan{id});
			assert(static_cast<bool>(o));
		}
	}

	std::filesystem::remove_all(temp_dir_str);

	return 0;
}
