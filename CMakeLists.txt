cmake_minimum_required(VERSION 3.21)

########################################################################################################################
## Define project
########################################################################################################################

project(
	InfinityUI
	VERSION 2.1.3
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)

option(INFINITY_SHARED_BUILD "Use the workspace's pinned shared dependencies" OFF)
option(INFINITY_BUILD_TESTS "Build the standalone regression tests" OFF)
set(INFINITY_COMMONLIB_SOURCE_DIR "" CACHE PATH "Pinned CommonLibSSE-NG source checkout")
if(INFINITY_SHARED_BUILD)
    include(cmake/SharedBuild.cmake)
elseif(INFINITY_COMMONLIB_SOURCE_DIR)
    include(cmake/StandaloneBuild.cmake)
endif()

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
	${CMAKE_CURRENT_BINARY_DIR}/version.rc
	@ONLY
)

file(GLOB_RECURSE headers "include/*.h")
file(GLOB_RECURSE sources "source/*.cpp")

source_group(
	TREE
		${CMAKE_CURRENT_SOURCE_DIR}
	FILES
		${headers}
		${sources}
)

########################################################################################################################
## Configure target DLL
########################################################################################################################

if(NOT TARGET CommonLibSSE::CommonLibSSE)
    find_package(CommonLibSSE CONFIG REQUIRED)
endif()
find_package(spdlog REQUIRED CONFIG)

add_library(${PROJECT_NAME} SHARED ${headers} ${sources})
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
		CommonLibSSE::CommonLibSSE
		spdlog::spdlog
)

target_compile_definitions(
	${PROJECT_NAME}
	PRIVATE
		NOMINMAX
)

target_compile_options(
	${PROJECT_NAME}
	PRIVATE
		$<IF:$<OR:$<BOOL:${INFINITY_SHARED_BUILD}>,$<BOOL:${INFINITY_COMMONLIB_SOURCE_DIR}>>,/Z7,/Zi>
		/UENABLE_SKYRIM_VR
)

if (MSVC)
	target_link_options(
		${PROJECT_NAME}
		PRIVATE
			/DEBUG:FULL
			/PDBALTPATH:InfinityUI.pdb
			"$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
			"$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF>"
	)
endif()

target_precompile_headers(
	${PROJECT_NAME}
	PRIVATE
		include/pch.h
)

if(INFINITY_SHARED_BUILD)
    add_library(InfinityApiProbe OBJECT tests/api-probe.cpp source/Hooks.cpp
        source/GFxMoviePatcher.cpp source/RE/G/GFxValue.cpp source/utils/INISettingCollection.cpp)
    target_include_directories(InfinityApiProbe PRIVATE include
        "${INFINITY_TOOLCHAIN}/CommonLibSSE-NG-v12.2.0/include"
        "${INFINITY_TOOLCHAIN}/CommonLibSSE-NG-v12.2.0/extern/openvr/headers"
        "${INFINITY_TOOLCHAIN}/skyrim_1_7_104_compat/deps-msvc1951/include"
        "${INFINITY_TOOLCHAIN}/skyrim_1_7_104_compat/deps-msvc1951/include/directxtk"
        "${INFINITY_TOOLCHAIN}/vcpkg-installed/x64-windows-static-md/include"
        "${INFINITY_TOOLCHAIN}/ied-build-deps/xbyak"
        "${INFINITY_TOOLCHAIN}/vcpkg-installed/x64-windows-static-md/include/directxmath")
    target_compile_definitions(InfinityApiProbe PRIVATE ENABLE_SKYRIM_SE=1 ENABLE_SKYRIM_AE=1
        HAS_SKYRIM_MULTI_TARGETING=1 SKSE_SUPPORT_XBYAK=1 SPDLOG_FMT_EXTERNAL=1 SPDLOG_COMPILED_LIB=1
        NOMINMAX WINVER=0x0601 _WIN32_WINNT=0x0601)
    target_compile_options(InfinityApiProbe PRIVATE /Zc:preprocessor /wd4005)
    target_precompile_headers(InfinityApiProbe PRIVATE include/pch.h)
endif()
if(INFINITY_SHARED_BUILD OR INFINITY_BUILD_TESTS)
    if(NOT INFINITY_SHARED_BUILD AND INFINITY_COMMONLIB_SOURCE_DIR)
        add_library(InfinitySdkHeaderProbe OBJECT tests/dependency-probe.cpp)
        target_include_directories(InfinitySdkHeaderProbe PRIVATE $<TARGET_PROPERTY:CommonLibSSE,INCLUDE_DIRECTORIES>)
        target_compile_definitions(InfinitySdkHeaderProbe PRIVATE $<TARGET_PROPERTY:CommonLibSSE,COMPILE_DEFINITIONS>)
        target_link_libraries(InfinitySdkHeaderProbe PRIVATE spdlog::spdlog)
    endif()
    add_executable(InfinityPolicyTests tests/policy-tests.cpp)
    target_include_directories(InfinityPolicyTests PRIVATE include)
    if(INFINITY_SHARED_BUILD)
        target_include_directories(InfinityPolicyTests PRIVATE "${INFINITY_TOOLCHAIN}/ied-build-deps/xbyak")
    else()
        target_link_libraries(InfinityPolicyTests PRIVATE xbyak::xbyak)
    endif()
    target_compile_features(InfinityPolicyTests PRIVATE cxx_std_23)
    target_compile_definitions(InfinityPolicyTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    enable_testing()
    add_test(NAME InfinityPolicyTests COMMAND InfinityPolicyTests)
    add_executable(InfinityValueTests tests/value-tests.cpp source/RE/G/GFxValue.cpp)
    target_include_directories(InfinityValueTests PRIVATE include)
    target_compile_definitions(InfinityValueTests PRIVATE INFINITY_VALUE_TESTS=1 NOMINMAX)
    target_precompile_headers(InfinityValueTests PRIVATE include/pch.h)
    target_link_libraries(InfinityValueTests PRIVATE CommonLibSSE::CommonLibSSE)
    add_test(NAME InfinityValueTests COMMAND InfinityValueTests)
    add_executable(InfinityDependencyProbe tests/dependency-probe.cpp)
    target_link_libraries(InfinityDependencyProbe PRIVATE spdlog::spdlog)
    add_executable(InfinityAddressLibraryProbe tests/address-library-probe.cpp)
    target_compile_definitions(InfinityAddressLibraryProbe PRIVATE ENABLE_COMMONLIBSSE_TESTING=1)
    target_link_libraries(InfinityAddressLibraryProbe PRIVATE CommonLibSSE::CommonLibSSE)
endif()

target_sources(
	${PROJECT_NAME}
	INTERFACE
		"$<BUILD_INTERFACE:CommonLibSSE.natvis>"
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/version.rc
)
