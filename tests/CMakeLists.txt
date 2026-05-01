function(add_udp_test TARGET_NAME)
    add_executable(${TARGET_NAME}
        ${ARGN}
    )

    target_link_libraries(${TARGET_NAME}
        PRIVATE
            Qt${QT_VERSION_MAJOR}::Test
            Qt${QT_VERSION_MAJOR}::Widgets
            udp_core
    )

    target_include_directories(${TARGET_NAME}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/core/include
            ${CMAKE_SOURCE_DIR}/client/include
            ${CMAKE_SOURCE_DIR}/server/include
    )

    add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
endfunction()

add_udp_test(test_appsettings
    test_appsettings.cpp
)
