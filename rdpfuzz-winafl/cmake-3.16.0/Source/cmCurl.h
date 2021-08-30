/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmCurl_h
#define cmCurl_h

#include "cmConfigure.h" // IWYU pragma: keep

#include <string>

#include "cm_curl.h"

std::string cmCurlSetCAInfo(::CURL* curl, const char* cafile = nullptr);
std::string cmCurlSetNETRCOption(::CURL* curl, const std::string& netrc_level,
                                 const std::string& netrc_file);

#endif
