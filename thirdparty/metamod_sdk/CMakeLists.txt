add_sources(
    CMakeLists.txt
    commands_meta.h
    conf_meta.h
    dllapi.h
    engine_api.h
    engine_t.h
    enginecallbacks.h
    engineinfo.h
    game_support.h
    games.h
    h_export.h
    info_name.h
    linkent.h
    log_meta.h
    meta_api.h
    meta_eiface.h
    metamod.h
    mhook.h
    mlist.h
    mplayer.h
    mplugin.h
    mqueue.h
    mreg.h
    mutil.h
    osdep.h
    plinfo.h
    reg_support.h
    sdk_util.h
    studioapi.h
    support_meta.h
    thread_logparse.h
    tqueue.h
    types_meta.h
    vdate.h
    vers_meta.h

    Dummy.cpp
)

preprocess_sources()
add_library( metamod_sdk STATIC ${PREP_SRCS} )
check_winxp_support( metamod_sdk )

target_include_directories( metamod_sdk PUBLIC
    # TODO: Move into a subdir
    .
)

target_link_libraries( metamod_sdk PUBLIC
    hlsdk
)

create_source_groups( "${CMAKE_SOURCE_DIR}" )
clear_sources()
