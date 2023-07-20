cmake_minimum_required(VERSION 3.13)

project(PHONEBOOK VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# I need sudo apt-get install libsqlite3-dev for this!!!
find_package(SQLite3 REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# this command is unknown, used the following 3 lines for it
# qt_standard_project_setup()
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(src/app)
add_subdirectory(src/core)
