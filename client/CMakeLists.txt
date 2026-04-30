set(CLIENT_SOURCES
    include/mainwindow.h
    include/udpclientcontroller.h

    src/main.cpp
    src/mainwindow.cpp
    src/udpclientcontroller.cpp
    src/mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(udp_client
        MANUAL_FINALIZATION
        ${CLIENT_SOURCES}
    )
else()
    add_executable(udp_client
        ${CLIENT_SOURCES}
    )
endif()

target_include_directories(udp_client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(udp_client
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
        udp_core
)

set_target_properties(udp_client PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_finalize_executable(udp_client)
endif()
