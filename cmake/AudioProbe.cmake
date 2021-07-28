if(NOT MSVC)
	list(APPEND LINKLIBS
		m
	)
endif()



add_executable(AudioProbe  ../../src/AudioProbe.cpp ${SRC})
target_include_directories(AudioProbe 	PRIVATE	../lib/RtAudioWrapper ${INCL})
target_compile_options(AudioProbe PRIVATE ${COMPILE_OPTION} )
target_link_libraries(AudioProbe ${LINKLIBS})
set_target_properties(AudioProbe PROPERTIES  CXX_STANDARD 14 )

