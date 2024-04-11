cmake_minimum_required(VERSION 3.9...3.24 FATAL_ERROR)

project(solanaceae)

add_library(solanaceae_object_store
	./solanaceae/object_store/types.hpp
	./solanaceae/object_store/meta_components.hpp
	./solanaceae/object_store/meta_components_id.inl
	./solanaceae/object_store/serializer_json.hpp
	./solanaceae/object_store/object_store.hpp
	./solanaceae/object_store/object_store.cpp
)

target_include_directories(solanaceae_object_store PUBLIC .)
target_compile_features(solanaceae_object_store PUBLIC cxx_std_17)
target_link_libraries(solanaceae_object_store PUBLIC
	nlohmann_json::nlohmann_json
	EnTT::EnTT
	solanaceae_util
)
