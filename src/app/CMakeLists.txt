qt_add_executable(phonebook
    mainwindow.ui
    mainwindow.cpp
    insertform.ui
    insertform.cpp
    editform.ui
    editform.cpp
    main.cpp
)

target_link_libraries(phonebook PRIVATE
    corelib
    Qt6::Widgets
    SQLite::SQLite3
)

set_target_properties(phonebook PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

add_custom_command(TARGET phonebook POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:phonebook> ${PROJECT_BINARY_DIR}/phonebook
    COMMENT "Created ${PROJECT_BINARY_DIR}/phonebook"
)