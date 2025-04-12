add_library(threading
    SymbolThreadPool.cpp
    SymbolThreadPool.hpp
)

target_include_directories(threading PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
