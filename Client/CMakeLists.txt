cmake_minimum_required(VERSION 3.16)

project(ChatApp VERSION 0.1 LANGUAGES CXX)

# --- Qt Auto Features ---
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# --- C++ Standard ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find Qt Packages ---
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Network)

# --- Project Sources ---
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    login.cpp login.h login.ui

    net_client.h
    net_common.h
    net_connection.h
    net_message.h
    net_server.h
    net_tsqueue.h
    olc_net.h

    register.h register.cpp register.ui
    types.h
    client.h client.cpp
    pch.h

    clientdiscovery.h clientdiscovery.cpp
)

# --- Target Definition ---
qt_add_executable(ChatApp
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    connectserver.h connectserver.cpp connectserver.ui
)

target_precompile_headers(ChatApp PRIVATE pch.h)

# --- Include Directories ---
target_include_directories(ChatApp
    PUBLIC
        "C:/Users/hamda/OneDrive/Desktop/c++/Visual Studio C++/SDKs/asio-1.36.0/include"
)

# --- Compile Definitions ---
target_compile_definitions(ChatApp PRIVATE ASIO_STANDALONE)

# --- Link Libraries ---
target_link_libraries(ChatApp PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Network
    Ws2_32
)

# --- Finalize Qt6 Executable ---
qt_finalize_executable(ChatApp)
