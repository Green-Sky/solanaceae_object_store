cmake_minimum_required(VERSION 3.9...3.24 FATAL_ERROR)

project(solanaceae)

add_library(solanaceae_object_store
	./solanaceae/object_store/types.hpp
	./solanaceae/object_store/meta_components.hpp
	./solanaceae/object_store/meta_components_id.inl
	./solanaceae/object_store/meta_components_file.hpp
	./solanaceae/object_store/meta_components_file_id.inl
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

########################################

add_library(solanaceae_file2_zstd
	./solanaceae/file/file2_zstd.hpp
	./solanaceae/file/file2_zstd.cpp
)

target_include_directories(solanaceae_file2_zstd PUBLIC .)
target_compile_features(solanaceae_file2_zstd PUBLIC cxx_std_17)
target_link_libraries(solanaceae_file2_zstd PUBLIC
	solanaceae_file2
	zstd::zstd
)

########################################

add_library(solanaceae_object_store_backend_filesystem
	./solanaceae/object_store/backends/file2_stack.hpp
	./solanaceae/object_store/backends/file2_stack.cpp
	./solanaceae/object_store/backends/filesystem_storage.hpp
	./solanaceae/object_store/backends/filesystem_storage.cpp
)

target_include_directories(solanaceae_object_store_backend_filesystem PUBLIC .)
target_compile_features(solanaceae_object_store_backend_filesystem PUBLIC cxx_std_17)
target_link_libraries(solanaceae_object_store_backend_filesystem PUBLIC
	solanaceae_object_store
	solanaceae_file2_zstd
)

