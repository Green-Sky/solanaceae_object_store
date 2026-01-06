cmake_minimum_required(VERSION 3.18...3.24 FATAL_ERROR)

include(FetchContent)

if (NOT TARGET zstd::libzstd)
	find_package(zstd CONFIG GLOBAL QUIET)
endif()

if (NOT TARGET zstd::libzstd)
	# TODO: try find_package() first
	# TODO: try pkg-config next (will work on most distros)

	set(ZSTD_BUILD_STATIC ON)
	set(ZSTD_BUILD_SHARED OFF)
	set(ZSTD_BUILD_PROGRAMS OFF)
	set(ZSTD_BUILD_CONTRIB OFF)
	set(ZSTD_BUILD_TESTS OFF)
	FetchContent_Declare(zstd
		#URL "https://github.com/facebook/zstd/releases/download/v1.5.6/zstd-1.5.6.tar.gz"
		URL "https://github.com/facebook/zstd/releases/download/v1.5.7/zstd-1.5.7.tar.gz"
		URL_HASH SHA256=eb33e51f49a15e023950cd7825ca74a4a2b43db8354825ac24fc1b7ee09e6fa3
		DOWNLOAD_EXTRACT_TIMESTAMP TRUE
		SOURCE_SUBDIR build/cmake
		EXCLUDE_FROM_ALL
	)
	FetchContent_MakeAvailable(zstd)

	add_library(libzstd_tmp INTERFACE) # somehow zstd fkd this up
	target_include_directories(libzstd_tmp INTERFACE ${zstd_SOURCE_DIR}/lib/)
	target_link_libraries(libzstd_tmp INTERFACE libzstd_static)
	#TODO: new zstd also provides zstd::libzstd
	add_library(zstd::libzstd ALIAS libzstd_tmp)
endif()

