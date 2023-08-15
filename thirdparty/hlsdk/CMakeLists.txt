add_sources(
    CMakeLists.txt

    common/beamdef.h   
    common/cl_entity.h 
    common/com_model.h 
    common/const.h     
    common/con_nprint.h
    common/crc.h
    common/cvardef.h
    common/demo_api.h
    common/director_cmds.h
    common/dlight.h
    common/dll_state.h
    common/engine_launcher_api.h
    common/entity_state.h
    common/entity_types.h
    common/event_api.h
    common/event_args.h
    common/event_flags.h
    common/exefuncs.h
    common/hltv.h
    common/interface.h
    common/in_buttons.h
    common/ivoicetweak.h
    common/mathlib.h
    common/netadr.h
    common/net_api.h
    common/nowin.h
    common/particledef.h
    common/pmtrace.h
    common/qfont.h
    common/ref_params.h
    common/r_efx.h
    common/r_studioint.h
    common/screenfade.h
    common/studio_event.h
    common/triangleapi.h
    common/usercmd.h
    common/weaponinfo.h

    dlls/activity.h
    dlls/activitymap.h
    dlls/animation.h
    dlls/basemonster.h
    dlls/cbase.h
    dlls/cdll_dll.h
    dlls/client.h
    dlls/decals.h
    dlls/doors.h
    dlls/effects.h
    dlls/enginecallback.h
    dlls/explode.h
    dlls/extdll.h
    dlls/func_break.h
    dlls/game.h
    dlls/gamerules.h
    dlls/hornet.h
    dlls/items.h
    dlls/maprules.h
    dlls/monsterevent.h
    dlls/monsters.h
    dlls/nodes.h
    dlls/plane.h
    dlls/player.h
    dlls/saverestore.h
    dlls/schedule.h
    dlls/scriptevent.h
    dlls/skill.h
    dlls/soundent.h
    dlls/spectator.h
    dlls/talkmonster.h
    dlls/teamplay_gamerules.h
    dlls/trains.h
    dlls/util.h
    dlls/vector.h
    dlls/weapons.h

    engine/anorms.h      
    engine/archtypes.h   
    engine/cdll_int.h    
    engine/custom.h      
    engine/customentity.h
    engine/edict.h       
    engine/eiface.h      
    engine/keydefs.h     
    engine/progdefs.h    
    engine/progs.h       
    engine/Sequence.h    
    engine/shake.h       
    engine/studio.h

    pm_shared/pm_debug.h
    pm_shared/pm_defs.h
    pm_shared/pm_info.h
    pm_shared/pm_materials.h
    pm_shared/pm_movevars.h
    pm_shared/pm_shared.h

    Dummy.cpp
)

preprocess_sources()
add_library( hlsdk STATIC ${PREP_SRCS} )
check_winxp_support( hlsdk )

target_include_directories( hlsdk PUBLIC
    # TODO: Only leave common and .
    common
    dlls
    engine
    pm_shared
)

create_source_groups( "${CMAKE_SOURCE_DIR}" )
clear_sources()
