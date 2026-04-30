set(SERVER_SOURCES
    include/mainwindow.h
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(udp_server
        MANUAL_FINALIZATION
        ${SERVER_SOURCES}
    )
else()
    add_executable(udp_server
        ${SERVER_SOURCES}
    )
endif()

target_include_directories(udp_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(udp_server
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        udp_core
)

set_target_properties(udp_server PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_finalize_executable(udp_server)
endif()