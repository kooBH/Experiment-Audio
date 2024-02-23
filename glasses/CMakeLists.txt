# CMAKE version of Ubuntu 16.04 is 3.5.
cmake_minimum_required(VERSION 3.5)
project(Glasses_MEMS)

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
	.
	../lib
	../lib/RtAudioWrapper
	../lib/WAV
)

list(APPEND COMPILE_OPTION
  -fopenmp
  -g
)

include(../lib/RtAudioWrapper/RtAudio.cmake)
include(../lib/RtAudioWrapper/Libsamplerate.cmake)

include(../cmake/AudioProbe.cmake)

add_executable(${PROJECT_NAME} main.cpp ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE	${INCL}  )
target_link_libraries(${PROJECT_NAME} ${LINKLIBS})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTION} )
set_target_properties(${PROJECT_NAME} PROPERTIES  CXX_STANDARD 17 )

