# 
# Internal file for GetGitRevisionDescription.cmake
#
# Requires CMake 2.6 or newer (uses the 'function' command)
#
# Original Author:
# 2009-2010 Ryan Pavlik <rpavlik@iastate.edu> <abiryan@ryand.net>
# http://academic.cleardefinition.com
# Iowa State University HCI Graduate Program/VRAC
#
# Copyright Iowa State University 2009-2010.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(HEAD_HASH)

file(READ "/home/cyber/rdp/FreeRDP/CMakeFiles/git-data/HEAD" HEAD_CONTENTS LIMIT 1024)

string(STRIP "${HEAD_CONTENTS}" HEAD_CONTENTS)
if(HEAD_CONTENTS MATCHES "ref")
	# named branch
	string(REPLACE "ref: " "" HEAD_REF "${HEAD_CONTENTS}")
	if(EXISTS "/home/cyber/rdp/FreeRDP/.git/${HEAD_REF}")
		configure_file("/home/cyber/rdp/FreeRDP/.git/${HEAD_REF}" "/home/cyber/rdp/FreeRDP/CMakeFiles/git-data/head-ref" COPYONLY)
	elseif(EXISTS "/home/cyber/rdp/FreeRDP/.git/logs/${HEAD_REF}")
		configure_file("/home/cyber/rdp/FreeRDP/.git/logs/${HEAD_REF}" "/home/cyber/rdp/FreeRDP/CMakeFiles/git-data/head-ref" COPYONLY)
		set(HEAD_HASH "${HEAD_REF}")
	endif()
else()
	# detached HEAD
	configure_file("/home/cyber/rdp/FreeRDP/.git/HEAD" "/home/cyber/rdp/FreeRDP/CMakeFiles/git-data/head-ref" COPYONLY)
endif()

if(NOT HEAD_HASH)
	file(READ "/home/cyber/rdp/FreeRDP/CMakeFiles/git-data/head-ref" HEAD_HASH LIMIT 1024)
	string(STRIP "${HEAD_HASH}" HEAD_HASH)
endif()
