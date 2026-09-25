set (HEADERS)

macro (fxc VS_PROFILE PS_PROFILE)
    set (VS_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/tri_${VS_PROFILE}.h)
    set (PS_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/tri_${PS_PROFILE}.h)

    add_custom_command (
        OUTPUT ${VS_HEADER}
        COMMAND ${DirectX_FXC_EXECUTABLE} /nologo /Qstrip_reflect /T ${VS_PROFILE} /E VS /Fh ${VS_HEADER} ${CMAKE_CURRENT_SOURCE_DIR}/tri.fx
    )

    add_custom_command (
        OUTPUT ${PS_HEADER}
        COMMAND ${DirectX_FXC_EXECUTABLE} /nologo /Qstrip_reflect /T ${PS_PROFILE} /E PS /Fh ${PS_HEADER} ${CMAKE_CURRENT_SOURCE_DIR}/tri.fx
    )

    set (HEADERS ${HEADERS} ${VS_HEADER} ${PS_HEADER})
endmacro ()

fxc (vs_2_0 ps_2_0)
fxc (vs_4_0 ps_4_0)
fxc (vs_4_0_level_9_1 ps_4_0_level_9_1)
fxc (vs_5_0 ps_5_0)

add_custom_target (hlsl DEPENDS ${HEADERS})
