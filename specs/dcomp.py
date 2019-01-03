##########################################################################
#
# Copyright 2009-2016 VMware, Inc.
# All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
##########################################################################/


from .dxgi import *
from .d2d1 import D2D_MATRIX_3X2_F, D2D_RECT_F
from .d3d9types import D3DMATRIX
from .d2d1 import D2D1_COMPOSITE_MODE

DCOMPOSITION_BITMAP_INTERPOLATION_MODE = Enum('DCOMPOSITION_BITMAP_INTERPOLATION_MODE', [
    'DCOMPOSITION_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR',
    'DCOMPOSITION_BITMAP_INTERPOLATION_MODE_LINEAR',
    'DCOMPOSITION_BITMAP_INTERPOLATION_MODE_INHERIT',
])

DCOMPOSITION_BORDER_MODE = Enum('DCOMPOSITION_BORDER_MODE', [
    'DCOMPOSITION_BORDER_MODE_SOFT',
    'DCOMPOSITION_BORDER_MODE_HARD',
    'DCOMPOSITION_BORDER_MODE_INHERIT',
])

DCOMPOSITION_COMPOSITE_MODE = Enum('DCOMPOSITION_COMPOSITE_MODE', [
    'DCOMPOSITION_COMPOSITE_MODE_SOURCE_OVER',
    'DCOMPOSITION_COMPOSITE_MODE_DESTINATION_INVERT',
    'DCOMPOSITION_COMPOSITE_MODE_INHERIT',
])

DCOMPOSITION_FRAME_STATISTICS = Struct('DCOMPOSITION_FRAME_STATISTICS', [
    (LARGE_INTEGER, 'lastFrameTime'),
    (DXGI_RATIONAL, 'currentCompositionRate'),
    (LARGE_INTEGER, 'currentTime'),
    (LARGE_INTEGER, 'timeFrequency'),
    (LARGE_INTEGER, 'nextEstimatedFrameTime'),
])

COMPOSITIONOBJECT = Flags(DWORD, [
    'COMPOSITIONOBJECT_ALL_ACCESS',  # 0x0003
    'COMPOSITIONOBJECT_READ',        # 0x0001
    'COMPOSITIONOBJECT_WRITE',       # 0x0002
])

IDCompositionAnimation = Interface('IDCompositionAnimation', IUnknown)
IDCompositionDevice = Interface('IDCompositionDevice', IUnknown)
IDCompositionTarget = Interface('IDCompositionTarget', IUnknown)
IDCompositionVisual = Interface('IDCompositionVisual', IUnknown)
IDCompositionEffect = Interface('IDCompositionEffect', IUnknown)
IDCompositionEffectGroup = Interface('IDCompositionEffectGroup', IDCompositionEffect)
IDCompositionTransform3D = Interface('IDCompositionTransform3D', IDCompositionEffect)
IDCompositionTransform = Interface('IDCompositionTransform', IDCompositionTransform3D)
IDCompositionTranslateTransform = Interface('IDCompositionTranslateTransform', IDCompositionTransform)
IDCompositionTranslateTransform3D = Interface('IDCompositionTranslateTransform3D', IDCompositionTransform3D)
IDCompositionScaleTransform = Interface('IDCompositionScaleTransform', IDCompositionTransform)
IDCompositionScaleTransform3D = Interface('IDCompositionScaleTransform3D', IDCompositionTransform3D)
IDCompositionRotateTransform = Interface('IDCompositionRotateTransform', IDCompositionTransform)
IDCompositionRotateTransform3D = Interface('IDCompositionRotateTransform3D', IDCompositionTransform3D)
IDCompositionSkewTransform = Interface('IDCompositionSkewTransform', IDCompositionTransform)
IDCompositionMatrixTransform = Interface('IDCompositionMatrixTransform', IDCompositionTransform)
IDCompositionMatrixTransform3D = Interface('IDCompositionMatrixTransform3D', IDCompositionTransform3D)
IDCompositionClip = Interface('IDCompositionClip', IUnknown)
IDCompositionRectangleClip = Interface('IDCompositionRectangleClip', IDCompositionClip)
IDCompositionSurface = Interface('IDCompositionSurface', IUnknown)
IDCompositionVirtualSurface = Interface('IDCompositionVirtualSurface', IDCompositionSurface)

IDCompositionAnimation.methods += [
    StdMethod(HRESULT, 'Reset', []),
    StdMethod(HRESULT, 'SetAbsoluteBeginTime', [(LARGE_INTEGER, 'beginTime')]),
    StdMethod(HRESULT, 'AddCubic', [(Double, 'beginOffset'), (Float, 'constantCoefficient'), (Float, 'linearCoefficient'), (Float, 'quadraticCoefficient'), (Float, 'cubicCoefficient')]),
    StdMethod(HRESULT, 'AddSinusoidal', [(Double, 'beginOffset'), (Float, 'bias'), (Float, 'amplitude'), (Float, 'frequency'), (Float, 'phase')]),
    StdMethod(HRESULT, 'AddRepeat', [(Double, 'beginOffset'), (Double, 'durationToRepeat')]),
    StdMethod(HRESULT, 'End', [(Double, 'endOffset'), (Float, 'endValue')]),
]

IDCompositionDevice.methods += [
    StdMethod(HRESULT, 'Commit', []),
    StdMethod(HRESULT, 'WaitForCommitCompletion', []),
    StdMethod(HRESULT, 'GetFrameStatistics', [Out(Pointer(DCOMPOSITION_FRAME_STATISTICS), 'statistics')]),
    StdMethod(HRESULT, 'CreateTargetForHwnd', [(HWND, 'hwnd'), (BOOL, 'topmost'), Out(Pointer(ObjPointer(IDCompositionTarget)), 'target')]),
    StdMethod(HRESULT, 'CreateVisual', [Out(Pointer(ObjPointer(IDCompositionVisual)), 'visual')]),
    StdMethod(HRESULT, 'CreateSurface', [(UINT, 'width'), (UINT, 'height'), (DXGI_FORMAT, 'pixelFormat'), (DXGI_ALPHA_MODE, 'alphaMode'), Out(Pointer(ObjPointer(IDCompositionSurface)), 'surface')]),
    StdMethod(HRESULT, 'CreateVirtualSurface', [(UINT, 'initialWidth'), (UINT, 'initialHeight'), (DXGI_FORMAT, 'pixelFormat'), (DXGI_ALPHA_MODE, 'alphaMode'), Out(Pointer(ObjPointer(IDCompositionVirtualSurface)), 'virtualSurface')]),
    StdMethod(HRESULT, 'CreateSurfaceFromHandle', [(HANDLE, 'handle'), Out(Pointer(ObjPointer(IUnknown)), 'surface')]),
    StdMethod(HRESULT, 'CreateSurfaceFromHwnd', [(HWND, 'hwnd'), Out(Pointer(ObjPointer(IUnknown)), 'surface')]),
    StdMethod(HRESULT, 'CreateTranslateTransform', [Out(Pointer(ObjPointer(IDCompositionTranslateTransform)), 'translateTransform')]),
    StdMethod(HRESULT, 'CreateScaleTransform', [Out(Pointer(ObjPointer(IDCompositionScaleTransform)), 'scaleTransform')]),
    StdMethod(HRESULT, 'CreateRotateTransform', [Out(Pointer(ObjPointer(IDCompositionRotateTransform)), 'rotateTransform')]),
    StdMethod(HRESULT, 'CreateSkewTransform', [Out(Pointer(ObjPointer(IDCompositionSkewTransform)), 'skewTransform')]),
    StdMethod(HRESULT, 'CreateMatrixTransform', [Out(Pointer(ObjPointer(IDCompositionMatrixTransform)), 'matrixTransform')]),
    StdMethod(HRESULT, 'CreateTransformGroup', [(Array(ObjPointer(IDCompositionTransform), 'elements'), 'transforms'), (UINT, 'elements'), Out(Pointer(ObjPointer(IDCompositionTransform)), 'transformGroup')]),
    StdMethod(HRESULT, 'CreateTranslateTransform3D', [Out(Pointer(ObjPointer(IDCompositionTranslateTransform3D)), 'translateTransform3D')]),
    StdMethod(HRESULT, 'CreateScaleTransform3D', [Out(Pointer(ObjPointer(IDCompositionScaleTransform3D)), 'scaleTransform3D')]),
    StdMethod(HRESULT, 'CreateRotateTransform3D', [Out(Pointer(ObjPointer(IDCompositionRotateTransform3D)), 'rotateTransform3D')]),
    StdMethod(HRESULT, 'CreateMatrixTransform3D', [Out(Pointer(ObjPointer(IDCompositionMatrixTransform3D)), 'matrixTransform3D')]),
    StdMethod(HRESULT, 'CreateTransform3DGroup', [(Array(ObjPointer(IDCompositionTransform3D), 'elements'), 'transforms3D'), (UINT, 'elements'), Out(Pointer(ObjPointer(IDCompositionTransform3D)), 'transform3DGroup')]),
    StdMethod(HRESULT, 'CreateEffectGroup', [Out(Pointer(ObjPointer(IDCompositionEffectGroup)), 'effectGroup')]),
    StdMethod(HRESULT, 'CreateRectangleClip', [Out(Pointer(ObjPointer(IDCompositionRectangleClip)), 'clip')]),
    StdMethod(HRESULT, 'CreateAnimation', [Out(Pointer(ObjPointer(IDCompositionAnimation)), 'animation')]),
    StdMethod(HRESULT, 'CheckDeviceState', [Out(Pointer(BOOL), 'pfValid')]),
]

IDCompositionTarget.methods += [
    StdMethod(HRESULT, 'SetRoot', [(ObjPointer(IDCompositionVisual), 'visual')]),
]

IDCompositionVisual.methods += [
    StdMethod(HRESULT, 'SetOffsetX', [(Float, 'offsetX')], overloaded=True),
    StdMethod(HRESULT, 'SetOffsetX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetOffsetY', [(Float, 'offsetY')], overloaded=True),
    StdMethod(HRESULT, 'SetOffsetY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetTransform', [(Reference(Const(D2D_MATRIX_3X2_F)), 'matrix')], overloaded=True),
    StdMethod(HRESULT, 'SetTransform', [(ObjPointer(IDCompositionTransform), 'transform')], overloaded=True),
    StdMethod(HRESULT, 'SetTransformParent', [(ObjPointer(IDCompositionVisual), 'visual')]),
    StdMethod(HRESULT, 'SetEffect', [(ObjPointer(IDCompositionEffect), 'effect')]),
    StdMethod(HRESULT, 'SetBitmapInterpolationMode', [(DCOMPOSITION_BITMAP_INTERPOLATION_MODE, 'interpolationMode')]),
    StdMethod(HRESULT, 'SetBorderMode', [(DCOMPOSITION_BORDER_MODE, 'borderMode')]),
    StdMethod(HRESULT, 'SetClip', [(Reference(Const(D2D_RECT_F)), 'rect')], overloaded=True),
    StdMethod(HRESULT, 'SetClip', [(ObjPointer(IDCompositionClip), 'clip')], overloaded=True),
    StdMethod(HRESULT, 'SetContent', [(ObjPointer(IUnknown), 'content')]),
    StdMethod(HRESULT, 'AddVisual', [(ObjPointer(IDCompositionVisual), 'visual'), (BOOL, 'insertAbove'), (ObjPointer(IDCompositionVisual), 'referenceVisual')]),
    StdMethod(HRESULT, 'RemoveVisual', [(ObjPointer(IDCompositionVisual), 'visual')]),
    StdMethod(HRESULT, 'RemoveAllVisuals', []),
    StdMethod(HRESULT, 'SetCompositeMode', [(DCOMPOSITION_COMPOSITE_MODE, 'compositeMode')]),
]

IDCompositionTransform.methods += [
]

IDCompositionTransform3D.methods += [
]

IDCompositionTranslateTransform.methods += [
    StdMethod(HRESULT, 'SetOffsetX', [(Float, 'offsetX')], overloaded=True),
    StdMethod(HRESULT, 'SetOffsetX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetOffsetY', [(Float, 'offsetY')], overloaded=True),
    StdMethod(HRESULT, 'SetOffsetY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
]

IDCompositionTranslateTransform3D.methods += [
    StdMethod(HRESULT, 'SetOffsetX', [(Float, 'offsetX')], overloaded=True),
    StdMethod(HRESULT, 'SetOffsetX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetOffsetY', [(Float, 'offsetY')], overloaded=True),
    StdMethod(HRESULT, 'SetOffsetY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetOffsetZ', [(Float, 'offsetZ')], overloaded=True),
    StdMethod(HRESULT, 'SetOffsetZ', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
]

IDCompositionScaleTransform.methods += [
    StdMethod(HRESULT, 'SetScaleX', [(Float, 'scaleX')], overloaded=True),
    StdMethod(HRESULT, 'SetScaleX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetScaleY', [(Float, 'scaleY')], overloaded=True),
    StdMethod(HRESULT, 'SetScaleY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterX', [(Float, 'centerX')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterY', [(Float, 'centerY')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
]

IDCompositionScaleTransform3D.methods += [
    StdMethod(HRESULT, 'SetScaleX', [(Float, 'scaleX')], overloaded=True),
    StdMethod(HRESULT, 'SetScaleX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetScaleY', [(Float, 'scaleY')], overloaded=True),
    StdMethod(HRESULT, 'SetScaleY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetScaleZ', [(Float, 'scaleZ')], overloaded=True),
    StdMethod(HRESULT, 'SetScaleZ', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterX', [(Float, 'centerX')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterY', [(Float, 'centerY')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterZ', [(Float, 'centerZ')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterZ', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
]

IDCompositionRotateTransform.methods += [
    StdMethod(HRESULT, 'SetAngle', [(Float, 'angle')], overloaded=True),
    StdMethod(HRESULT, 'SetAngle', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterX', [(Float, 'centerX')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterY', [(Float, 'centerY')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
]

IDCompositionRotateTransform3D.methods += [
    StdMethod(HRESULT, 'SetAngle', [(Float, 'angle')], overloaded=True),
    StdMethod(HRESULT, 'SetAngle', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetAxisX', [(Float, 'axisX')], overloaded=True),
    StdMethod(HRESULT, 'SetAxisX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetAxisY', [(Float, 'axisY')], overloaded=True),
    StdMethod(HRESULT, 'SetAxisY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetAxisZ', [(Float, 'axisZ')], overloaded=True),
    StdMethod(HRESULT, 'SetAxisZ', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterX', [(Float, 'centerX')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterY', [(Float, 'centerY')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterZ', [(Float, 'centerZ')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterZ', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
]

IDCompositionSkewTransform.methods += [
    StdMethod(HRESULT, 'SetAngleX', [(Float, 'angleX')], overloaded=True),
    StdMethod(HRESULT, 'SetAngleX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetAngleY', [(Float, 'angleY')], overloaded=True),
    StdMethod(HRESULT, 'SetAngleY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterX', [(Float, 'centerX')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterY', [(Float, 'centerY')], overloaded=True),
    StdMethod(HRESULT, 'SetCenterY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
]

IDCompositionMatrixTransform.methods += [
    StdMethod(HRESULT, 'SetMatrix', [(Reference(Const(D2D_MATRIX_3X2_F)), 'matrix')]),
    StdMethod(HRESULT, 'SetMatrixElement', [(Int, 'row'), (Int, 'column'), (Float, 'value')], overloaded=True),
    StdMethod(HRESULT, 'SetMatrixElement', [(Int, 'row'), (Int, 'column'), (ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
]

IDCompositionMatrixTransform3D.methods += [
    StdMethod(HRESULT, 'SetMatrix', [(Reference(Const(D3DMATRIX)), 'matrix')]),
    StdMethod(HRESULT, 'SetMatrixElement', [(Int, 'row'), (Int, 'column'), (Float, 'value')], overloaded=True),
    StdMethod(HRESULT, 'SetMatrixElement', [(Int, 'row'), (Int, 'column'), (ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
]

IDCompositionEffect.methods += [
]

IDCompositionEffectGroup.methods += [
    StdMethod(HRESULT, 'SetOpacity', [(Float, 'opacity')], overloaded=True),
    StdMethod(HRESULT, 'SetOpacity', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetTransform3D', [(ObjPointer(IDCompositionTransform3D), 'transform3D')]),
]

IDCompositionClip.methods += [
]

IDCompositionRectangleClip.methods += [
    StdMethod(HRESULT, 'SetLeft', [(Float, 'left')], overloaded=True),
    StdMethod(HRESULT, 'SetLeft', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetTop', [(Float, 'top')], overloaded=True),
    StdMethod(HRESULT, 'SetTop', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetRight', [(Float, 'right')], overloaded=True),
    StdMethod(HRESULT, 'SetRight', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetBottom', [(Float, 'bottom')], overloaded=True),
    StdMethod(HRESULT, 'SetBottom', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetTopLeftRadiusX', [(Float, 'radius')], overloaded=True),
    StdMethod(HRESULT, 'SetTopLeftRadiusX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetTopLeftRadiusY', [(Float, 'radius')], overloaded=True),
    StdMethod(HRESULT, 'SetTopLeftRadiusY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetTopRightRadiusX', [(Float, 'radius')], overloaded=True),
    StdMethod(HRESULT, 'SetTopRightRadiusX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetTopRightRadiusY', [(Float, 'radius')], overloaded=True),
    StdMethod(HRESULT, 'SetTopRightRadiusY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetBottomLeftRadiusX', [(Float, 'radius')], overloaded=True),
    StdMethod(HRESULT, 'SetBottomLeftRadiusX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetBottomLeftRadiusY', [(Float, 'radius')], overloaded=True),
    StdMethod(HRESULT, 'SetBottomLeftRadiusY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetBottomRightRadiusX', [(Float, 'radius')], overloaded=True),
    StdMethod(HRESULT, 'SetBottomRightRadiusX', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
    StdMethod(HRESULT, 'SetBottomRightRadiusY', [(Float, 'radius')], overloaded=True),
    StdMethod(HRESULT, 'SetBottomRightRadiusY', [(ObjPointer(IDCompositionAnimation), 'animation')], overloaded=True),
]

IDCompositionSurface.methods += [
    # XXX: riid might be ID2D1DeviceContext
    StdMethod(HRESULT, 'BeginDraw', [(Pointer(Const(RECT)), 'updateRect'), (REFIID, 'iid'), Out(Pointer(ObjPointer(Void)), 'updateObject'), Out(Pointer(POINT), 'updateOffset')]),
    StdMethod(HRESULT, 'EndDraw', []),
    StdMethod(HRESULT, 'SuspendDraw', []),
    StdMethod(HRESULT, 'ResumeDraw', []),
    StdMethod(HRESULT, 'Scroll', [(Pointer(Const(RECT)), 'scrollRect'), (Pointer(Const(RECT)), 'clipRect'), (Int, 'offsetX'), (Int, 'offsetY')]),
]

IDCompositionVirtualSurface.methods += [
    StdMethod(HRESULT, 'Resize', [(UINT, 'width'), (UINT, 'height')]),
    StdMethod(HRESULT, 'Trim', [(Array(Const(RECT), 'count'), 'rectangles'), (UINT, 'count')]),
]

dcomp = Module('dcomp')
dcomp.addFunctions([
    StdFunction(HRESULT, 'DCompositionCreateDevice', [(ObjPointer(IDXGIDevice), 'dxgiDevice'), (REFIID, 'iid'), Out(Pointer(ObjPointer(Void)), 'dcompositionDevice')]),
    StdFunction(HRESULT, 'DCompositionCreateSurfaceHandle', [(COMPOSITIONOBJECT, 'desiredAccess'), (Pointer(SECURITY_ATTRIBUTES), 'securityAttributes'), Out(Pointer(HANDLE), 'surfaceHandle')]),
])
dcomp.addInterfaces([
    IDCompositionDevice,
])
