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