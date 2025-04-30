cmake_minimum_required(VERSION 3.9...3.24 FATAL_ERROR)

project(solanaceae)

add_executable(solanaceae_file2_zstd_test
	./test_file_zstd.cpp
)

target_link_libraries(solanaceae_file2_zstd_test PUBLIC
	solanaceae_file2_zstd
)

add_test(NAME solanaceae_file2_zstd_test COMMAND solanaceae_file2_zstd_test)

########################################

add_executable(solanaceae_backend_fs_atomic_test
	./test_backend_fs_atomic.cpp
)

target_link_libraries(solanaceae_backend_fs_atomic_test PUBLIC
	solanaceae_object_store_backend_filesystem
)

add_test(NAME solanaceae_backend_fs_atomic_test COMMAND solanaceae_backend_fs_atomic_test)
