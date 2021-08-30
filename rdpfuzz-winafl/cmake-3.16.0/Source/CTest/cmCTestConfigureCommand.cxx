/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#include "cmCTestConfigureCommand.h"

#include <cstring>
#include <sstream>
#include <vector>

#include "cm_static_string_view.hxx"

#include "cmCTest.h"
#include "cmCTestConfigureHandler.h"
#include "cmGlobalGenerator.h"
#include "cmMakefile.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"
#include "cmake.h"

void cmCTestConfigureCommand::BindArguments()
{
  this->cmCTestHandlerCommand::BindArguments();
  this->Bind("OPTIONS"_s, this->Options);
}

cmCTestGenericHandler* cmCTestConfigureCommand::InitializeHandler()
{
  std::vector<std::string> options;

  if (!this->Options.empty()) {
    cmExpandList(this->Options, options);
  }

  if (this->CTest->GetCTestConfiguration("BuildDirectory").empty()) {
    this->SetError(
      "Build directory not specified. Either use BUILD "
      "argument to CTEST_CONFIGURE command or set CTEST_BINARY_DIRECTORY "
      "variable");
    return nullptr;
  }

  const char* ctestConfigureCommand =
    this->Makefile->GetDefinition("CTEST_CONFIGURE_COMMAND");

  if (ctestConfigureCommand && *ctestConfigureCommand) {
    this->CTest->SetCTestConfiguration("ConfigureCommand",
                                       ctestConfigureCommand, this->Quiet);
  } else {
    const char* cmakeGeneratorName =
      this->Makefile->GetDefinition("CTEST_CMAKE_GENERATOR");
    if (cmakeGeneratorName && *cmakeGeneratorName) {
      const std::string& source_dir =
        this->CTest->GetCTestConfiguration("SourceDirectory");
      if (source_dir.empty()) {
        this->SetError(
          "Source directory not specified. Either use SOURCE "
          "argument to CTEST_CONFIGURE command or set CTEST_SOURCE_DIRECTORY "
          "variable");
        return nullptr;
      }

      const std::string cmakelists_file = source_dir + "/CMakeLists.txt";
      if (!cmSystemTools::FileExists(cmakelists_file)) {
        std::ostringstream e;
        e << "CMakeLists.txt file does not exist [" << cmakelists_file << "]";
        this->SetError(e.str());
        return nullptr;
      }

      bool multiConfig = false;
      bool cmakeBuildTypeInOptions = false;

      cmGlobalGenerator* gg =
        this->Makefile->GetCMakeInstance()->CreateGlobalGenerator(
          cmakeGeneratorName);
      if (gg) {
        multiConfig = gg->IsMultiConfig();
        delete gg;
      }

      std::string cmakeConfigureCommand =
        cmStrCat('"', cmSystemTools::GetCMakeCommand(), '"');

      for (std::string const& option : options) {
        cmakeConfigureCommand += " \"";
        cmakeConfigureCommand += option;
        cmakeConfigureCommand += "\"";

        if ((nullptr != strstr(option.c_str(), "CMAKE_BUILD_TYPE=")) ||
            (nullptr != strstr(option.c_str(), "CMAKE_BUILD_TYPE:STRING="))) {
          cmakeBuildTypeInOptions = true;
        }
      }

      if (!multiConfig && !cmakeBuildTypeInOptions &&
          !this->CTest->GetConfigType().empty()) {
        cmakeConfigureCommand += " \"-DCMAKE_BUILD_TYPE:STRING=";
        cmakeConfigureCommand += this->CTest->GetConfigType();
        cmakeConfigureCommand += "\"";
      }

      if (this->Makefile->IsOn("CTEST_USE_LAUNCHERS")) {
        cmakeConfigureCommand += " \"-DCTEST_USE_LAUNCHERS:BOOL=TRUE\"";
      }

      cmakeConfigureCommand += " \"-G";
      cmakeConfigureCommand += cmakeGeneratorName;
      cmakeConfigureCommand += "\"";

      const char* cmakeGeneratorPlatform =
        this->Makefile->GetDefinition("CTEST_CMAKE_GENERATOR_PLATFORM");
      if (cmakeGeneratorPlatform && *cmakeGeneratorPlatform) {
        cmakeConfigureCommand += " \"-A";
        cmakeConfigureCommand += cmakeGeneratorPlatform;
        cmakeConfigureCommand += "\"";
      }

      const char* cmakeGeneratorToolset =
        this->Makefile->GetDefinition("CTEST_CMAKE_GENERATOR_TOOLSET");
      if (cmakeGeneratorToolset && *cmakeGeneratorToolset) {
        cmakeConfigureCommand += " \"-T";
        cmakeConfigureCommand += cmakeGeneratorToolset;
        cmakeConfigureCommand += "\"";
      }

      cmakeConfigureCommand += " \"";
      cmakeConfigureCommand += source_dir;
      cmakeConfigureCommand += "\"";

      this->CTest->SetCTestConfiguration(
        "ConfigureCommand", cmakeConfigureCommand.c_str(), this->Quiet);
    } else {
      this->SetError(
        "Configure command is not specified. If this is a "
        "\"built with CMake\" project, set CTEST_CMAKE_GENERATOR. If not, "
        "set CTEST_CONFIGURE_COMMAND.");
      return nullptr;
    }
  }

  if (const char* labelsForSubprojects =
        this->Makefile->GetDefinition("CTEST_LABELS_FOR_SUBPROJECTS")) {
    this->CTest->SetCTestConfiguration("LabelsForSubprojects",
                                       labelsForSubprojects, this->Quiet);
  }

  cmCTestConfigureHandler* handler = this->CTest->GetConfigureHandler();
  handler->Initialize();
  handler->SetQuiet(this->Quiet);
  return handler;
}
