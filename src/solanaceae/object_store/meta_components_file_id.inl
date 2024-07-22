#pragma once

#include "./meta_components_file.hpp"

#include <entt/core/type_info.hpp>

// TODO: move more central
#define DEFINE_COMP_ID(x) \
template<> \
constexpr entt::id_type entt::type_hash<x>::value() noexcept { \
	using namespace entt::literals; \
	return #x##_hs; \
} \
template<> \
constexpr std::string_view entt::type_name<x>::value() noexcept { \
	return #x; \
}

// cross compile(r) stable ids

DEFINE_COMP_ID(ObjComp::F::TagLocalHaveAll)
DEFINE_COMP_ID(ObjComp::F::LocalHaveBitset)
//DEFINE_COMP_ID(ObjComp::F::RemoteHaveBitset)
DEFINE_COMP_ID(ObjComp::F::SingleInfo)
DEFINE_COMP_ID(ObjComp::F::SingleInfoLocal)
DEFINE_COMP_ID(ObjComp::F::Collection)
DEFINE_COMP_ID(ObjComp::F::CollectionInfo)
DEFINE_COMP_ID(ObjComp::F::CollectionInfoLocal)

DEFINE_COMP_ID(ObjComp::Ephemeral::File::DownloadPriority)
DEFINE_COMP_ID(ObjComp::Ephemeral::File::ReadHeadHint)
DEFINE_COMP_ID(ObjComp::Ephemeral::File::TransferStats)
//DEFINE_COMP_ID(ObjComp::Ephemeral::File::TransferStatsSeparated)

#undef DEFINE_COMP_ID

