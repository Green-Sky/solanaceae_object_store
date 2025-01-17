#pragma once

#include <entt/entity/fwd.hpp>

// internal id
enum class Object : uint32_t {};
using ObjectRegistry = entt::basic_registry<Object>;
using ObjectHandle = entt::basic_handle<ObjectRegistry>;

// fwd
struct StorageBackendIMeta;
struct StorageBackendIAtomic;
struct StorageBackendIFile2;
struct ObjectStore2;
struct File2I;

