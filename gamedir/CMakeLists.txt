set( SERVER_FILES
	addons/amxmodx/configs/weaponmod/spawnpoints/_my_map.ini
	addons/amxmodx/configs/weaponmod/weaponmod.ini
	addons/amxmodx/data/gamedata/weaponmod.games/master.games.txt
	addons/amxmodx/data/gamedata/weaponmod.games/valve/offsets-virtual-cbaseplayerammo.txt
	addons/amxmodx/data/gamedata/weaponmod.games/valve/offsets-virtual-cbaseplayeritem.txt
	addons/amxmodx/data/gamedata/weaponmod.games/valve/offsets-virtual-cbaseplayerweapon.txt
	addons/amxmodx/data/gamedata/weaponmod.games/valve/settings.txt
	addons/amxmodx/scripting/include/beams.inc
	addons/amxmodx/scripting/include/hl_wpnmod.inc
	addons/amxmodx/scripting/include/hl_wpnmod_compat.inc
	addons/amxmodx/scripting/include/hl_wpnmod_const.inc
)

function(bhl_install_files file_list)
	foreach( rel_file_path IN LISTS file_list)
		cmake_path( HAS_PARENT_PATH rel_file_path has_parent_path )

		if( has_parent_path)
			cmake_path( GET rel_file_path PARENT_PATH parent_path )
		else()
			set( parent_path "." )
		endif()

		# Copy the file as-is
		set( file_to_copy "${CMAKE_CURRENT_SOURCE_DIR}/${rel_file_path}" )

		install(
			FILES ${file_to_copy}
			DESTINATION ${parent_path}
		)
	endforeach()

	# Install readme
	install(
		FILES ${CMAKE_SOURCE_DIR}/README.md
		DESTINATION "."
		RENAME README_WeaponMod.md
	)
endfunction()


function(bhl_install_target target_name install_dir)
	install(
		FILES $<TARGET_FILE:${target_name}>
		DESTINATION ${install_dir}
	)

	if( CMAKE_CXX_LINKER_SUPPORTS_PDB )
		install(
			FILES $<TARGET_PDB_FILE:${target_name}>
			DESTINATION ${install_dir}
			OPTIONAL
		)
	endif()
endfunction()

bhl_install_files( "${SERVER_FILES}" )
bhl_install_target( weaponmod_amxx "addons/amxmodx/modules" )
