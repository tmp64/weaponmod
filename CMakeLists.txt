cmake_minimum_required(VERSION 3.20.0)

# Use static MSVC runtime to prevent multiple different CRTs being loaded
set( CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" )

set( CMAKE_POSITION_INDEPENDENT_CODE ON )
set( CMAKE_SKIP_BUILD_RPATH TRUE )

#-----------------------------------------------------------------
# CMake modules
#-----------------------------------------------------------------
list( APPEND CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_LIST_DIR}/cmake"
)

include(InputFilesList)
include(WinXPSupport)
include(GitVersionSemverfier)

project( WeaponMod VERSION "${GIT_MAJOR}.${GIT_MINOR}.${GIT_PATCH}" )
include( PlatformInfo )

#-----------------------------------------------------------------
# Build options
#-----------------------------------------------------------------
option( AUTO_DEPLOY "Whether to automatically deploy to deploy paths" )

#-----------------------------------------------------------------
# Compiler checks
#-----------------------------------------------------------------
if( NOT CMAKE_SIZEOF_VOID_P EQUAL 4 )
	if( NO_SIZEOF_VOID_P_CHECK )
		message( WARNING "You are not building for 32 bit systems. This is not supported." )
	else()
		set( PLAT_ERROR "" )
		
		if( CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU" )
			set( PLAT_ERROR "Did you forget CMAKE_TOOLCHAIN_FILE?" )
		elseif( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
			set( PLAT_ERROR "Are you generating for Win64?" )
		else()
			set( PLAT_ERROR "Check your compiler options." )
		endif()
		
		message( FATAL_ERROR "You are not building for 32 bit systems. ${PLAT_ERROR} Use NO_SIZEOF_VOID_P_CHECK to disable this error." )
	endif()
endif()

#-----------------------------------------------------------------
# Compiler and linker settings
#-----------------------------------------------------------------
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

set( C_CXX_COMPILER_FLAGS "" )	# Flags for C and C++ compilers
set( GAME_LINKER_FLAGS "" )		# Flags for Game DLL linker

if( COMPILER_GNU )
	
	#-----------------------------------------------------------------
	# Sanitizers
	#-----------------------------------------------------------------
	set( SANITIZERS "" )
	option( SANITIZE_ADDRESS "Enable AddressSanitizer" )
	option( SANITIZE_UNDEFINED "Enable UndefinedBehaviorSanitizer" )
	option( SANITIZE_MEMORY "Enable MemorySanitizer" )
	
	if( SANITIZE_ADDRESS )
		set( SANITIZERS "${SANITIZERS},address" )
	endif()
	
	if( SANITIZE_UNDEFINED )
		set( SANITIZERS "${SANITIZERS},undefined" )
	endif()

	if( SANITIZE_MEMORY )
		set( SANITIZERS "${SANITIZERS},memory" )
	endif()

	if( SANITIZERS )
		string( SUBSTRING "${SANITIZERS}" 1 -1 SANITIZERS )
		message( STATUS "Enabled sanitizers: ${SANITIZERS}" )
		set( C_CXX_COMPILER_FLAGS "${C_CXX_COMPILER_FLAGS} -fsanitize=${SANITIZERS}" )
		set( GAME_LINKER_FLAGS "${GAME_LINKER_FLAGS} -fsanitize=${SANITIZERS}" )
	endif()
	
	unset( SANITIZERS )
	
	#-----------------------------------------------------------------
	# Linker options
	#-----------------------------------------------------------------
	# From the Github 2013 Makefile, match the settings for Unix environments.
	# - OLD: Force 387 for FP math so the precision between win32 and linux and osx match
	# - Use SSE2 for FP math so Source SDK mathlib compiles correctly
	# - Trigger an error if any code tries to use an implicit return type
	# - Default visibility is hidden unless explicitly altered with __attribute__( visibility() )
	set( C_CXX_COMPILER_FLAGS "${C_CXX_COMPILER_FLAGS} -mfpmath=sse -msse2 -fvisibility=hidden" )
	set( GAME_LINKER_FLAGS "${GAME_LINKER_FLAGS} -mfpmath=sse -msse2 -Werror=return-type -fvisibility=hidden" )
	
	if( PLATFORM_MACOS )
		set( GAME_LINKER_FLAGS "${GAME_LINKER_FLAGS} -momit-leaf-frame-pointer -mtune=core2" )
	endif()
	
	#-----------------------------------------------------------------
	# Additional compiler options
	#-----------------------------------------------------------------
	# Link-Time Optimizations
	option( GNU_USE_LTO "GCC/Clang: Enable Link-Time Optimizations (LTO)" )
	if( GNU_USE_LTO )
		set( C_CXX_COMPILER_FLAGS       "${C_CXX_COMPILER_FLAGS} -flto" )
		set( GAME_LINKER_FLAGS "${GAME_LINKER_FLAGS} -flto" )
	endif()
	
	# Static link with libstdc++
	option( GNU_STATIC_LIBCPP "GCC/Clang: Link statically with libstdc++" )
	if( GNU_STATIC_LIBCPP )
		set( GAME_LINKER_FLAGS "${GAME_LINKER_FLAGS} -static-libstdc++" )
	endif()

	# Colored output
	option( GNU_FORCE_COLORED_OUTPUT "Always produce ANSI-colored output" )
	if ( GNU_FORCE_COLORED_OUTPUT )
		if ( COMPILER_GCC )
			add_compile_options (-fdiagnostics-color=always)
		elseif ( COMPILER_CLANG )
			add_compile_options (-fcolor-diagnostics)
		endif ()
	endif ()

	# Force function alignment.
	# When calling member funcitons, GCC checks if LSB bit is set.
	# If function just happens to have an odd address, GCC will process the address as
	# an offset into vtable. Which segfaults.
	set( C_CXX_COMPILER_FLAGS "${C_CXX_COMPILER_FLAGS} -falign-functions=16" )
	
elseif( COMPILER_MSVC )

	# Set Windows subsystem
	set( GAME_LINKER_FLAGS "${GAME_LINKER_FLAGS} /SUBSYSTEM:WINDOWS " )
	
	# Set target OS to WinXP for XP toolsets
	if( CMAKE_VS_PLATFORM_TOOLSET MATCHES ".*_xp$" )
		add_compile_definitions( _WIN32_WINNT=0x0501 )
	endif()
endif()

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_CXX_COMPILER_FLAGS}" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_CXX_COMPILER_FLAGS}" )

unset( C_CXX_COMPILER_FLAGS )

if (NOT HAS_STD_FILESYSTEM)
	if( PLATFORM_MACOS )
		# macOS 10.13 SDK doesn't have <filesystem>
		set( HAS_STD_FILESYSTEM FALSE )
		add_compile_definitions( HAS_STD_FILESYSTEM=0 )
	else()
		# MSVC 2017 and GCC 8+ support std::filesystem
		set( HAS_STD_FILESYSTEM TRUE )
		add_compile_definitions( HAS_STD_FILESYSTEM=1 )
	endif()
endif()

#-----------------------------------------------------------------
# Preprocessor definitions
#-----------------------------------------------------------------
# Add platform defines
add_compile_definitions(${PLATFORM_DEFINES})

if( COMPILER_GNU )

	add_compile_definitions(
		_stricmp=strcasecmp
		_strnicmp=strncasecmp
		_snprintf=snprintf
		_alloca=alloca
		_vsnprintf=vsnprintf
		_snwprintf=swprintf
	)
	
elseif( COMPILER_MSVC )

	# Disable "unsafe" warnings
	add_compile_definitions(
		_CRT_SECURE_NO_WARNINGS
		_SCL_SECURE_NO_WARNINGS
	)
	
endif()

if ( PLATFORM_LINUX )
	add_compile_definitions(linux __linux__ POSIX)
endif()

#-----------------------------------------------------------------
# Warning level
#-----------------------------------------------------------------

option( WARNINGS_ARE_ERRORS "Treat warnings as errors" OFF )

set( WARNING_LEVEL_C "" )
set( WARNING_LEVEL_CXX "" )
set( WARNING_LEVEL_C_CXX "" )

if( COMPILER_GNU )

	set( WARNING_LEVEL_C_CXX "${WARNING_LEVEL_C_CXX} -Wno-write-strings -Wno-unknown-pragmas -Wno-endif-labels" )
	set( WARNING_LEVEL_C_CXX "${WARNING_LEVEL_C_CXX} -Wno-switch -Wno-unused-variable" )
	set( WARNING_LEVEL_C_CXX "${WARNING_LEVEL_C_CXX} -Wno-unused-function -Wno-unused-value" )
	set( WARNING_LEVEL_C_CXX "${WARNING_LEVEL_C_CXX} -Wno-pointer-arith -Wno-unused-result" )
	set( WARNING_LEVEL_CXX "${WARNING_LEVEL_CXX} -Wno-delete-non-virtual-dtor -Wno-invalid-offsetof -Wno-conversion-null" )
	
	if( COMPILER_GCC )
		set( WARNING_LEVEL_C_CXX "${WARNING_LEVEL_C_CXX} -Wno-format-truncation -Wno-unused-but-set-variable" )
	elseif( COMPILER_CLANG )
		set( WARNING_LEVEL_C_CXX "${WARNING_LEVEL_C_CXX} -Wno-null-arithmetic" )
	endif()
	
	if( WARNINGS_ARE_ERRORS )
		set( WARNING_LEVEL_C_CXX "${WARNING_LEVEL_C_CXX} -Werror" )
	endif()
elseif( COMPILER_MSVC )
	if( WARNINGS_ARE_ERRORS )
		set( WARNING_LEVEL_C_CXX "${WARNING_LEVEL_C_CXX} /WX" )
	endif()
endif()

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_LEVEL_C} ${WARNING_LEVEL_C_CXX}" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_LEVEL_CXX} ${WARNING_LEVEL_C_CXX}" )

#-----------------------------------------------------------------
# appversion.h generator
#-----------------------------------------------------------------

# Generate version file
set( GENERATED_INCLUDE_DIR ${CMAKE_BINARY_DIR}/generated)
configure_file( "src/appversion.h.in" "${GENERATED_INCLUDE_DIR}/appversion.h" )
include_directories( ${GENERATED_INCLUDE_DIR} )

#-----------------------------------------------------------------
# Autodeploy macro
#-----------------------------------------------------------------
if( AUTO_DEPLOY )
	macro( add_auto_deploy TARGET_NAME PUBLISH_PATHS_FILE )
		if ( WIN32 )
			add_custom_command( TARGET ${TARGET_NAME}
				POST_BUILD
				COMMAND ${CMAKE_SOURCE_DIR}/scripts/deploy_libs.bat "${CMAKE_BINARY_DIR}\\${PUBLISH_PATHS_FILE}" "$<TARGET_FILE:${TARGET_NAME}>" "$<TARGET_PDB_FILE:${TARGET_NAME}>"
			)
		else()
			add_custom_command( TARGET ${TARGET_NAME}
				POST_BUILD
				COMMAND ${CMAKE_SOURCE_DIR}/scripts/deploy_libs.sh "${CMAKE_BINARY_DIR}/${PUBLISH_PATHS_FILE}" "$<TARGET_FILE:${TARGET_NAME}>"
			)
		endif()
	endmacro()
else()
	macro( add_auto_deploy )
	endmacro()
endif()

#-----------------------------------------------------------------
# Libraries
#-----------------------------------------------------------------
set( THREADS_PREFER_PTHREAD_FLAG ON )
find_package( Threads REQUIRED )

add_subdirectory( thirdparty )
add_subdirectory( src )
