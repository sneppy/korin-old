### Platform setup
if(WIN32)
	add_compile_definitions("PLATFORM_WINDOWS=1")
elseif(APPLE)
	add_compile_definitions("PLATFORM_APPLE=1")
elseif(UNIX)
	add_compile_definitions("PLATFORM_LINUX=1")
endif()