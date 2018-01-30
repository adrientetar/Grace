cmake_minimum_required(VERSION 3.0)
set(CMAKE_C_COMPILER cl)

project(grace)
add_definitions("/std:c++17")

set(wxWidgets_ROOT_DIR $ENV{WXWIN})
set(wxWidgets_LIB_DIR $ENV{WXWIN}/lib/vc_x64_lib)
set(wxWidgets_CONFIGURATION mswud)

find_package(wxWidgets REQUIRED core base adv stc scintilla)
include(${wxWidgets_USE_FILE})

include_directories(src/grace src/grace/gproc)
file(GLOB sources src/grace/*.cpp src/grace/gproc/*.cpp)

add_executable(app WIN32 ${sources})
set_target_properties(app PROPERTIES LINKER_LANGUAGE CXX)

# and for each of your dependent executable/library targets:
target_link_libraries(app ${wxWidgets_LIBRARIES})
