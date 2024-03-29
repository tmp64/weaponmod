add_sources(
    sdk/amxx_gameconfigs.h
    sdk/amxxmodule.cpp
    sdk/amxxmodule.h
    sdk/HLTypeConversion.h
    sdk/IGameConfigs.h
    sdk/ITextParsers.h
    sdk/moduleconfig.h
    
    appversion.h.in
    CMakeLists.txt
    CString.h
    weaponmod.rc
    wpnmod_config.cpp
    wpnmod_config.h
    wpnmod_entity.cpp
    wpnmod_entity.h
    wpnmod_grenade.cpp
    wpnmod_grenade.h
    wpnmod_hooker.cpp
    wpnmod_hooker.h
    wpnmod_hooks.cpp
    wpnmod_hooks.h
    wpnmod_items.cpp
    wpnmod_items.h
    wpnmod_log.cpp
    wpnmod_log.h
    wpnmod_main.cpp
    wpnmod_memory.cpp
    wpnmod_memory.h
    wpnmod_memory_engine.cpp
    wpnmod_memory_gamedll.cpp
    wpnmod_natives.cpp
    wpnmod_parse.cpp
    wpnmod_parse.h
    wpnmod_pvdata.cpp
    wpnmod_pvdata.h
    wpnmod_utils.cpp
    wpnmod_utils.h
    wpnmod_vtable.cpp
    wpnmod_vtable.h
)

preprocess_sources()
add_library( weaponmod_amxx SHARED ${PREP_SRCS} )
check_winxp_support( weaponmod_amxx )
add_auto_deploy( weaponmod_amxx PublishPath.txt )

target_include_directories( weaponmod_amxx PRIVATE
    .
)

target_link_libraries( weaponmod_amxx PRIVATE
    metamod_sdk
)

# Remove "lib" prefix
set_target_properties( weaponmod_amxx PROPERTIES PREFIX "" )

if( MSVC )
    # Export GiveFnptrsToDll
    set( GAME_LINKER_FLAGS_PLAT "/EXPORT:GiveFnptrsToDll=_GiveFnptrsToDll@8,@1" )
else()
    # Add _i386 suffix
    set_target_properties( weaponmod_amxx PROPERTIES OUTPUT_NAME "weaponmod_amxx_i386" )
endif()

set_target_properties( weaponmod_amxx PROPERTIES
	LINK_FLAGS "${GAME_LINKER_FLAGS} ${GAME_LINKER_FLAGS_PLAT}"
)

create_source_groups( "${CMAKE_SOURCE_DIR}" )
clear_sources()
