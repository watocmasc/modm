cmake_minimum_required(VERSION 3.16)

project( modm VERSION 1.0.0 LANGUAGES CXX)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включение автоматической обработки файлов UI, MOC и RCC для Qt

set(CMAKE_OSX_ARCHITECTURES "arm64")

# Указываем исходные файлы проекта
set(SOURCE_FILES
    src/main.cpp
)

if(NOT CMAKE_GENERATOR)
    set(CMAKE_GENERATOR "Ninja")
endif()

# Создаём исполняемый файл
add_executable(modm
    ${SOURCE_FILES}
)

# Указываем путь к заголовочным файлам modbus (например, для Homebrew)
target_include_directories(modm PRIVATE
    /opt/homebrew/Cellar/libmodbus/3.1.11/include
)

# Указываем путь к библиотекам modbus
link_directories("/opt/homebrew/Cellar/libmodbus/3.1.11/lib")

# Линкуем с библиотекой modbus и компонентами Qt
target_link_libraries(modm PRIVATE 
    /opt/homebrew/Cellar/libmodbus/3.1.11/lib/libmodbus.dylib  # Указываем путь к библиотеке
)

# Специфичные настройки для macOS
if(APPLE)
    message(STATUS "Building for macOS")

    # set(MACOSX_BUNDLE_ICON_FILE "icon.icns")
    # set(MACOSX_BUNDLE_BUNDLE_NAME "CPS")
    # set(MACOSX_BUNDLE_BUNDLE_IDENTIFIER "com.example.cps")
    # set(MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist)

    # # Добавляем флаг MACOS_BUILD
    # target_compile_definitions(cps PRIVATE MACOS_BUILD)

    # # Устанавливаем параметры для macOS Bundle
    # set_target_properties(cps PROPERTIES
    #     MACOSX_BUNDLE TRUE
    #     MACOSX_BUNDLE_ICON_FILE "${MACOSX_BUNDLE_ICON_FILE}"
    #     MACOSX_BUNDLE_INFO_PLIST "${MACOSX_BUNDLE_INFO_PLIST}"
    # )

    # # Копируем иконку в .app
    # set(ICON_PATH "${CMAKE_SOURCE_DIR}/resources/logotypes/icon.icns")
    # if(EXISTS "${ICON_PATH}")
    #     add_custom_command(TARGET cps POST_BUILD
    #         COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ICON_PATH}" 
    #         "$<TARGET_BUNDLE_DIR:cps>/Contents/Resources/logotypes/icon.icns"
    #     )
    # endif()
endif()

# Настройки установки
include(GNUInstallDirs)
install(TARGETS modm
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
