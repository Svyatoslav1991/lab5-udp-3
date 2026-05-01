set(CORE_SOURCES
    include/appsettings.h
    include/udpdatagraminfo.h

    src/appsettings.cpp
)

add_library(udp_core STATIC
    ${CORE_SOURCES}
)

target_include_directories(udp_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(udp_core
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
)
