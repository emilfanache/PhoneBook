find_package(SQLite3 REQUIRED)
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/core/contact.cpp"
                  "${CMAKE_SOURCE_DIR}/src/core/contact_builder.cpp"
                  "${CMAKE_SOURCE_DIR}/src/core/pb_operations.cpp")

qt_add_library(corelib STATIC ${SOURCES})
target_link_libraries(corelib PRIVATE SQLite::SQLite3 Qt6::Core)
target_include_directories(corelib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})