if (GLESV2_FOUND)
	add_subdirectory (gles2)
endif ()

if (OpenGL_FOUND)
	add_subdirectory (gl)
endif ()
