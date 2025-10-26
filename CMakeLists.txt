# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Pico_OLED C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Pico_OLED Pico_OLED.cpp
                QrCode/qrcodegen.cpp)

pico_set_program_name(Pico_OLED "Pico_OLED")
pico_set_program_version(Pico_OLED "0.1")

set_source_files_properties(QrCode/qrcodegen.cpp
    PROPERTIES COMPILE_FLAGS "-fexceptions -funwind-tables")

# << add: let main find qrcodegen.hpp if you include it from there
target_include_directories(Pico_OLED PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/QrCode
)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Pico_OLED 1)
pico_enable_stdio_usb(Pico_OLED 0)

# Add the standard library to the build
target_link_libraries(Pico_OLED
pico_stdlib
    hardware_i2c
    )

# Add the standard include files to the build
target_include_directories(Pico_OLED PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(Pico_OLED 
        hardware_spi
        hardware_i2c
        
        )

pico_add_extra_outputs(Pico_OLED)

set(U8G2_DIR ${CMAKE_CURRENT_LIST_DIR}/lib/u8g2/csrc)

target_include_directories(Pico_OLED PRIVATE ${U8G2_DIR})

set(U8G2_SRC
  ${U8G2_DIR}/u8g2_bitmap.c
  ${U8G2_DIR}/u8g2_box.c
  ${U8G2_DIR}/u8g2_buffer.c
  ${U8G2_DIR}/u8g2_circle.c
  ${U8G2_DIR}/u8g2_cleardisplay.c
  ${U8G2_DIR}/u8g2_d_memory.c
  ${U8G2_DIR}/u8g2_d_setup.c
  ${U8G2_DIR}/u8g2_font.c
  ${U8G2_DIR}/u8g2_fonts.c
  ${U8G2_DIR}/u8g2_hvline.c
  ${U8G2_DIR}/u8g2_input_value.c
  ${U8G2_DIR}/u8g2_intersection.c
  ${U8G2_DIR}/u8g2_kerning.c
  ${U8G2_DIR}/u8g2_line.c
  ${U8G2_DIR}/u8g2_ll_hvline.c
  ${U8G2_DIR}/u8g2_message.c
  ${U8G2_DIR}/u8g2_polygon.c
  ${U8G2_DIR}/u8g2_selection_list.c
  ${U8G2_DIR}/u8g2_setup.c
  ${U8G2_DIR}/u8x8_8x8.c
  ${U8G2_DIR}/u8x8_byte.c
  ${U8G2_DIR}/u8x8_cad.c
  ${U8G2_DIR}/u8x8_d_ssd1306_128x64_noname.c
  ${U8G2_DIR}/u8x8_display.c
  ${U8G2_DIR}/u8x8_fonts.c
  ${U8G2_DIR}/u8x8_gpio.c
  ${U8G2_DIR}/u8x8_setup.c
  ${U8G2_DIR}/u8x8_string.c
)
target_sources(Pico_OLED PRIVATE ${U8G2_SRC})

