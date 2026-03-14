cmake_minimum_required(VERSION 3.20)

project(TrueGraphics VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT WIN32)
  message(FATAL_ERROR "TrueGraphics 1.0.0 currently targets Windows.")
endif()

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

set(TG_ENGINE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/engine/include")

set(TG_ENGINE_SOURCES
  engine/src/core/Engine.cpp
  engine/src/core/App.cpp
  engine/src/core/Config.cpp
  engine/src/core/Time.cpp
  engine/src/core/Uuid.cpp

  engine/src/system/Log.cpp
  engine/src/system/Profiler.cpp

  engine/src/window/Window.cpp
  engine/src/window/Input.cpp
  engine/src/window/Dpi.cpp

  engine/src/graphics/Renderer.cpp
  engine/src/graphics/Shader.cpp
  engine/src/graphics/Gpu.cpp
  engine/src/graphics/Image.cpp
  engine/src/graphics/Canvas3D.cpp

  engine/src/resource/Resources.cpp

  engine/src/layout/Layout.cpp
  engine/src/layout/FlexLayout.cpp

  engine/src/widgets/Widget.cpp
  engine/src/widgets/Button.cpp
  engine/src/widgets/Label.cpp
  engine/src/widgets/TextBox.cpp
  engine/src/widgets/Checkbox.cpp
  engine/src/widgets/RadioButton.cpp
  engine/src/widgets/ScrollView.cpp
  engine/src/widgets/Slider.cpp
  engine/src/widgets/ProgressBar.cpp
  engine/src/widgets/Dropdown.cpp
  engine/src/widgets/Tabs.cpp
  engine/src/widgets/ListView.cpp
  engine/src/widgets/Table.cpp
  engine/src/widgets/TreeView.cpp
  engine/src/widgets/PasswordBox.cpp
  engine/src/widgets/TextArea.cpp
  engine/src/widgets/Container.cpp
  engine/src/widgets/Canvas.cpp
  engine/src/widgets/Image.cpp

  engine/src/style/Style.cpp
  engine/src/style/StyleEngine.cpp
  engine/src/style/Theme.cpp

  engine/src/animation/Animator.cpp
  engine/src/animation/Easing.cpp

  engine/src/state/State.cpp

  engine/src/async/Async.cpp

  engine/src/plugin/Plugin.cpp
)

pybind11_add_module(_truegraphics
  bindings/module.cpp
  ${TG_ENGINE_SOURCES}
)

target_include_directories(_truegraphics PRIVATE
  "${TG_ENGINE_INCLUDE_DIR}"
)

target_compile_definitions(_truegraphics PRIVATE
  TG_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
  TG_VERSION_MINOR=${PROJECT_VERSION_MINOR}
  TG_VERSION_PATCH=${PROJECT_VERSION_PATCH}
  TG_VERSION_STRING="${PROJECT_VERSION}"
  NOMINMAX
  WIN32_LEAN_AND_MEAN
)

if(MSVC)
  target_compile_options(_truegraphics PRIVATE /W4 /EHsc /permissive-)
else()
  target_compile_options(_truegraphics PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(_truegraphics PRIVATE
  user32
  gdi32
  gdiplus
  comdlg32
  ole32
  uuid
)

install(TARGETS _truegraphics LIBRARY DESTINATION truegraphics RUNTIME DESTINATION truegraphics)
