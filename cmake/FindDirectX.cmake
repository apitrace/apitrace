# - try to find DirectX include dirs and libraries

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	find_path (D3D8_INCLUDE_DIR d3d8.h
		PATHS
			"$ENV{DXSDK_DIR}/Include"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Include"
		DOC "The directory where d3d8.h resides")

	find_path (D3DX8_INCLUDE_DIR d3dx8.h
		PATHS
			"$ENV{DXSDK_DIR}/Include"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Include"
		DOC "The directory where d3dx8.h resides")

	find_library (D3D8_LIBRARY d3d8.lib
		PATHS
			"$ENV{DXSDK_DIR}/Lib/x86"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x86"
		DOC "The directory where d3d8.lib resides")

	find_library (D3DX8_LIBRARY d3dx8.lib
		PATHS
			"$ENV{DXSDK_DIR}/Lib/x86"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x86"
		DOC "The directory where d3dx8.lib resides")

	if (D3D8_INCLUDE_DIR AND D3D8_LIBRARY)
		set (D3D8_FOUND 1)
		if (D3DX8_INCLUDE_DIR AND D3DX8_LIBRARY)
			set (D3DX8_FOUND 1)
		endif (D3DX8_INCLUDE_DIR AND D3DX8_LIBRARY)
	endif (D3D8_INCLUDE_DIR AND D3D8_LIBRARY)


	find_path (D3D9_INCLUDE_DIR d3d9.h
		PATHS
			"$ENV{DXSDK_DIR}/Include"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Include"
		DOC "The directory where d3d9.h resides")

	find_path (D3DX9_INCLUDE_DIR d3dx9.h
		PATHS
			"$ENV{DXSDK_DIR}/Include"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Include"
		DOC "The directory where d3dx9.h resides")

	find_library (D3D9_LIBRARY d3d9.lib
		PATHS
			"$ENV{DXSDK_DIR}/Lib/x86"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x86"
		DOC "The directory where d3d9.lib resides")

	find_library (D3DX9_LIBRARY d3dx9.lib
		PATHS
			"$ENV{DXSDK_DIR}/Lib/x86"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x86"
		DOC "The directory where d3dx9.lib resides")

	if (D3D9_INCLUDE_DIR AND D3D9_LIBRARY)
		set (D3D9_FOUND 1)
		if (D3DX9_INCLUDE_DIR AND D3DX9_LIBRARY)
			set (D3DX9_FOUND 1)
		endif (D3DX9_INCLUDE_DIR AND D3DX9_LIBRARY)
	endif (D3D9_INCLUDE_DIR AND D3D9_LIBRARY)


	find_path (D3D10_INCLUDE_DIR d3d10.h
		PATHS
			"$ENV{DXSDK_DIR}/Include"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Include"
		DOC "The directory where d3d10.h resides")

	find_path (D3DX10_INCLUDE_DIR d3dx10.h
		PATHS
			"$ENV{DXSDK_DIR}/Include"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Include"
		DOC "The directory where d3dx10.h resides")

	find_library (D3D10_LIBRARY d3d10.lib
		PATHS
			"$ENV{DXSDK_DIR}/Lib/x86"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x86"
		DOC "The directory where d3d10.lib resides")

	find_library (D3DX10_LIBRARY d3dx10.lib
		PATHS
			"$ENV{DXSDK_DIR}/Lib/x86"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x86"
		DOC "The directory where d3dx10.lib resides")

	if (D3D10_INCLUDE_DIR AND D3D10_LIBRARY)
		set (D3D10_FOUND 1)
		if (D3DX10_INCLUDE_DIR AND D3DX10_LIBRARY)
			set (D3DX10_FOUND 1)
		endif (D3DX10_INCLUDE_DIR AND D3DX10_LIBRARY)
	endif (D3D10_INCLUDE_DIR AND D3D10_LIBRARY)

endif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

mark_as_advanced (
	D3D8_FOUND
	D3DX8_FOUND
	D3D9_FOUND
	D3DX9_FOUND
	D3D10_FOUND
	D3DX10_FOUND
)
