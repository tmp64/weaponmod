#!/bin/sh

	# Get old version information from appversion.h
	if [ -e ./wpnmod_appversion.h ]; then
		oldver=$(cat ./wpnmod_appversion.h | grep -i '#define APP_VERSION_STRD' | sed -e 's/#define APP_VERSION_STRD \(.*\)/\1/i')
		if [ $? -ne 0 ]; then
			oldver=""
		fi
		oldmod=$(cat ./wpnmod_appversion.h | grep -i '#define APP_VERSION_SPECIALBUILD' | sed -e 's/#define APP_VERSION_SPECIALBUILD \(.*\)/\1/i')
		if [ $? -ne 0 ]; then
			oldmod=""
		fi
	fi

	# Get major, minor and maitenance information from version.h
	major=$(cat ./include/wpnmod_version.h | grep -i '#define VERSION_MAJOR' | sed -e 's/#define VERSION_MAJOR \(.*\)/\1/i')
	if [ $? -ne 0 -o "$major" = "" ]; then
		major=0
	fi
	minor=$(cat ./include/wpnmod_version.h | grep -i '#define VERSION_MINOR' | sed -e 's/#define VERSION_MINOR \(.*\)/\1/i')
	if [ $? -ne 0 -o "$minor" = "" ]; then
		minor=0
	fi
	maintenance=$(cat ./include/wpnmod_version.h | grep -i '#define VERSION_MAITENANCE' | sed -e 's/#define VERSION_MAITENANCE \(.*\)/\1/i')
	if [ $? -ne 0 -o "$maintenance" = "" ]; then
		maintenance=
	fi

	# Get revision and modification status from local SVN copy
	revision=$(svnversion -c -n | sed -e 's/\([0-9]\+:\)\?\([0-9]\+\).*/\2/')
	if [ $? -ne 0 -o "$revision" = "" ]; then
		revision=0
	fi
	modifications=$(svnversion -c -n | sed -e 's/\([0-9]\+:\)\?[0-9]\+.*\(M\).*/\2/')
	if [ $? -eq 0 -a "$modifications" = "M" ]; then
		modifications='"'modified'"'
	else
		modifications=""
	fi

	# Construct version string
	if [ "$maitenance" = "" ]; then
		version='"'$major.$minor.$revision'"'
	else
		version='"'$major.$minor.$maitenance.$revision'"'
	fi

	if [ "$version" != "$oldver" -o "$modifications" != "$oldmod" ]; then
		echo Old version is $oldver $oldmod and new one is $version $modifications
		echo Going to update wpnmod_appversion.h

		echo '#ifndef __APPVERSION_H__' > wpnmod_appversion.h
		echo '#define __APPVERSION_H__' >> wpnmod_appversion.h
		echo '' >> wpnmod_appversion.h

		echo -n '#define APP_VERSION_STRD ' >> wpnmod_appversion.h
		echo $version >> wpnmod_appversion.h

		if [ "$modifications" != "" ]; then
			echo -n '#define APP_VERSION_SPECIALBUILD ' >> wpnmod_appversion.h
			echo $modifications >> wpnmod_appversion.h
			echo '#define APP_VERSION APP_VERSION_STRD " " APP_VERSION_SPECIALBUILD' >> wpnmod_appversion.h
		else
			echo '#define APP_VERSION APP_VERSION_STRD' >> wpnmod_appversion.h
		fi
		echo '' >> wpnmod_appversion.h

		echo '#endif //__APPVERSION_H__' >> wpnmod_appversion.h
	fi
