execute_process(COMMAND ${RPMBUILD_EXECUTABLE} -E %{?dist}
                OUTPUT_VARIABLE DIST_TAG
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE)

set(whitespaces_ "[\t\n\r ]*")

# since we have no control over dist tag value we should try to escape
# the content for use as a regular expression
string(REPLACE "+" "\\+" DIST_TAG "${DIST_TAG}")
string(REPLACE "." "\\." DIST_TAG "${DIST_TAG}")

getPackageInfo("${FOUND_FILE_1}" "FILE_INFO_")
if(NOT FILE_INFO_ MATCHES ".*Release${whitespaces_}:${whitespaces_}1${DIST_TAG}")
  message(FATAL_ERROR "Unexpected Release in '${FOUND_FILE_1}'; file info: '${FILE_INFO_}'")
endif()
