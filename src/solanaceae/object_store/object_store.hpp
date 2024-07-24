#pragma once

#include <solanaceae/util/event_provider.hpp>
#include <solanaceae/util/span.hpp>

#include "./fwd.hpp"

#include <entt/entity/registry.hpp>
#include <entt/entity/handle.hpp>

struct StorageBackendI {
	// OR or OS ?
	ObjectStore2& _os;

	StorageBackendI(ObjectStore2& os);

	// default impl fails, acting like a read only store
	virtual ObjectHandle newObject(ByteSpan id);

	// ========== write object to storage (atomic-ish) ==========
	using write_to_storage_fetch_data_cb = uint64_t(uint8_t* request_buffer, uint64_t buffer_size);
	// calls data_cb with a buffer to be filled in, cb returns actual count of data. if returned < max, its the last buffer.
	virtual bool write(Object o, std::function<write_to_storage_fetch_data_cb>& data_cb);
	bool write(Object o, const ByteSpan data);

	// ========== read object from storage (atomic-ish) ==========
	using read_from_storage_put_data_cb = void(const ByteSpan buffer);
	virtual bool read(Object o, std::function<read_from_storage_put_data_cb>& data_cb);

	// ========== File2 interop ==========
	enum FILE2_FLAGS : uint32_t {
		FILE2_NONE = 0u,

		FILE2_READ = 1u << 0,
		FILE2_WRITE = 1u << 1,

		// only relevant for backend implementing this (ideally all)
		FILE2_NO_COMP = 1u << 2,// dont do any de-/compression
		FILE2_NO_ENC = 1u << 3, // dont do any de-/encryption

		FILE2_RAW = FILE2_NO_COMP | FILE2_NO_ENC, // combined
	};
	// TODO: stronger requirements
	// the backend might decide to not support writing using file2, if it's eg. zstd compressed
	// backends might only support a single file2 instance per object!
	virtual std::unique_ptr<File2I> file2(Object o, FILE2_FLAGS flags); // default does nothing
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
	static constexpr const char* version {"3"};

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

