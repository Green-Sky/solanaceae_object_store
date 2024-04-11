cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

include(FetchContent)

if (NOT TARGET solanaceae_util)
	FetchContent_Declare(solanaceae_util
		GIT_REPOSITORY https://github.com/Green-Sky/solanaceae_util.git
		GIT_TAG master
	)
	FetchContent_MakeAvailable(solanaceae_util)
endif()

if (NOT TARGET EnTT::EnTT)
	FetchContent_Declare(EnTT
		GIT_REPOSITORY https://github.com/skypjack/entt.git
		GIT_TAG v3.12.2
		EXCLUDE_FROM_ALL
	)
	FetchContent_MakeAvailable(EnTT)
endif()

if (NOT TARGET nlohmann_json::nlohmann_json)
	FetchContent_Declare(json
		URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
		URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d
		EXCLUDE_FROM_ALL
	)
	FetchContent_MakeAvailable(json)
endif()

#if (NOT TARGET solanaceae_plugin)
	#FetchContent_Declare(solanaceae_plugin
		#GIT_REPOSITORY https://github.com/Green-Sky/solanaceae_plugin.git
		#GIT_TAG master
	#)
	#FetchContent_MakeAvailable(solanaceae_plugin)
#endif()

