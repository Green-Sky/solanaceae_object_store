#pragma once

#include <solanaceae/util/event_provider.hpp>
#include <solanaceae/util/span.hpp>

#include "./fwd.hpp"

#include <entt/entity/registry.hpp>
#include <entt/entity/handle.hpp>

// the different backend components
// depending on what a backend (or composed backends) provide
// different component (pointers) might be provided

// provides metadata storage(?)
struct StorageBackendIMeta {
	virtual ~StorageBackendIMeta(void) = default;
	virtual ObjectHandle newObject(ByteSpan id, bool throw_construct = true) = 0;
};

// provides atomic-ish read write
// simplest backend strategy to implement data encryption for
//
// collides with file backend and implementers providing both need to
// orchestrate access.
struct StorageBackendIAtomic {
	using write_to_storage_fetch_data_cb = uint64_t(uint8_t* request_buffer, uint64_t buffer_size);
	using read_from_storage_put_data_cb = void(const ByteSpan buffer);

	virtual ~StorageBackendIAtomic(void) = default;

	// calls data_cb with a buffer to be filled in, cb returns actual count of data. if returned < max, its the last buffer.

	// ========== write object to storage (atomic-ish) ==========
	virtual bool write(Object o, std::function<write_to_storage_fetch_data_cb>& data_cb) = 0;
	bool write(Object o, const ByteSpan data); // helper, calls cb variant internally

	// ========== read object from storage (atomic-ish) ==========
	virtual bool read(Object o, std::function<read_from_storage_put_data_cb>& data_cb) = 0;
};

struct StorageBackendIFile2 {
	virtual ~StorageBackendIFile2(void) = default;

	// ========== File2 interop ==========
	enum FILE2_FLAGS : uint32_t {
		FILE2_NONE = 0u,

		FILE2_READ = 1u << 0,
		FILE2_WRITE = 1u << 1,

		// only relevant for backends implementing this (ideally all)
		FILE2_NO_COMP = 1u << 2,// dont do any de-/compression
		FILE2_NO_ENC = 1u << 3, // dont do any de-/encryption

		FILE2_RAW = FILE2_NO_COMP | FILE2_NO_ENC, // combined
	};
	// TODO: stronger requirements
	// the backend might decide to not support writing using file2, if it's eg. zstd compressed
	// backends might only support a single file2 instance per object!
	virtual std::unique_ptr<File2I> file2(Object o, FILE2_FLAGS flags) = 0;
};

namespace ObjectStore::Events {

	struct ObjectConstruct {
		const ObjectHandle e;
	};
	struct ObjectUpdate {
		const ObjectHandle e;
	};
	struct ObjectDestory {
		const ObjectHandle e;
	};

} // ObjectStore::Events

enum class ObjectStore_Event : uint16_t {
	object_construct,
	object_update,
	object_destroy,

	MAX
};

struct ObjectStoreEventI {
	using enumType = ObjectStore_Event;

	virtual ~ObjectStoreEventI(void) {}

	virtual bool onEvent(const ObjectStore::Events::ObjectConstruct&) { return false; }
	virtual bool onEvent(const ObjectStore::Events::ObjectUpdate&) { return false; }
	virtual bool onEvent(const ObjectStore::Events::ObjectDestory&) { return false; }
};
using ObjectStoreEventProviderI = EventProviderI<ObjectStoreEventI>;

struct ObjectStore2 : public ObjectStoreEventProviderI {
	static constexpr const char* version {"4"};

	ObjectRegistry _reg;

	// TODO: default backend?

	ObjectStore2(void);
	virtual ~ObjectStore2(void);

	ObjectRegistry& registry(void);
	ObjectHandle objectHandle(const Object o);

	// TODO: properly think about multiple objects witht he same id / different backends
	ObjectHandle getOneObjectByID(const ByteSpan id);

	// sync?

	void throwEventConstruct(const Object o);
	void throwEventUpdate(const Object o);
	void throwEventDestroy(const Object o);
};

template<>
struct std::hash<ObjectHandle> {
	std::size_t operator()(ObjectHandle const& o) const noexcept {
		const std::size_t h1 = reinterpret_cast<std::size_t>(o.registry());
		const std::size_t h2 = entt::to_integral(o.entity());
		return (h1 << 3) ^ (h2 * 11400714819323198485llu);
	}
};

