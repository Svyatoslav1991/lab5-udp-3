function(add_udp_test TARGET_NAME)
    add_executable(${TARGET_NAME}
        ${ARGN}
    )

    target_link_libraries(${TARGET_NAME}
        PRIVATE
            Qt${QT_VERSION_MAJOR}::Test
            Qt${QT_VERSION_MAJOR}::Widgets
            Qt${QT_VERSION_MAJOR}::Network
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

add_udp_test(test_udpclientcontroller
    test_udpclientcontroller.cpp
    ${CMAKE_SOURCE_DIR}/client/include/udpclientcontroller.h
    ${CMAKE_SOURCE_DIR}/client/src/udpclientcontroller.cpp
)

add_udp_test(test_client_mainwindow
    test_client_mainwindow.cpp

    ${CMAKE_SOURCE_DIR}/client/include/mainwindow.h
    ${CMAKE_SOURCE_DIR}/client/src/mainwindow.cpp
    ${CMAKE_SOURCE_DIR}/client/src/mainwindow.ui

    ${CMAKE_SOURCE_DIR}/client/include/udpclientcontroller.h
    ${CMAKE_SOURCE_DIR}/client/src/udpclientcontroller.cpp
)

add_udp_test(test_udpservercontroller
    test_udpservercontroller.cpp

    ${CMAKE_SOURCE_DIR}/server/include/udpservercontroller.h
    ${CMAKE_SOURCE_DIR}/server/src/udpservercontroller.cpp
)
