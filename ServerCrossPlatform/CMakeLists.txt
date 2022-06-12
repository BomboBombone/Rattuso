# CMakeList.txt: progetto CMake per ServerCrossPlatform. Includere l'origine e definire
# qui la logica specifica del progetto.
#
cmake_minimum_required (VERSION 3.8)

# Aggiungere l'origine all'eseguibile di questo progetto.
aux_source_directory(. DIR_SRCS)

file(GLOB_RECURSE HEADERS
    "./*.h"
)
find_package (Threads)

add_executable(ServerCrossPlatform ${DIR_SRCS} ${HEADERS}) 

target_link_libraries (ServerCrossPlatform ${CMAKE_THREAD_LIBS_INIT})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ServerCrossPlatform PROPERTY CXX_STANDARD 20)
endif()

# TODO: Aggiungere i test e, se necessario, installare le destinazioni.
