# CMAKE version of Ubuntu 16.04 is 3.5.
cmake_minimum_required(VERSION 3.5)
project(RECORD)

set(LINKLIBS)
set(COMPILE_OPTION)
set(SRC)
set(INCL)
  
if(NOT MSVC)
	list(APPEND LINKLIBS
		m
	)
endif()

list(APPEND INCL
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/../lib
	${CMAKE_CURRENT_LIST_DIR}/../lib/RtAudioWrapper
	${CMAKE_CURRENT_LIST_DIR}/../lib/WAV
)

list(APPEND COMPILE_OPTION
  -fopenmp
  -g
)

include(${CMAKE_CURRENT_LIST_DIR}/../lib/RtAudioWrapper/RtAudio.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../lib/RtAudioWrapper/Libsamplerate.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/../cmake/AudioProbe.cmake)

add_executable(${PROJECT_NAME} main.cpp ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE	${INCL}  )
target_link_libraries(${PROJECT_NAME} ${LINKLIBS})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTION} )
set_target_properties(${PROJECT_NAME} PROPERTIES  CXX_STANDARD 17 )

