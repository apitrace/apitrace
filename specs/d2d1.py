##########################################################################
#
# Copyright 2015 VMware, Inc
# Copyright 2011 Jose Fonseca
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


from winapi import *
from dxgi import DXGI_FORMAT, IDXGISurface
from dwrite import *



#
# D2D
#

HRESULT = MAKE_HRESULT(errors = [
    "D2DERR_UNSUPPORTED_PIXEL_FORMAT",
    "D2DERR_INSUFFICIENT_BUFFER",
    "D2DERR_WRONG_STATE",
    "D2DERR_NOT_INITIALIZED",
    "D2DERR_UNSUPPORTED_OPERATION",
    "D2DERR_SCANNER_FAILED",
    "D2DERR_SCREEN_ACCESS_DENIED",
    "D2DERR_DISPLAY_STATE_INVALID",
    "D2DERR_ZERO_VECTOR",
    "D2DERR_INTERNAL_ERROR",
    "D2DERR_DISPLAY_FORMAT_NOT_SUPPORTED",
    "D2DERR_INVALID_CALL",
    "D2DERR_NO_HARDWARE_DEVICE",
    "D2DERR_RECREATE_TARGET",
    "D2DERR_TOO_MANY_SHADER_ELEMENTS",
    "D2DERR_SHADER_COMPILE_FAILED",
    "D2DERR_MAX_TEXTURE_SIZE_EXCEEDED",
    "D2DERR_UNSUPPORTED_VERSION",
    "D2DERR_BAD_NUMBER",
    "D2DERR_WRONG_FACTORY",
    "D2DERR_LAYER_ALREADY_IN_USE",
    "D2DERR_POP_CALL_DID_NOT_MATCH_PUSH",
    "D2DERR_WRONG_RESOURCE_DOMAIN",
    "D2DERR_PUSH_POP_UNBALANCED",
    "D2DERR_RENDER_TARGET_HAS_LAYER_OR_CLIPRECT",
    "D2DERR_INCOMPATIBLE_BRUSH_TYPES",
    "D2DERR_WIN32_ERROR",
    "D2DERR_TARGET_NOT_GDI_COMPATIBLE",
    "D2DERR_TEXT_EFFECT_IS_WRONG_TYPE",
    "D2DERR_TEXT_RENDERER_NOT_RELEASED",
    "D2DERR_EXCEEDS_MAX_BITMAP_SIZE",
])

D3DCOLORVALUE = Struct("D3DCOLORVALUE", [
    (FLOAT, "r"),
    (FLOAT, "g"),
    (FLOAT, "b"),
    (FLOAT, "a"),
])

D2D_POINT_2U = Struct("D2D_POINT_2U", [
    (UINT32, "x"),
    (UINT32, "y"),
])

D2D_POINT_2F = Struct("D2D_POINT_2F", [
    (FLOAT, "x"),
    (FLOAT, "y"),
])

D2D_POINT_2L = Alias("D2D_POINT_2L", POINT)

D2D_VECTOR_2F = Struct("D2D_VECTOR_2F", [
    (FLOAT, "x"),
    (FLOAT, "y"),
])

D2D_VECTOR_3F = Struct("D2D_VECTOR_3F", [
    (FLOAT, "x"),
    (FLOAT, "y"),
    (FLOAT, "z"),
])

D2D_VECTOR_4F = Struct("D2D_VECTOR_4F", [
    (FLOAT, "x"),
    (FLOAT, "y"),
    (FLOAT, "z"),
    (FLOAT, "w"),
])

D2D_RECT_F = Struct("D2D_RECT_F", [
    (FLOAT, "left"),
    (FLOAT, "top"),
    (FLOAT, "right"),
    (FLOAT, "bottom"),
])

D2D_RECT_U = Struct("D2D_RECT_U", [
    (UINT32, "left"),
    (UINT32, "top"),
    (UINT32, "right"),
    (UINT32, "bottom"),
])

D2D_RECT_L = Alias("D2D_RECT_L", RECT)

D2D_SIZE_F = Struct("D2D_SIZE_F", [
    (FLOAT, "width"),
    (FLOAT, "height"),
])

D2D_SIZE_U = Struct("D2D_SIZE_U", [
    (UINT32, "width"),
    (UINT32, "height"),
])

D2D_COLOR_F = Alias("D2D_COLOR_F", D3DCOLORVALUE)
D2D_MATRIX_3X2_F = Struct("D2D_MATRIX_3X2_F", [
    (FLOAT, "_11"),
    (FLOAT, "_12"),
    (FLOAT, "_21"),
    (FLOAT, "_22"),
    (FLOAT, "_31"),
    (FLOAT, "_32"),
])

D2D_MATRIX_4X3_F = Struct("D2D_MATRIX_4X3_F", [
    (Array(Array(FLOAT, 3), 4), "m"),
])

D2D_MATRIX_4X4_F = Struct("D2D_MATRIX_4X4_F", [
    (Array(Array(FLOAT, 4), 4), "m"),
])

D2D_MATRIX_5X4_F = Struct("D2D_MATRIX_5X4_F", [
    (Array(Array(FLOAT, 4), 5), "m"),
])



#
# D2D1
#

ID2D1Resource = Interface("ID2D1Resource", IUnknown)
ID2D1Image = Interface("ID2D1Image", ID2D1Resource)
ID2D1Bitmap = Interface("ID2D1Bitmap", ID2D1Resource)
ID2D1GradientStopCollection = Interface("ID2D1GradientStopCollection", ID2D1Resource)
ID2D1Brush = Interface("ID2D1Brush", ID2D1Resource)
ID2D1BitmapBrush = Interface("ID2D1BitmapBrush", ID2D1Brush)
ID2D1SolidColorBrush = Interface("ID2D1SolidColorBrush", ID2D1Brush)
ID2D1LinearGradientBrush = Interface("ID2D1LinearGradientBrush", ID2D1Brush)
ID2D1RadialGradientBrush = Interface("ID2D1RadialGradientBrush", ID2D1Brush)
ID2D1StrokeStyle = Interface("ID2D1StrokeStyle", ID2D1Resource)
ID2D1Geometry = Interface("ID2D1Geometry", ID2D1Resource)
ID2D1RectangleGeometry = Interface("ID2D1RectangleGeometry", ID2D1Geometry)
ID2D1RoundedRectangleGeometry = Interface("ID2D1RoundedRectangleGeometry", ID2D1Geometry)
ID2D1EllipseGeometry = Interface("ID2D1EllipseGeometry", ID2D1Geometry)
ID2D1GeometryGroup = Interface("ID2D1GeometryGroup", ID2D1Geometry)
ID2D1TransformedGeometry = Interface("ID2D1TransformedGeometry", ID2D1Geometry)
ID2D1GeometrySink = Interface("ID2D1GeometrySink", ID2D1SimplifiedGeometrySink)
ID2D1TessellationSink = Interface("ID2D1TessellationSink", IUnknown)
ID2D1PathGeometry = Interface("ID2D1PathGeometry", ID2D1Geometry)
ID2D1Mesh = Interface("ID2D1Mesh", ID2D1Resource)
ID2D1Layer = Interface("ID2D1Layer", ID2D1Resource)
ID2D1DrawingStateBlock = Interface("ID2D1DrawingStateBlock", ID2D1Resource)
ID2D1RenderTarget = Interface("ID2D1RenderTarget", ID2D1Resource)
ID2D1BitmapRenderTarget = Interface("ID2D1BitmapRenderTarget", ID2D1RenderTarget)
ID2D1HwndRenderTarget = Interface("ID2D1HwndRenderTarget", ID2D1RenderTarget)
ID2D1GdiInteropRenderTarget = Interface("ID2D1GdiInteropRenderTarget", IUnknown)
ID2D1DCRenderTarget = Interface("ID2D1DCRenderTarget", ID2D1RenderTarget)
ID2D1Factory = Interface("ID2D1Factory", IUnknown)


D2D1_ALPHA_MODE = Enum("D2D1_ALPHA_MODE", [
    "D2D1_ALPHA_MODE_UNKNOWN",
    "D2D1_ALPHA_MODE_PREMULTIPLIED",
    "D2D1_ALPHA_MODE_STRAIGHT",
    "D2D1_ALPHA_MODE_IGNORE",
])

D2D1_GAMMA = Enum("D2D1_GAMMA", [
    "D2D1_GAMMA_2_2",
    "D2D1_GAMMA_1_0",
])

D2D1_OPACITY_MASK_CONTENT = Enum("D2D1_OPACITY_MASK_CONTENT", [
    "D2D1_OPACITY_MASK_CONTENT_GRAPHICS",
    "D2D1_OPACITY_MASK_CONTENT_TEXT_NATURAL",
    "D2D1_OPACITY_MASK_CONTENT_TEXT_GDI_COMPATIBLE",
])

D2D1_EXTEND_MODE = Enum("D2D1_EXTEND_MODE", [
    "D2D1_EXTEND_MODE_CLAMP",
    "D2D1_EXTEND_MODE_WRAP",
    "D2D1_EXTEND_MODE_MIRROR",
])

D2D1_ANTIALIAS_MODE = Enum("D2D1_ANTIALIAS_MODE", [
    "D2D1_ANTIALIAS_MODE_PER_PRIMITIVE",
    "D2D1_ANTIALIAS_MODE_ALIASED",
])

D2D1_TEXT_ANTIALIAS_MODE = Enum("D2D1_TEXT_ANTIALIAS_MODE", [
    "D2D1_TEXT_ANTIALIAS_MODE_DEFAULT",
    "D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE",
    "D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE",
    "D2D1_TEXT_ANTIALIAS_MODE_ALIASED",
])

D2D1_BITMAP_INTERPOLATION_MODE = Enum("D2D1_BITMAP_INTERPOLATION_MODE", [
    "D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR",
    "D2D1_BITMAP_INTERPOLATION_MODE_LINEAR",
])

D2D1_DRAW_TEXT_OPTIONS = EnumFlags("D2D1_DRAW_TEXT_OPTIONS", [
    "D2D1_DRAW_TEXT_OPTIONS_NONE",
    "D2D1_DRAW_TEXT_OPTIONS_NO_SNAP",
    "D2D1_DRAW_TEXT_OPTIONS_CLIP",
])

D2D1_PIXEL_FORMAT = Struct("D2D1_PIXEL_FORMAT", [
    (DXGI_FORMAT, "format"),
    (D2D1_ALPHA_MODE, "alphaMode"),
])

D2D1_POINT_2U = Alias("D2D1_POINT_2U", D2D_POINT_2U)
D2D1_POINT_2F = Alias("D2D1_POINT_2F", D2D_POINT_2F)
D2D1_RECT_F = Alias("D2D1_RECT_F", D2D_RECT_F)
D2D1_RECT_U = Alias("D2D1_RECT_U", D2D_RECT_U)
D2D1_SIZE_F = Alias("D2D1_SIZE_F", D2D_SIZE_F)
D2D1_SIZE_U = Alias("D2D1_SIZE_U", D2D_SIZE_U)
D2D1_COLOR_F = Alias("D2D1_COLOR_F", D2D_COLOR_F)
D2D1_MATRIX_3X2_F = Alias("D2D1_MATRIX_3X2_F", D2D_MATRIX_3X2_F)
D2D1_TAG = Alias("D2D1_TAG", UINT64)
D2D1_BITMAP_PROPERTIES = Struct("D2D1_BITMAP_PROPERTIES", [
    (D2D1_PIXEL_FORMAT, "pixelFormat"),
    (FLOAT, "dpiX"),
    (FLOAT, "dpiY"),
])

D2D1_GRADIENT_STOP = Struct("D2D1_GRADIENT_STOP", [
    (FLOAT, "position"),
    (D2D1_COLOR_F, "color"),
])

D2D1_BRUSH_PROPERTIES = Struct("D2D1_BRUSH_PROPERTIES", [
    (FLOAT, "opacity"),
    (D2D1_MATRIX_3X2_F, "transform"),
])

D2D1_BITMAP_BRUSH_PROPERTIES = Struct("D2D1_BITMAP_BRUSH_PROPERTIES", [
    (D2D1_EXTEND_MODE, "extendModeX"),
    (D2D1_EXTEND_MODE, "extendModeY"),
    (D2D1_BITMAP_INTERPOLATION_MODE, "interpolationMode"),
])

D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES = Struct("D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES", [
    (D2D1_POINT_2F, "startPoint"),
    (D2D1_POINT_2F, "endPoint"),
])

D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES = Struct("D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES", [
    (D2D1_POINT_2F, "center"),
    (D2D1_POINT_2F, "gradientOriginOffset"),
    (FLOAT, "radiusX"),
    (FLOAT, "radiusY"),
])

D2D1_ARC_SIZE = Enum("D2D1_ARC_SIZE", [
    "D2D1_ARC_SIZE_SMALL",
    "D2D1_ARC_SIZE_LARGE",
])

D2D1_CAP_STYLE = Enum("D2D1_CAP_STYLE", [
    "D2D1_CAP_STYLE_FLAT",
    "D2D1_CAP_STYLE_SQUARE",
    "D2D1_CAP_STYLE_ROUND",
    "D2D1_CAP_STYLE_TRIANGLE",
])

D2D1_DASH_STYLE = Enum("D2D1_DASH_STYLE", [
    "D2D1_DASH_STYLE_SOLID",
    "D2D1_DASH_STYLE_DASH",
    "D2D1_DASH_STYLE_DOT",
    "D2D1_DASH_STYLE_DASH_DOT",
    "D2D1_DASH_STYLE_DASH_DOT_DOT",
    "D2D1_DASH_STYLE_CUSTOM",
])

D2D1_LINE_JOIN = Enum("D2D1_LINE_JOIN", [
    "D2D1_LINE_JOIN_MITER",
    "D2D1_LINE_JOIN_BEVEL",
    "D2D1_LINE_JOIN_ROUND",
    "D2D1_LINE_JOIN_MITER_OR_BEVEL",
])

D2D1_COMBINE_MODE = Enum("D2D1_COMBINE_MODE", [
    "D2D1_COMBINE_MODE_UNION",
    "D2D1_COMBINE_MODE_INTERSECT",
    "D2D1_COMBINE_MODE_XOR",
    "D2D1_COMBINE_MODE_EXCLUDE",
])

D2D1_GEOMETRY_RELATION = Enum("D2D1_GEOMETRY_RELATION", [
    "D2D1_GEOMETRY_RELATION_UNKNOWN",
    "D2D1_GEOMETRY_RELATION_DISJOINT",
    "D2D1_GEOMETRY_RELATION_IS_CONTAINED",
    "D2D1_GEOMETRY_RELATION_CONTAINS",
    "D2D1_GEOMETRY_RELATION_OVERLAP",
])

D2D1_GEOMETRY_SIMPLIFICATION_OPTION = Enum("D2D1_GEOMETRY_SIMPLIFICATION_OPTION", [
    "D2D1_GEOMETRY_SIMPLIFICATION_OPTION_CUBICS_AND_LINES",
    "D2D1_GEOMETRY_SIMPLIFICATION_OPTION_LINES",
])

D2D1_FIGURE_BEGIN = Enum("D2D1_FIGURE_BEGIN", [
    "D2D1_FIGURE_BEGIN_FILLED",
    "D2D1_FIGURE_BEGIN_HOLLOW",
])

D2D1_FIGURE_END = Enum("D2D1_FIGURE_END", [
    "D2D1_FIGURE_END_OPEN",
    "D2D1_FIGURE_END_CLOSED",
])

D2D1_BEZIER_SEGMENT = Struct("D2D1_BEZIER_SEGMENT", [
    (D2D1_POINT_2F, "point1"),
    (D2D1_POINT_2F, "point2"),
    (D2D1_POINT_2F, "point3"),
])

D2D1_TRIANGLE = Struct("D2D1_TRIANGLE", [
    (D2D1_POINT_2F, "point1"),
    (D2D1_POINT_2F, "point2"),
    (D2D1_POINT_2F, "point3"),
])

D2D1_PATH_SEGMENT = EnumFlags("D2D1_PATH_SEGMENT", [
    "D2D1_PATH_SEGMENT_NONE",
    "D2D1_PATH_SEGMENT_FORCE_UNSTROKED",
    "D2D1_PATH_SEGMENT_FORCE_ROUND_LINE_JOIN",
])

D2D1_SWEEP_DIRECTION = Enum("D2D1_SWEEP_DIRECTION", [
    "D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE",
    "D2D1_SWEEP_DIRECTION_CLOCKWISE",
])

D2D1_FILL_MODE = Enum("D2D1_FILL_MODE", [
    "D2D1_FILL_MODE_ALTERNATE",
    "D2D1_FILL_MODE_WINDING",
])

D2D1_ARC_SEGMENT = Struct("D2D1_ARC_SEGMENT", [
    (D2D1_POINT_2F, "point"),
    (D2D1_SIZE_F, "size"),
    (FLOAT, "rotationAngle"),
    (D2D1_SWEEP_DIRECTION, "sweepDirection"),
    (D2D1_ARC_SIZE, "arcSize"),
])

D2D1_QUADRATIC_BEZIER_SEGMENT = Struct("D2D1_QUADRATIC_BEZIER_SEGMENT", [
    (D2D1_POINT_2F, "point1"),
    (D2D1_POINT_2F, "point2"),
])

D2D1_ELLIPSE = Struct("D2D1_ELLIPSE", [
    (D2D1_POINT_2F, "point"),
    (FLOAT, "radiusX"),
    (FLOAT, "radiusY"),
])

D2D1_ROUNDED_RECT = Struct("D2D1_ROUNDED_RECT", [
    (D2D1_RECT_F, "rect"),
    (FLOAT, "radiusX"),
    (FLOAT, "radiusY"),
])

D2D1_STROKE_STYLE_PROPERTIES = Struct("D2D1_STROKE_STYLE_PROPERTIES", [
    (D2D1_CAP_STYLE, "startCap"),
    (D2D1_CAP_STYLE, "endCap"),
    (D2D1_CAP_STYLE, "dashCap"),
    (D2D1_LINE_JOIN, "lineJoin"),
    (FLOAT, "miterLimit"),
    (D2D1_DASH_STYLE, "dashStyle"),
    (FLOAT, "dashOffset"),
])

D2D1_LAYER_OPTIONS = EnumFlags("D2D1_LAYER_OPTIONS", [
    "D2D1_LAYER_OPTIONS_NONE",
    "D2D1_LAYER_OPTIONS_INITIALIZE_FOR_CLEARTYPE",
])

D2D1_LAYER_PARAMETERS = Struct("D2D1_LAYER_PARAMETERS", [
    (D2D1_RECT_F, "contentBounds"),
    (ObjPointer(ID2D1Geometry), "geometricMask"),
    (D2D1_ANTIALIAS_MODE, "maskAntialiasMode"),
    (D2D1_MATRIX_3X2_F, "maskTransform"),
    (FLOAT, "opacity"),
    (ObjPointer(ID2D1Brush), "opacityBrush"),
    (D2D1_LAYER_OPTIONS, "layerOptions"),
])

D2D1_WINDOW_STATE = EnumFlags("D2D1_WINDOW_STATE", [
    "D2D1_WINDOW_STATE_NONE",
    "D2D1_WINDOW_STATE_OCCLUDED",
])

D2D1_RENDER_TARGET_TYPE = Enum("D2D1_RENDER_TARGET_TYPE", [
    "D2D1_RENDER_TARGET_TYPE_DEFAULT",
    "D2D1_RENDER_TARGET_TYPE_SOFTWARE",
    "D2D1_RENDER_TARGET_TYPE_HARDWARE",
])

D2D1_FEATURE_LEVEL = Enum("D2D1_FEATURE_LEVEL", [
    "D2D1_FEATURE_LEVEL_DEFAULT",
    "D2D1_FEATURE_LEVEL_9",
    "D2D1_FEATURE_LEVEL_10",
])

D2D1_RENDER_TARGET_USAGE = EnumFlags("D2D1_RENDER_TARGET_USAGE", [
    "D2D1_RENDER_TARGET_USAGE_NONE",
    "D2D1_RENDER_TARGET_USAGE_FORCE_BITMAP_REMOTING",
    "D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE",
])

D2D1_PRESENT_OPTIONS = EnumFlags("D2D1_PRESENT_OPTIONS", [
    "D2D1_PRESENT_OPTIONS_NONE",
    "D2D1_PRESENT_OPTIONS_RETAIN_CONTENTS",
    "D2D1_PRESENT_OPTIONS_IMMEDIATELY",
])

D2D1_RENDER_TARGET_PROPERTIES = Struct("D2D1_RENDER_TARGET_PROPERTIES", [
    (D2D1_RENDER_TARGET_TYPE, "type"),
    (D2D1_PIXEL_FORMAT, "pixelFormat"),
    (FLOAT, "dpiX"),
    (FLOAT, "dpiY"),
    (D2D1_RENDER_TARGET_USAGE, "usage"),
    (D2D1_FEATURE_LEVEL, "minLevel"),
])

D2D1_HWND_RENDER_TARGET_PROPERTIES = Struct("D2D1_HWND_RENDER_TARGET_PROPERTIES", [
    (HWND, "hwnd"),
    (D2D1_SIZE_U, "pixelSize"),
    (D2D1_PRESENT_OPTIONS, "presentOptions"),
])

D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS = EnumFlags("D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS", [
    "D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE",
    "D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_GDI_COMPATIBLE",
])

D2D1_DRAWING_STATE_DESCRIPTION = Struct("D2D1_DRAWING_STATE_DESCRIPTION", [
    (D2D1_ANTIALIAS_MODE, "antialiasMode"),
    (D2D1_TEXT_ANTIALIAS_MODE, "textAntialiasMode"),
    (D2D1_TAG, "tag1"),
    (D2D1_TAG, "tag2"),
    (D2D1_MATRIX_3X2_F, "transform"),
])

D2D1_DC_INITIALIZE_MODE = Enum("D2D1_DC_INITIALIZE_MODE", [
    "D2D1_DC_INITIALIZE_MODE_COPY",
    "D2D1_DC_INITIALIZE_MODE_CLEAR",
])

D2D1_DEBUG_LEVEL = Enum("D2D1_DEBUG_LEVEL", [
    "D2D1_DEBUG_LEVEL_NONE",
    "D2D1_DEBUG_LEVEL_ERROR",
    "D2D1_DEBUG_LEVEL_WARNING",
    "D2D1_DEBUG_LEVEL_INFORMATION",
])

D2D1_FACTORY_TYPE = Enum("D2D1_FACTORY_TYPE", [
    "D2D1_FACTORY_TYPE_SINGLE_THREADED",
    "D2D1_FACTORY_TYPE_MULTI_THREADED",
])

D2D1_FACTORY_OPTIONS = Struct("D2D1_FACTORY_OPTIONS", [
    (D2D1_DEBUG_LEVEL, "debugLevel"),
])

ID2D1Resource.methods += [
    StdMethod(Void, "GetFactory", [Out(Pointer(ObjPointer(ID2D1Factory)), "factory")], const=True),
]

ID2D1Bitmap.methods += [
    StdMethod(D2D1_SIZE_F, "GetSize", [], const=True, sideeffects=False),
    StdMethod(D2D1_SIZE_U, "GetPixelSize", [], const=True, sideeffects=False),
    StdMethod(D2D1_PIXEL_FORMAT, "GetPixelFormat", [], const=True, sideeffects=False),
    StdMethod(Void, "GetDpi", [Out(Pointer(FLOAT), "dpiX"), Out(Pointer(FLOAT), "dpiY")], const=True, sideeffects=False),
    StdMethod(HRESULT, "CopyFromBitmap", [(Pointer(Const(D2D1_POINT_2U)), "destPoint"), (ObjPointer(ID2D1Bitmap), "bitmap"), (Pointer(Const(D2D1_RECT_U)), "srcRect")]),
    StdMethod(HRESULT, "CopyFromRenderTarget", [(Pointer(Const(D2D1_POINT_2U)), "destPoint"), (ObjPointer(ID2D1RenderTarget), "renderTarget"), (Pointer(Const(D2D1_RECT_U)), "srcRect")]),
    StdMethod(HRESULT, "CopyFromMemory", [(Pointer(Const(D2D1_RECT_U)), "dstRect"), (OpaquePointer(Const(Void)), "srcData"), (UINT32, "pitch")]),
]

ID2D1GradientStopCollection.methods += [
    StdMethod(UINT32, "GetGradientStopCount", [], const=True, sideeffects=False),
    StdMethod(Void, "GetGradientStops", [Out(Pointer(D2D1_GRADIENT_STOP), "gradientStops"), (UINT, "gradientStopsCount")], const=True, sideeffects=False),
    StdMethod(D2D1_GAMMA, "GetColorInterpolationGamma", [], const=True, sideeffects=False),
    StdMethod(D2D1_EXTEND_MODE, "GetExtendMode", [], const=True, sideeffects=False),
]

ID2D1Brush.methods += [
    StdMethod(Void, "SetOpacity", [(FLOAT, "opacity")]),
    StdMethod(Void, "SetTransform", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "transform")]),
    StdMethod(FLOAT, "GetOpacity", [], const=True, sideeffects=False),
    StdMethod(Void, "GetTransform", [Out(Pointer(D2D1_MATRIX_3X2_F), "transform")], const=True, sideeffects=False),
]

ID2D1BitmapBrush.methods += [
    StdMethod(Void, "SetExtendModeX", [(D2D1_EXTEND_MODE, "extendModeX")]),
    StdMethod(Void, "SetExtendModeY", [(D2D1_EXTEND_MODE, "extendModeY")]),
    StdMethod(Void, "SetInterpolationMode", [(D2D1_BITMAP_INTERPOLATION_MODE, "interpolationMode")]),
    StdMethod(Void, "SetBitmap", [(ObjPointer(ID2D1Bitmap), "bitmap")]),
    StdMethod(D2D1_EXTEND_MODE, "GetExtendModeX", [], const=True, sideeffects=False),
    StdMethod(D2D1_EXTEND_MODE, "GetExtendModeY", [], const=True, sideeffects=False),
    StdMethod(D2D1_BITMAP_INTERPOLATION_MODE, "GetInterpolationMode", [], const=True, sideeffects=False),
    StdMethod(Void, "GetBitmap", [Out(Pointer(ObjPointer(ID2D1Bitmap)), "bitmap")], const=True),
]

ID2D1SolidColorBrush.methods += [
    StdMethod(Void, "SetColor", [(Pointer(Const(D2D1_COLOR_F)), "color")]),
    StdMethod(D2D1_COLOR_F, "GetColor", [], const=True, sideeffects=False),
]

ID2D1LinearGradientBrush.methods += [
    StdMethod(Void, "SetStartPoint", [(D2D1_POINT_2F, "startPoint")]),
    StdMethod(Void, "SetEndPoint", [(D2D1_POINT_2F, "endPoint")]),
    StdMethod(D2D1_POINT_2F, "GetStartPoint", [], const=True, sideeffects=False),
    StdMethod(D2D1_POINT_2F, "GetEndPoint", [], const=True, sideeffects=False),
    StdMethod(Void, "GetGradientStopCollection", [Out(Pointer(ObjPointer(ID2D1GradientStopCollection)), "gradientStopCollection")], const=True),
]

ID2D1RadialGradientBrush.methods += [
    StdMethod(Void, "SetCenter", [(D2D1_POINT_2F, "center")]),
    StdMethod(Void, "SetGradientOriginOffset", [(D2D1_POINT_2F, "gradientOriginOffset")]),
    StdMethod(Void, "SetRadiusX", [(FLOAT, "radiusX")]),
    StdMethod(Void, "SetRadiusY", [(FLOAT, "radiusY")]),
    StdMethod(D2D1_POINT_2F, "GetCenter", [], const=True, sideeffects=False),
    StdMethod(D2D1_POINT_2F, "GetGradientOriginOffset", [], const=True, sideeffects=False),
    StdMethod(FLOAT, "GetRadiusX", [], const=True, sideeffects=False),
    StdMethod(FLOAT, "GetRadiusY", [], const=True, sideeffects=False),
    StdMethod(Void, "GetGradientStopCollection", [Out(Pointer(ObjPointer(ID2D1GradientStopCollection)), "gradientStopCollection")], const=True),
]

ID2D1StrokeStyle.methods += [
    StdMethod(D2D1_CAP_STYLE, "GetStartCap", [], const=True, sideeffects=False),
    StdMethod(D2D1_CAP_STYLE, "GetEndCap", [], const=True, sideeffects=False),
    StdMethod(D2D1_CAP_STYLE, "GetDashCap", [], const=True, sideeffects=False),
    StdMethod(FLOAT, "GetMiterLimit", [], const=True, sideeffects=False),
    StdMethod(D2D1_LINE_JOIN, "GetLineJoin", [], const=True, sideeffects=False),
    StdMethod(FLOAT, "GetDashOffset", [], const=True, sideeffects=False),
    StdMethod(D2D1_DASH_STYLE, "GetDashStyle", [], const=True, sideeffects=False),
    StdMethod(UINT32, "GetDashesCount", [], const=True, sideeffects=False),
    StdMethod(Void, "GetDashes", [Out(Array(FLOAT, "dashesCount"), "dashes"), (UINT, "dashesCount")], const=True, sideeffects=False),
]

ID2D1Geometry.methods += [
    StdMethod(HRESULT, "GetBounds", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), Out(Pointer(D2D1_RECT_F), "bounds")], const=True, sideeffects=False),
    StdMethod(HRESULT, "GetWidenedBounds", [(FLOAT, "strokeWidth"), (ObjPointer(ID2D1StrokeStyle), "strokeStyle"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(D2D1_RECT_F), "bounds")], const=True, sideeffects=False),
    StdMethod(HRESULT, "StrokeContainsPoint", [(D2D1_POINT_2F, "point"), (FLOAT, "strokeWidth"), (ObjPointer(ID2D1StrokeStyle), "strokeStyle"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(BOOL), "contains")], const=True, sideeffects=False),
    StdMethod(HRESULT, "FillContainsPoint", [(D2D1_POINT_2F, "point"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(BOOL), "contains")], const=True, sideeffects=False),
    StdMethod(HRESULT, "CompareWithGeometry", [(ObjPointer(ID2D1Geometry), "inputGeometry"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "inputGeometryTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(D2D1_GEOMETRY_RELATION), "relation")], const=True, sideeffects=False),
    StdMethod(HRESULT, "Simplify", [(D2D1_GEOMETRY_SIMPLIFICATION_OPTION, "simplificationOption"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), (ObjPointer(ID2D1SimplifiedGeometrySink), "geometrySink")], const=True, sideeffects=False),
    StdMethod(HRESULT, "Tessellate", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), (ObjPointer(ID2D1TessellationSink), "tessellationSink")], const=True, sideeffects=False),
    StdMethod(HRESULT, "CombineWithGeometry", [(ObjPointer(ID2D1Geometry), "inputGeometry"), (D2D1_COMBINE_MODE, "combineMode"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "inputGeometryTransform"), (FLOAT, "flatteningTolerance"), (ObjPointer(ID2D1SimplifiedGeometrySink), "geometrySink")], const=True, sideeffects=False),
    StdMethod(HRESULT, "Outline", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), (ObjPointer(ID2D1SimplifiedGeometrySink), "geometrySink")], const=True, sideeffects=False),
    StdMethod(HRESULT, "ComputeArea", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(FLOAT), "area")], const=True, sideeffects=False),
    StdMethod(HRESULT, "ComputeLength", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(FLOAT), "length")], const=True, sideeffects=False),
    StdMethod(HRESULT, "ComputePointAtLength", [(FLOAT, "length"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(D2D1_POINT_2F), "point"), Out(Pointer(D2D1_POINT_2F), "unitTangentVector")], const=True, sideeffects=False),
    StdMethod(HRESULT, "Widen", [(FLOAT, "strokeWidth"), (ObjPointer(ID2D1StrokeStyle), "strokeStyle"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), (ObjPointer(ID2D1SimplifiedGeometrySink), "geometrySink")], const=True, sideeffects=False),
]

ID2D1RectangleGeometry.methods += [
    StdMethod(Void, "GetRect", [Out(Pointer(D2D1_RECT_F), "rect")], const=True, sideeffects=False),
]

ID2D1RoundedRectangleGeometry.methods += [
    StdMethod(Void, "GetRoundedRect", [Out(Pointer(D2D1_ROUNDED_RECT), "roundedRect")], const=True, sideeffects=False),
]

ID2D1EllipseGeometry.methods += [
    StdMethod(Void, "GetEllipse", [Out(Pointer(D2D1_ELLIPSE), "ellipse")], const=True, sideeffects=False),
]

ID2D1GeometryGroup.methods += [
    StdMethod(D2D1_FILL_MODE, "GetFillMode", [], const=True, sideeffects=False),
    StdMethod(UINT32, "GetSourceGeometryCount", [], const=True, sideeffects=False),
    StdMethod(Void, "GetSourceGeometries", [Out(Array(ObjPointer(ID2D1Geometry), "geometriesCount"), "geometries"), (UINT, "geometriesCount")], const=True, sideeffects=False),
]

ID2D1TransformedGeometry.methods += [
    StdMethod(Void, "GetSourceGeometry", [Out(Pointer(ObjPointer(ID2D1Geometry)), "sourceGeometry")], const=True),
    StdMethod(Void, "GetTransform", [Out(Pointer(D2D1_MATRIX_3X2_F), "transform")], const=True, sideeffects=False),
]

ID2D1SimplifiedGeometrySink.methods += [
    StdMethod(Void, "SetFillMode", [(D2D1_FILL_MODE, "fillMode")]),
    StdMethod(Void, "SetSegmentFlags", [(D2D1_PATH_SEGMENT, "vertexFlags")]),
    StdMethod(Void, "BeginFigure", [(D2D1_POINT_2F, "startPoint"), (D2D1_FIGURE_BEGIN, "figureBegin")]),
    StdMethod(Void, "AddLines", [(Array(Const(D2D1_POINT_2F), "pointsCount"), "points"), (UINT, "pointsCount")]),
    StdMethod(Void, "AddBeziers", [(Array(Const(D2D1_BEZIER_SEGMENT), "beziersCount"), "beziers"), (UINT, "beziersCount")]),
    StdMethod(Void, "EndFigure", [(D2D1_FIGURE_END, "figureEnd")]),
    StdMethod(HRESULT, "Close", []),
]

ID2D1GeometrySink.methods += [
    StdMethod(Void, "AddLine", [(D2D1_POINT_2F, "point")]),
    StdMethod(Void, "AddBezier", [(Pointer(Const(D2D1_BEZIER_SEGMENT)), "bezier")]),
    StdMethod(Void, "AddQuadraticBezier", [(Pointer(Const(D2D1_QUADRATIC_BEZIER_SEGMENT)), "bezier")]),
    StdMethod(Void, "AddQuadraticBeziers", [(Array(Const(D2D1_QUADRATIC_BEZIER_SEGMENT), "beziersCount"), "beziers"), (UINT, "beziersCount")]),
    StdMethod(Void, "AddArc", [(Pointer(Const(D2D1_ARC_SEGMENT)), "arc")]),
]

ID2D1TessellationSink.methods += [
    StdMethod(Void, "AddTriangles", [(Array(Const(D2D1_TRIANGLE), "trianglesCount"), "triangles"), (UINT, "trianglesCount")]),
    StdMethod(HRESULT, "Close", []),
]

ID2D1PathGeometry.methods += [
    StdMethod(HRESULT, "Open", [Out(Pointer(ObjPointer(ID2D1GeometrySink)), "geometrySink")]),
    StdMethod(HRESULT, "Stream", [(ObjPointer(ID2D1GeometrySink), "geometrySink")], const=True, sideeffects=False),
    StdMethod(HRESULT, "GetSegmentCount", [Out(Pointer(UINT32), "count")], const=True, sideeffects=False),
    StdMethod(HRESULT, "GetFigureCount", [Out(Pointer(UINT32), "count")], const=True, sideeffects=False),
]

ID2D1Mesh.methods += [
    StdMethod(HRESULT, "Open", [Out(Pointer(ObjPointer(ID2D1TessellationSink)), "tessellationSink")]),
]

ID2D1Layer.methods += [
    StdMethod(D2D1_SIZE_F, "GetSize", [], const=True, sideeffects=False),
]

ID2D1DrawingStateBlock.methods += [
    StdMethod(Void, "GetDescription", [Out(Pointer(D2D1_DRAWING_STATE_DESCRIPTION), "stateDescription")], const=True, sideeffects=False),
    StdMethod(Void, "SetDescription", [(Pointer(Const(D2D1_DRAWING_STATE_DESCRIPTION)), "stateDescription")]),
    StdMethod(Void, "SetTextRenderingParams", [(ObjPointer(IDWriteRenderingParams), "textRenderingParams")]),
    StdMethod(Void, "GetTextRenderingParams", [Out(Pointer(ObjPointer(IDWriteRenderingParams)), "textRenderingParams")], const=True),
]

ID2D1RenderTarget.methods += [
    StdMethod(HRESULT, "CreateBitmap", [(D2D1_SIZE_U, "size"), (OpaquePointer(Const(Void)), "srcData"), (UINT32, "pitch"), (Pointer(Const(D2D1_BITMAP_PROPERTIES)), "bitmapProperties"), Out(Pointer(ObjPointer(ID2D1Bitmap)), "bitmap")]),
    StdMethod(HRESULT, "CreateBitmapFromWicBitmap", [(Opaque("IWICBitmapSource *"), "wicBitmapSource"), (Pointer(Const(D2D1_BITMAP_PROPERTIES)), "bitmapProperties"), Out(Pointer(ObjPointer(ID2D1Bitmap)), "bitmap")]),
    StdMethod(HRESULT, "CreateSharedBitmap", [(REFIID, "riid"), Out(OpaquePointer(Void), "data"), (Pointer(Const(D2D1_BITMAP_PROPERTIES)), "bitmapProperties"), Out(Pointer(ObjPointer(ID2D1Bitmap)), "bitmap")]),
    StdMethod(HRESULT, "CreateBitmapBrush", [(ObjPointer(ID2D1Bitmap), "bitmap"), (Pointer(Const(D2D1_BITMAP_BRUSH_PROPERTIES)), "bitmapBrushProperties"), (Pointer(Const(D2D1_BRUSH_PROPERTIES)), "brushProperties"), Out(Pointer(ObjPointer(ID2D1BitmapBrush)), "bitmapBrush")]),
    StdMethod(HRESULT, "CreateSolidColorBrush", [(Pointer(Const(D2D1_COLOR_F)), "color"), (Pointer(Const(D2D1_BRUSH_PROPERTIES)), "brushProperties"), Out(Pointer(ObjPointer(ID2D1SolidColorBrush)), "solidColorBrush")]),
    StdMethod(HRESULT, "CreateGradientStopCollection", [(Array(Const(D2D1_GRADIENT_STOP), "gradientStopsCount"), "gradientStops"), (UINT, "gradientStopsCount"), (D2D1_GAMMA, "colorInterpolationGamma"), (D2D1_EXTEND_MODE, "extendMode"), Out(Pointer(ObjPointer(ID2D1GradientStopCollection)), "gradientStopCollection")]),
    StdMethod(HRESULT, "CreateLinearGradientBrush", [(Pointer(Const(D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES)), "linearGradientBrushProperties"), (Pointer(Const(D2D1_BRUSH_PROPERTIES)), "brushProperties"), (ObjPointer(ID2D1GradientStopCollection), "gradientStopCollection"), Out(Pointer(ObjPointer(ID2D1LinearGradientBrush)), "linearGradientBrush")]),
    StdMethod(HRESULT, "CreateRadialGradientBrush", [(Pointer(Const(D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES)), "radialGradientBrushProperties"), (Pointer(Const(D2D1_BRUSH_PROPERTIES)), "brushProperties"), (ObjPointer(ID2D1GradientStopCollection), "gradientStopCollection"), Out(Pointer(ObjPointer(ID2D1RadialGradientBrush)), "radialGradientBrush")]),
    StdMethod(HRESULT, "CreateCompatibleRenderTarget", [(Pointer(Const(D2D1_SIZE_F)), "desiredSize"), (Pointer(Const(D2D1_SIZE_U)), "desiredPixelSize"), (Pointer(Const(D2D1_PIXEL_FORMAT)), "desiredFormat"), (D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS, "options"), Out(Pointer(ObjPointer(ID2D1BitmapRenderTarget)), "bitmapRenderTarget")]),
    StdMethod(HRESULT, "CreateLayer", [(Pointer(Const(D2D1_SIZE_F)), "size"), Out(Pointer(ObjPointer(ID2D1Layer)), "layer")]),
    StdMethod(HRESULT, "CreateMesh", [Out(Pointer(ObjPointer(ID2D1Mesh)), "mesh")]),
    StdMethod(Void, "DrawLine", [(D2D1_POINT_2F, "point0"), (D2D1_POINT_2F, "point1"), (ObjPointer(ID2D1Brush), "brush"), (FLOAT, "strokeWidth"), (ObjPointer(ID2D1StrokeStyle), "strokeStyle")]),
    StdMethod(Void, "DrawRectangle", [(Pointer(Const(D2D1_RECT_F)), "rect"), (ObjPointer(ID2D1Brush), "brush"), (FLOAT, "strokeWidth"), (ObjPointer(ID2D1StrokeStyle), "strokeStyle")]),
    StdMethod(Void, "FillRectangle", [(Pointer(Const(D2D1_RECT_F)), "rect"), (ObjPointer(ID2D1Brush), "brush")]),
    StdMethod(Void, "DrawRoundedRectangle", [(Pointer(Const(D2D1_ROUNDED_RECT)), "roundedRect"), (ObjPointer(ID2D1Brush), "brush"), (FLOAT, "strokeWidth"), (ObjPointer(ID2D1StrokeStyle), "strokeStyle")]),
    StdMethod(Void, "FillRoundedRectangle", [(Pointer(Const(D2D1_ROUNDED_RECT)), "roundedRect"), (ObjPointer(ID2D1Brush), "brush")]),
    StdMethod(Void, "DrawEllipse", [(Pointer(Const(D2D1_ELLIPSE)), "ellipse"), (ObjPointer(ID2D1Brush), "brush"), (FLOAT, "strokeWidth"), (ObjPointer(ID2D1StrokeStyle), "strokeStyle")]),
    StdMethod(Void, "FillEllipse", [(Pointer(Const(D2D1_ELLIPSE)), "ellipse"), (ObjPointer(ID2D1Brush), "brush")]),
    StdMethod(Void, "DrawGeometry", [(ObjPointer(ID2D1Geometry), "geometry"), (ObjPointer(ID2D1Brush), "brush"), (FLOAT, "strokeWidth"), (ObjPointer(ID2D1StrokeStyle), "strokeStyle")]),
    StdMethod(Void, "FillGeometry", [(ObjPointer(ID2D1Geometry), "geometry"), (ObjPointer(ID2D1Brush), "brush"), (ObjPointer(ID2D1Brush), "opacityBrush")]),
    StdMethod(Void, "FillMesh", [(ObjPointer(ID2D1Mesh), "mesh"), (ObjPointer(ID2D1Brush), "brush")]),
    StdMethod(Void, "FillOpacityMask", [(ObjPointer(ID2D1Bitmap), "opacityMask"), (ObjPointer(ID2D1Brush), "brush"), (D2D1_OPACITY_MASK_CONTENT, "content"), (Pointer(Const(D2D1_RECT_F)), "destinationRectangle"), (Pointer(Const(D2D1_RECT_F)), "sourceRectangle")]),
    StdMethod(Void, "DrawBitmap", [(ObjPointer(ID2D1Bitmap), "bitmap"), (Pointer(Const(D2D1_RECT_F)), "destinationRectangle"), (FLOAT, "opacity"), (D2D1_BITMAP_INTERPOLATION_MODE, "interpolationMode"), (Pointer(Const(D2D1_RECT_F)), "sourceRectangle")]),
    StdMethod(Void, "DrawText", [(String(Const(WCHAR), "stringLength", wide=True), "string"), (UINT, "stringLength"), (ObjPointer(IDWriteTextFormat), "textFormat"), (Pointer(Const(D2D1_RECT_F)), "layoutRect"), (ObjPointer(ID2D1Brush), "defaultForegroundBrush"), (D2D1_DRAW_TEXT_OPTIONS, "options"), (DWRITE_MEASURING_MODE, "measuringMode")]),
    StdMethod(Void, "DrawTextLayout", [(D2D1_POINT_2F, "origin"), (ObjPointer(IDWriteTextLayout), "textLayout"), (ObjPointer(ID2D1Brush), "defaultForegroundBrush"), (D2D1_DRAW_TEXT_OPTIONS, "options")]),
    StdMethod(Void, "DrawGlyphRun", [(D2D1_POINT_2F, "baselineOrigin"), (Pointer(Const(DWRITE_GLYPH_RUN)), "glyphRun"), (ObjPointer(ID2D1Brush), "foregroundBrush"), (DWRITE_MEASURING_MODE, "measuringMode")]),
    StdMethod(Void, "SetTransform", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "transform")]),
    StdMethod(Void, "GetTransform", [Out(Pointer(D2D1_MATRIX_3X2_F), "transform")], const=True, sideeffects=False),
    StdMethod(Void, "SetAntialiasMode", [(D2D1_ANTIALIAS_MODE, "antialiasMode")]),
    StdMethod(D2D1_ANTIALIAS_MODE, "GetAntialiasMode", [], const=True, sideeffects=False),
    StdMethod(Void, "SetTextAntialiasMode", [(D2D1_TEXT_ANTIALIAS_MODE, "textAntialiasMode")]),
    StdMethod(D2D1_TEXT_ANTIALIAS_MODE, "GetTextAntialiasMode", [], const=True, sideeffects=False),
    StdMethod(Void, "SetTextRenderingParams", [(ObjPointer(IDWriteRenderingParams), "textRenderingParams")]),
    StdMethod(Void, "GetTextRenderingParams", [Out(Pointer(ObjPointer(IDWriteRenderingParams)), "textRenderingParams")], const=True),
    StdMethod(Void, "SetTags", [(D2D1_TAG, "tag1"), (D2D1_TAG, "tag2")]),
    StdMethod(Void, "GetTags", [Out(Pointer(D2D1_TAG), "tag1"), Out(Pointer(D2D1_TAG), "tag2")], const=True, sideeffects=False),
    StdMethod(Void, "PushLayer", [(Pointer(Const(D2D1_LAYER_PARAMETERS)), "layerParameters"), (ObjPointer(ID2D1Layer), "layer")]),
    StdMethod(Void, "PopLayer", []),
    StdMethod(HRESULT, "Flush", [Out(Pointer(D2D1_TAG), "tag1"), Out(Pointer(D2D1_TAG), "tag2")]),
    StdMethod(Void, "SaveDrawingState", [(ObjPointer(ID2D1DrawingStateBlock), "drawingStateBlock")], const=True, sideeffects=False),
    StdMethod(Void, "RestoreDrawingState", [(ObjPointer(ID2D1DrawingStateBlock), "drawingStateBlock")]),
    StdMethod(Void, "PushAxisAlignedClip", [(Pointer(Const(D2D1_RECT_F)), "clipRect"), (D2D1_ANTIALIAS_MODE, "antialiasMode")]),
    StdMethod(Void, "PopAxisAlignedClip", []),
    StdMethod(Void, "Clear", [(Pointer(Const(D2D1_COLOR_F)), "clearColor")]),
    StdMethod(Void, "BeginDraw", []),
    StdMethod(HRESULT, "EndDraw", [Out(Pointer(D2D1_TAG), "tag1"), Out(Pointer(D2D1_TAG), "tag2")]),
    StdMethod(D2D1_PIXEL_FORMAT, "GetPixelFormat", [], const=True, sideeffects=False),
    StdMethod(Void, "SetDpi", [(FLOAT, "dpiX"), (FLOAT, "dpiY")]),
    StdMethod(Void, "GetDpi", [Out(Pointer(FLOAT), "dpiX"), Out(Pointer(FLOAT), "dpiY")], const=True, sideeffects=False),
    StdMethod(D2D1_SIZE_F, "GetSize", [], const=True, sideeffects=False),
    StdMethod(D2D1_SIZE_U, "GetPixelSize", [], const=True, sideeffects=False),
    StdMethod(UINT32, "GetMaximumBitmapSize", [], const=True, sideeffects=False),
    StdMethod(BOOL, "IsSupported", [(Pointer(Const(D2D1_RENDER_TARGET_PROPERTIES)), "renderTargetProperties")], const=True, sideeffects=False),
]

ID2D1BitmapRenderTarget.methods += [
    StdMethod(HRESULT, "GetBitmap", [Out(Pointer(ObjPointer(ID2D1Bitmap)), "bitmap")]),
]

ID2D1HwndRenderTarget.methods += [
    StdMethod(D2D1_WINDOW_STATE, "CheckWindowState", []),
    StdMethod(HRESULT, "Resize", [(Pointer(Const(D2D1_SIZE_U)), "pixelSize")]),
    StdMethod(HWND, "GetHwnd", [], const=True, sideeffects=False),
]

ID2D1GdiInteropRenderTarget.methods += [
    StdMethod(HRESULT, "GetDC", [(D2D1_DC_INITIALIZE_MODE, "mode"), Out(Pointer(HDC), "hdc")]),
    StdMethod(HRESULT, "ReleaseDC", [(Pointer(Const(RECT)), "update")]),
]

ID2D1DCRenderTarget.methods += [
    StdMethod(HRESULT, "BindDC", [(Const(HDC), "hDC"), (Pointer(Const(RECT)), "pSubRect")]),
]

ID2D1Factory.methods += [
    StdMethod(HRESULT, "ReloadSystemMetrics", []),
    StdMethod(Void, "GetDesktopDpi", [Out(Pointer(FLOAT), "dpiX"), Out(Pointer(FLOAT), "dpiY")]),
    StdMethod(HRESULT, "CreateRectangleGeometry", [(Pointer(Const(D2D1_RECT_F)), "rectangle"), Out(Pointer(ObjPointer(ID2D1RectangleGeometry)), "rectangleGeometry")]),
    StdMethod(HRESULT, "CreateRoundedRectangleGeometry", [(Pointer(Const(D2D1_ROUNDED_RECT)), "roundedRectangle"), Out(Pointer(ObjPointer(ID2D1RoundedRectangleGeometry)), "roundedRectangleGeometry")]),
    StdMethod(HRESULT, "CreateEllipseGeometry", [(Pointer(Const(D2D1_ELLIPSE)), "ellipse"), Out(Pointer(ObjPointer(ID2D1EllipseGeometry)), "ellipseGeometry")]),
    StdMethod(HRESULT, "CreateGeometryGroup", [(D2D1_FILL_MODE, "fillMode"), (Array(ObjPointer(ID2D1Geometry), "geometriesCount"), "geometries"), (UINT, "geometriesCount"), Out(Pointer(ObjPointer(ID2D1GeometryGroup)), "geometryGroup")]),
    StdMethod(HRESULT, "CreateTransformedGeometry", [(ObjPointer(ID2D1Geometry), "sourceGeometry"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "transform"), Out(Pointer(ObjPointer(ID2D1TransformedGeometry)), "transformedGeometry")]),
    StdMethod(HRESULT, "CreatePathGeometry", [Out(Pointer(ObjPointer(ID2D1PathGeometry)), "pathGeometry")]),
    StdMethod(HRESULT, "CreateStrokeStyle", [(Pointer(Const(D2D1_STROKE_STYLE_PROPERTIES)), "strokeStyleProperties"), (Pointer(Const(FLOAT)), "dashes"), (UINT, "dashesCount"), Out(Pointer(ObjPointer(ID2D1StrokeStyle)), "strokeStyle")]),
    StdMethod(HRESULT, "CreateDrawingStateBlock", [(Pointer(Const(D2D1_DRAWING_STATE_DESCRIPTION)), "drawingStateDescription"), (ObjPointer(IDWriteRenderingParams), "textRenderingParams"), Out(Pointer(ObjPointer(ID2D1DrawingStateBlock)), "drawingStateBlock")]),
    StdMethod(HRESULT, "CreateWicBitmapRenderTarget", [(Opaque("IWICBitmap *"), "target"), (Pointer(Const(D2D1_RENDER_TARGET_PROPERTIES)), "renderTargetProperties"), Out(Pointer(ObjPointer(ID2D1RenderTarget)), "renderTarget")]),
    StdMethod(HRESULT, "CreateHwndRenderTarget", [(Pointer(Const(D2D1_RENDER_TARGET_PROPERTIES)), "renderTargetProperties"), (Pointer(Const(D2D1_HWND_RENDER_TARGET_PROPERTIES)), "hwndRenderTargetProperties"), Out(Pointer(ObjPointer(ID2D1HwndRenderTarget)), "hwndRenderTarget")]),
    StdMethod(HRESULT, "CreateDxgiSurfaceRenderTarget", [(ObjPointer(IDXGISurface), "dxgiSurface"), (Pointer(Const(D2D1_RENDER_TARGET_PROPERTIES)), "renderTargetProperties"), Out(Pointer(ObjPointer(ID2D1RenderTarget)), "renderTarget")]),
    StdMethod(HRESULT, "CreateDCRenderTarget", [(Pointer(Const(D2D1_RENDER_TARGET_PROPERTIES)), "renderTargetProperties"), Out(Pointer(ObjPointer(ID2D1DCRenderTarget)), "dcRenderTarget")]),
]

d2d1 = Module("d2d1")
d2d1.addInterfaces([
    ID2D1Factory
])
d2d1.addFunctions([
    StdFunction(HRESULT, "D2D1CreateFactory", [(D2D1_FACTORY_TYPE, "factoryType"), (REFIID, "riid"), (Pointer(Const(D2D1_FACTORY_OPTIONS)), "pFactoryOptions"), Out(Pointer(ObjPointer(Void)), "ppIFactory")]),
    StdFunction(Void, "D2D1MakeRotateMatrix", [(FLOAT, "angle"), (D2D1_POINT_2F, "center"), Out(Pointer(D2D1_MATRIX_3X2_F), "matrix")]),
    StdFunction(Void, "D2D1MakeSkewMatrix", [(FLOAT, "angleX"), (FLOAT, "angleY"), (D2D1_POINT_2F, "center"), Out(Pointer(D2D1_MATRIX_3X2_F), "matrix")]),
    StdFunction(BOOL, "D2D1IsMatrixInvertible", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "matrix")]),
    StdFunction(BOOL, "D2D1InvertMatrix", [InOut(Pointer(D2D1_MATRIX_3X2_F), "matrix")]),
])



#
# D2D1.1
#


ID2D1GdiMetafileSink = Interface("ID2D1GdiMetafileSink", IUnknown)
ID2D1GdiMetafile = Interface("ID2D1GdiMetafile", ID2D1Resource)
ID2D1CommandSink = Interface("ID2D1CommandSink", IUnknown)
ID2D1CommandList = Interface("ID2D1CommandList", ID2D1Image)
ID2D1PrintControl = Interface("ID2D1PrintControl", IUnknown)
ID2D1ImageBrush = Interface("ID2D1ImageBrush", ID2D1Brush)
ID2D1BitmapBrush1 = Interface("ID2D1BitmapBrush1", ID2D1BitmapBrush)
ID2D1StrokeStyle1 = Interface("ID2D1StrokeStyle1", ID2D1StrokeStyle)
ID2D1PathGeometry1 = Interface("ID2D1PathGeometry1", ID2D1PathGeometry)
ID2D1Properties = Interface("ID2D1Properties", IUnknown)
ID2D1Effect = Interface("ID2D1Effect", ID2D1Properties)
ID2D1Bitmap1 = Interface("ID2D1Bitmap1", ID2D1Bitmap)
ID2D1ColorContext = Interface("ID2D1ColorContext", ID2D1Resource)
ID2D1GradientStopCollection1 = Interface("ID2D1GradientStopCollection1", ID2D1GradientStopCollection)
ID2D1DrawingStateBlock1 = Interface("ID2D1DrawingStateBlock1", ID2D1DrawingStateBlock)
ID2D1DeviceContext = Interface("ID2D1DeviceContext", ID2D1RenderTarget)
ID2D1Device = Interface("ID2D1Device", ID2D1Resource)
ID2D1Factory1 = Interface("ID2D1Factory1", ID2D1Factory)
ID2D1Multithread = Interface("ID2D1Multithread", IUnknown)


D2D1_RECT_L = Alias("D2D1_RECT_L", D2D_RECT_L)
D2D1_POINT_2L = Alias("D2D1_POINT_2L", D2D_POINT_2L)


D2D1_PROPERTY_INDEX = FakeEnum(UINT32, [
    "D2D1_INVALID_PROPERTY_INDEX",
])

D2D1_PROPERTY_TYPE = Enum("D2D1_PROPERTY_TYPE", [
    "D2D1_PROPERTY_TYPE_UNKNOWN",
    "D2D1_PROPERTY_TYPE_STRING",
    "D2D1_PROPERTY_TYPE_BOOL",
    "D2D1_PROPERTY_TYPE_UINT32",
    "D2D1_PROPERTY_TYPE_INT32",
    "D2D1_PROPERTY_TYPE_FLOAT",
    "D2D1_PROPERTY_TYPE_VECTOR2",
    "D2D1_PROPERTY_TYPE_VECTOR3",
    "D2D1_PROPERTY_TYPE_VECTOR4",
    "D2D1_PROPERTY_TYPE_BLOB",
    "D2D1_PROPERTY_TYPE_IUNKNOWN",
    "D2D1_PROPERTY_TYPE_ENUM",
    "D2D1_PROPERTY_TYPE_ARRAY",
    "D2D1_PROPERTY_TYPE_CLSID",
    "D2D1_PROPERTY_TYPE_MATRIX_3X2",
    "D2D1_PROPERTY_TYPE_MATRIX_4X3",
    "D2D1_PROPERTY_TYPE_MATRIX_4X4",
    "D2D1_PROPERTY_TYPE_MATRIX_5X4",
    "D2D1_PROPERTY_TYPE_COLOR_CONTEXT",
    "D2D1_PROPERTY_TYPE_FORCE_DWORD",
])

D2D1_PROPERTY = Enum("D2D1_PROPERTY", [
    "D2D1_PROPERTY_CLSID",
    "D2D1_PROPERTY_DISPLAYNAME",
    "D2D1_PROPERTY_AUTHOR",
    "D2D1_PROPERTY_CATEGORY",
    "D2D1_PROPERTY_DESCRIPTION",
    "D2D1_PROPERTY_INPUTS",
    "D2D1_PROPERTY_CACHED",
    "D2D1_PROPERTY_PRECISION",
    "D2D1_PROPERTY_MIN_INPUTS",
    "D2D1_PROPERTY_MAX_INPUTS",
    "D2D1_PROPERTY_FORCE_DWORD",
])

D2D1_SUBPROPERTY = Enum("D2D1_SUBPROPERTY", [
    "D2D1_SUBPROPERTY_DISPLAYNAME",
    "D2D1_SUBPROPERTY_ISREADONLY",
    "D2D1_SUBPROPERTY_MIN",
    "D2D1_SUBPROPERTY_MAX",
    "D2D1_SUBPROPERTY_DEFAULT",
    "D2D1_SUBPROPERTY_FIELDS",
    "D2D1_SUBPROPERTY_INDEX",
    "D2D1_SUBPROPERTY_FORCE_DWORD",
])

D2D1_BITMAP_OPTIONS = Enum("D2D1_BITMAP_OPTIONS", [
    "D2D1_BITMAP_OPTIONS_NONE",
    "D2D1_BITMAP_OPTIONS_TARGET",
    "D2D1_BITMAP_OPTIONS_CANNOT_DRAW",
    "D2D1_BITMAP_OPTIONS_CPU_READ",
    "D2D1_BITMAP_OPTIONS_GDI_COMPATIBLE",
    "D2D1_BITMAP_OPTIONS_FORCE_DWORD",
])

D2D1_COMPOSITE_MODE = Enum("D2D1_COMPOSITE_MODE", [
    "D2D1_COMPOSITE_MODE_SOURCE_OVER",
    "D2D1_COMPOSITE_MODE_DESTINATION_OVER",
    "D2D1_COMPOSITE_MODE_SOURCE_IN",
    "D2D1_COMPOSITE_MODE_DESTINATION_IN",
    "D2D1_COMPOSITE_MODE_SOURCE_OUT",
    "D2D1_COMPOSITE_MODE_DESTINATION_OUT",
    "D2D1_COMPOSITE_MODE_SOURCE_ATOP",
    "D2D1_COMPOSITE_MODE_DESTINATION_ATOP",
    "D2D1_COMPOSITE_MODE_XOR",
    "D2D1_COMPOSITE_MODE_PLUS",
    "D2D1_COMPOSITE_MODE_SOURCE_COPY",
    "D2D1_COMPOSITE_MODE_BOUNDED_SOURCE_COPY",
    "D2D1_COMPOSITE_MODE_MASK_INVERT",
    "D2D1_COMPOSITE_MODE_FORCE_DWORD",
])

D2D1_BUFFER_PRECISION = Enum("D2D1_BUFFER_PRECISION", [
    "D2D1_BUFFER_PRECISION_UNKNOWN",
    "D2D1_BUFFER_PRECISION_8BPC_UNORM",
    "D2D1_BUFFER_PRECISION_8BPC_UNORM_SRGB",
    "D2D1_BUFFER_PRECISION_16BPC_UNORM",
    "D2D1_BUFFER_PRECISION_16BPC_FLOAT",
    "D2D1_BUFFER_PRECISION_32BPC_FLOAT",
    "D2D1_BUFFER_PRECISION_FORCE_DWORD",
])

D2D1_MAP_OPTIONS = Enum("D2D1_MAP_OPTIONS", [
    "D2D1_MAP_OPTIONS_NONE",
    "D2D1_MAP_OPTIONS_READ",
    "D2D1_MAP_OPTIONS_WRITE",
    "D2D1_MAP_OPTIONS_DISCARD",
    "D2D1_MAP_OPTIONS_FORCE_DWORD",
])

D2D1_INTERPOLATION_MODE = Enum("D2D1_INTERPOLATION_MODE", [
    "D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR",
    "D2D1_INTERPOLATION_MODE_LINEAR",
    "D2D1_INTERPOLATION_MODE_CUBIC",
    "D2D1_INTERPOLATION_MODE_MULTI_SAMPLE_LINEAR",
    "D2D1_INTERPOLATION_MODE_ANISOTROPIC",
    "D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC",
    "D2D1_INTERPOLATION_MODE_FORCE_DWORD",
])

D2D1_UNIT_MODE = Enum("D2D1_UNIT_MODE", [
    "D2D1_UNIT_MODE_DIPS",
    "D2D1_UNIT_MODE_PIXELS",
    "D2D1_UNIT_MODE_FORCE_DWORD",
])

D2D1_COLOR_SPACE = Enum("D2D1_COLOR_SPACE", [
    "D2D1_COLOR_SPACE_CUSTOM",
    "D2D1_COLOR_SPACE_SRGB",
    "D2D1_COLOR_SPACE_SCRGB",
    "D2D1_COLOR_SPACE_FORCE_DWORD",
])

D2D1_DEVICE_CONTEXT_OPTIONS = Enum("D2D1_DEVICE_CONTEXT_OPTIONS", [
    "D2D1_DEVICE_CONTEXT_OPTIONS_NONE",
    "D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS",
    "D2D1_DEVICE_CONTEXT_OPTIONS_FORCE_DWORD",
])

D2D1_STROKE_TRANSFORM_TYPE = Enum("D2D1_STROKE_TRANSFORM_TYPE", [
    "D2D1_STROKE_TRANSFORM_TYPE_NORMAL",
    "D2D1_STROKE_TRANSFORM_TYPE_FIXED",
    "D2D1_STROKE_TRANSFORM_TYPE_HAIRLINE",
    "D2D1_STROKE_TRANSFORM_TYPE_FORCE_DWORD",
])

D2D1_PRIMITIVE_BLEND = Enum("D2D1_PRIMITIVE_BLEND", [
    "D2D1_PRIMITIVE_BLEND_SOURCE_OVER",
    "D2D1_PRIMITIVE_BLEND_COPY",
    "D2D1_PRIMITIVE_BLEND_MIN",
    "D2D1_PRIMITIVE_BLEND_ADD",
    "D2D1_PRIMITIVE_BLEND_FORCE_DWORD",
])

D2D1_THREADING_MODE = Enum("D2D1_THREADING_MODE", [
    "D2D1_THREADING_MODE_SINGLE_THREADED",
    "D2D1_THREADING_MODE_MULTI_THREADED",
    "D2D1_THREADING_MODE_FORCE_DWORD",
])

D2D1_COLOR_INTERPOLATION_MODE = Enum("D2D1_COLOR_INTERPOLATION_MODE", [
    "D2D1_COLOR_INTERPOLATION_MODE_STRAIGHT",
    "D2D1_COLOR_INTERPOLATION_MODE_PREMULTIPLIED",
    "D2D1_COLOR_INTERPOLATION_MODE_FORCE_DWORD",
])

D2D1_VECTOR_2F = Alias("D2D1_VECTOR_2F", D2D_VECTOR_2F)
D2D1_VECTOR_3F = Alias("D2D1_VECTOR_3F", D2D_VECTOR_3F)
D2D1_VECTOR_4F = Alias("D2D1_VECTOR_4F", D2D_VECTOR_4F)
D2D1_BITMAP_PROPERTIES1 = Struct("D2D1_BITMAP_PROPERTIES1", [
    (D2D1_PIXEL_FORMAT, "pixelFormat"),
    (FLOAT, "dpiX"),
    (FLOAT, "dpiY"),
    (D2D1_BITMAP_OPTIONS, "bitmapOptions"),
    (ObjPointer(ID2D1ColorContext), "colorContext"),
])

D2D1_MAPPED_RECT = Struct("D2D1_MAPPED_RECT", [
    (UINT32, "pitch"),
    (Pointer(BYTE), "bits"),
])

D2D1_RENDERING_CONTROLS = Struct("D2D1_RENDERING_CONTROLS", [
    (D2D1_BUFFER_PRECISION, "bufferPrecision"),
    (D2D1_SIZE_U, "tileSize"),
])

D2D1_EFFECT_INPUT_DESCRIPTION = Struct("D2D1_EFFECT_INPUT_DESCRIPTION", [
    (ObjPointer(ID2D1Effect), "effect"),
    (UINT32, "inputIndex"),
    (D2D1_RECT_F, "inputRectangle"),
])

D2D1_MATRIX_4X3_F = Alias("D2D1_MATRIX_4X3_F", D2D_MATRIX_4X3_F)
D2D1_MATRIX_4X4_F = Alias("D2D1_MATRIX_4X4_F", D2D_MATRIX_4X4_F)
D2D1_MATRIX_5X4_F = Alias("D2D1_MATRIX_5X4_F", D2D_MATRIX_5X4_F)
D2D1_POINT_DESCRIPTION = Struct("D2D1_POINT_DESCRIPTION", [
    (D2D1_POINT_2F, "point"),
    (D2D1_POINT_2F, "unitTangentVector"),
    (UINT32, "endSegment"),
    (UINT32, "endFigure"),
    (FLOAT, "lengthToEndSegment"),
])

D2D1_IMAGE_BRUSH_PROPERTIES = Struct("D2D1_IMAGE_BRUSH_PROPERTIES", [
    (D2D1_RECT_F, "sourceRectangle"),
    (D2D1_EXTEND_MODE, "extendModeX"),
    (D2D1_EXTEND_MODE, "extendModeY"),
    (D2D1_INTERPOLATION_MODE, "interpolationMode"),
])

D2D1_BITMAP_BRUSH_PROPERTIES1 = Struct("D2D1_BITMAP_BRUSH_PROPERTIES1", [
    (D2D1_EXTEND_MODE, "extendModeX"),
    (D2D1_EXTEND_MODE, "extendModeY"),
    (D2D1_INTERPOLATION_MODE, "interpolationMode"),
])

D2D1_STROKE_STYLE_PROPERTIES1 = Struct("D2D1_STROKE_STYLE_PROPERTIES1", [
    (D2D1_CAP_STYLE, "startCap"),
    (D2D1_CAP_STYLE, "endCap"),
    (D2D1_CAP_STYLE, "dashCap"),
    (D2D1_LINE_JOIN, "lineJoin"),
    (FLOAT, "miterLimit"),
    (D2D1_DASH_STYLE, "dashStyle"),
    (FLOAT, "dashOffset"),
    (D2D1_STROKE_TRANSFORM_TYPE, "transformType"),
])

D2D1_LAYER_OPTIONS1 = Enum("D2D1_LAYER_OPTIONS1", [
    "D2D1_LAYER_OPTIONS1_NONE",
    "D2D1_LAYER_OPTIONS1_INITIALIZE_FROM_BACKGROUND",
    "D2D1_LAYER_OPTIONS1_IGNORE_ALPHA",
    "D2D1_LAYER_OPTIONS1_FORCE_DWORD",
])

D2D1_LAYER_PARAMETERS1 = Struct("D2D1_LAYER_PARAMETERS1", [
    (D2D1_RECT_F, "contentBounds"),
    (ObjPointer(ID2D1Geometry), "geometricMask"),
    (D2D1_ANTIALIAS_MODE, "maskAntialiasMode"),
    (D2D1_MATRIX_3X2_F, "maskTransform"),
    (FLOAT, "opacity"),
    (ObjPointer(ID2D1Brush), "opacityBrush"),
    (D2D1_LAYER_OPTIONS1, "layerOptions"),
])

D2D1_PRINT_FONT_SUBSET_MODE = Enum("D2D1_PRINT_FONT_SUBSET_MODE", [
    "D2D1_PRINT_FONT_SUBSET_MODE_DEFAULT",
    "D2D1_PRINT_FONT_SUBSET_MODE_EACHPAGE",
    "D2D1_PRINT_FONT_SUBSET_MODE_NONE",
    "D2D1_PRINT_FONT_SUBSET_MODE_FORCE_DWORD",
])

D2D1_DRAWING_STATE_DESCRIPTION1 = Struct("D2D1_DRAWING_STATE_DESCRIPTION1", [
    (D2D1_ANTIALIAS_MODE, "antialiasMode"),
    (D2D1_TEXT_ANTIALIAS_MODE, "textAntialiasMode"),
    (D2D1_TAG, "tag1"),
    (D2D1_TAG, "tag2"),
    (D2D1_MATRIX_3X2_F, "transform"),
    (D2D1_PRIMITIVE_BLEND, "primitiveBlend"),
    (D2D1_UNIT_MODE, "unitMode"),
])

D2D1_PRINT_CONTROL_PROPERTIES = Struct("D2D1_PRINT_CONTROL_PROPERTIES", [
    (D2D1_PRINT_FONT_SUBSET_MODE, "fontSubset"),
    (FLOAT, "rasterDPI"),
    (D2D1_COLOR_SPACE, "colorSpace"),
])

D2D1_CREATION_PROPERTIES = Struct("D2D1_CREATION_PROPERTIES", [
    (D2D1_THREADING_MODE, "threadingMode"),
    (D2D1_DEBUG_LEVEL, "debugLevel"),
    (D2D1_DEVICE_CONTEXT_OPTIONS, "options"),
])

ID2D1GdiMetafileSink.methods += [
    StdMethod(HRESULT, "ProcessRecord", [(DWORD, "recordType"), (OpaqueBlob(Const(Void), "recordDataSize"), "recordData"), (DWORD, "recordDataSize")]),
]

ID2D1GdiMetafile.methods += [
    StdMethod(HRESULT, "Stream", [(ObjPointer(ID2D1GdiMetafileSink), "sink")]),
    StdMethod(HRESULT, "GetBounds", [Out(Pointer(D2D1_RECT_F), "bounds")]),
]

ID2D1CommandSink.methods += [
    StdMethod(HRESULT, "BeginDraw", []),
    StdMethod(HRESULT, "EndDraw", []),
    StdMethod(HRESULT, "SetAntialiasMode", [(D2D1_ANTIALIAS_MODE, "antialiasMode")]),
    StdMethod(HRESULT, "SetTags", [(D2D1_TAG, "tag1"), (D2D1_TAG, "tag2")]),
    StdMethod(HRESULT, "SetTextAntialiasMode", [(D2D1_TEXT_ANTIALIAS_MODE, "textAntialiasMode")]),
    StdMethod(HRESULT, "SetTextRenderingParams", [(ObjPointer(IDWriteRenderingParams), "textRenderingParams")]),
    StdMethod(HRESULT, "SetTransform", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "transform")]),
    StdMethod(HRESULT, "SetPrimitiveBlend", [(D2D1_PRIMITIVE_BLEND, "primitiveBlend")]),
    StdMethod(HRESULT, "SetUnitMode", [(D2D1_UNIT_MODE, "unitMode")]),
    StdMethod(HRESULT, "Clear", [(Pointer(Const(D2D1_COLOR_F)), "color")]),
    StdMethod(HRESULT, "DrawGlyphRun", [(D2D1_POINT_2F, "baselineOrigin"), (Pointer(Const(DWRITE_GLYPH_RUN)), "glyphRun"), (Pointer(Const(DWRITE_GLYPH_RUN_DESCRIPTION)), "glyphRunDescription"), (ObjPointer(ID2D1Brush), "foregroundBrush"), (DWRITE_MEASURING_MODE, "measuringMode")]),
    StdMethod(HRESULT, "DrawLine", [(D2D1_POINT_2F, "point0"), (D2D1_POINT_2F, "point1"), (ObjPointer(ID2D1Brush), "brush"), (FLOAT, "strokeWidth"), (ObjPointer(ID2D1StrokeStyle), "strokeStyle")]),
    StdMethod(HRESULT, "DrawGeometry", [(ObjPointer(ID2D1Geometry), "geometry"), (ObjPointer(ID2D1Brush), "brush"), (FLOAT, "strokeWidth"), (ObjPointer(ID2D1StrokeStyle), "strokeStyle")]),
    StdMethod(HRESULT, "DrawRectangle", [(Pointer(Const(D2D1_RECT_F)), "rect"), (ObjPointer(ID2D1Brush), "brush"), (FLOAT, "strokeWidth"), (ObjPointer(ID2D1StrokeStyle), "strokeStyle")]),
    StdMethod(HRESULT, "DrawBitmap", [(ObjPointer(ID2D1Bitmap), "bitmap"), (Pointer(Const(D2D1_RECT_F)), "destinationRectangle"), (FLOAT, "opacity"), (D2D1_INTERPOLATION_MODE, "interpolationMode"), (Pointer(Const(D2D1_RECT_F)), "sourceRectangle"), (Pointer(Const(D2D1_MATRIX_4X4_F)), "perspectiveTransform")]),
    StdMethod(HRESULT, "DrawImage", [(ObjPointer(ID2D1Image), "image"), (Pointer(Const(D2D1_POINT_2F)), "targetOffset"), (Pointer(Const(D2D1_RECT_F)), "imageRectangle"), (D2D1_INTERPOLATION_MODE, "interpolationMode"), (D2D1_COMPOSITE_MODE, "compositeMode")]),
    StdMethod(HRESULT, "DrawGdiMetafile", [(ObjPointer(ID2D1GdiMetafile), "gdiMetafile"), (Pointer(Const(D2D1_POINT_2F)), "targetOffset")]),
    StdMethod(HRESULT, "FillMesh", [(ObjPointer(ID2D1Mesh), "mesh"), (ObjPointer(ID2D1Brush), "brush")]),
    StdMethod(HRESULT, "FillOpacityMask", [(ObjPointer(ID2D1Bitmap), "opacityMask"), (ObjPointer(ID2D1Brush), "brush"), (Pointer(Const(D2D1_RECT_F)), "destinationRectangle"), (Pointer(Const(D2D1_RECT_F)), "sourceRectangle")]),
    StdMethod(HRESULT, "FillGeometry", [(ObjPointer(ID2D1Geometry), "geometry"), (ObjPointer(ID2D1Brush), "brush"), (ObjPointer(ID2D1Brush), "opacityBrush")]),
    StdMethod(HRESULT, "FillRectangle", [(Pointer(Const(D2D1_RECT_F)), "rect"), (ObjPointer(ID2D1Brush), "brush")]),
    StdMethod(HRESULT, "PushAxisAlignedClip", [(Pointer(Const(D2D1_RECT_F)), "clipRect"), (D2D1_ANTIALIAS_MODE, "antialiasMode")]),
    StdMethod(HRESULT, "PushLayer", [(Pointer(Const(D2D1_LAYER_PARAMETERS1)), "layerParameters1"), (ObjPointer(ID2D1Layer), "layer")]),
    StdMethod(HRESULT, "PopAxisAlignedClip", []),
    StdMethod(HRESULT, "PopLayer", []),
]

ID2D1CommandList.methods += [
    StdMethod(HRESULT, "Stream", [(ObjPointer(ID2D1CommandSink), "sink")]),
    StdMethod(HRESULT, "Close", []),
]

ID2D1PrintControl.methods += [
    StdMethod(HRESULT, "AddPage", [(ObjPointer(ID2D1CommandList), "commandList"), (D2D_SIZE_F, "pageSize"), (Opaque("IStream *"), "pagePrintTicketStream"), Out(Pointer(D2D1_TAG), "tag1"), Out(Pointer(D2D1_TAG), "tag2")]),
    StdMethod(HRESULT, "Close", []),
]

ID2D1ImageBrush.methods += [
    StdMethod(Void, "SetImage", [(ObjPointer(ID2D1Image), "image")]),
    StdMethod(Void, "SetExtendModeX", [(D2D1_EXTEND_MODE, "extendModeX")]),
    StdMethod(Void, "SetExtendModeY", [(D2D1_EXTEND_MODE, "extendModeY")]),
    StdMethod(Void, "SetInterpolationMode", [(D2D1_INTERPOLATION_MODE, "interpolationMode")]),
    StdMethod(Void, "SetSourceRectangle", [(Pointer(Const(D2D1_RECT_F)), "sourceRectangle")]),
    StdMethod(Void, "GetImage", [(Pointer(ObjPointer(ID2D1Image)), "image")], const=True, sideeffects=False),
    StdMethod(D2D1_EXTEND_MODE, "GetExtendModeX", [], const=True, sideeffects=False),
    StdMethod(D2D1_EXTEND_MODE, "GetExtendModeY", [], const=True, sideeffects=False),
    StdMethod(D2D1_INTERPOLATION_MODE, "GetInterpolationMode", [], const=True, sideeffects=False),
    StdMethod(Void, "GetSourceRectangle", [Out(Pointer(D2D1_RECT_F), "sourceRectangle")], const=True, sideeffects=False),
]

ID2D1BitmapBrush1.methods += [
    StdMethod(Void, "SetInterpolationMode1", [(D2D1_INTERPOLATION_MODE, "interpolationMode")]),
    StdMethod(D2D1_INTERPOLATION_MODE, "GetInterpolationMode1", [], const=True, sideeffects=False),
]

ID2D1StrokeStyle1.methods += [
    StdMethod(D2D1_STROKE_TRANSFORM_TYPE, "GetStrokeTransformType", [], const=True, sideeffects=False),
]

ID2D1PathGeometry1.methods += [
    StdMethod(HRESULT, "ComputePointAndSegmentAtLength", [(FLOAT, "length"), (UINT32, "startSegment"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(D2D1_POINT_DESCRIPTION), "pointDescription")], const=True, sideeffects=False),
]

ID2D1Properties.methods += [
    StdMethod(UINT32, "GetPropertyCount", [], const=True, sideeffects=False),
    StdMethod(HRESULT, "GetPropertyName", [(UINT32, "index"), Out(PWSTR, "name"), (UINT32, "nameCount")], const=True, sideeffects=False),
    StdMethod(UINT32, "GetPropertyNameLength", [(UINT32, "index")], const=True, sideeffects=False),
    StdMethod(D2D1_PROPERTY_TYPE, "GetType", [(UINT32, "index")], const=True, sideeffects=False),
    StdMethod(D2D1_PROPERTY_INDEX, "GetPropertyIndex", [(PCWSTR, "name")], const=True, sideeffects=False),
    StdMethod(HRESULT, "SetValueByName", [(PCWSTR, "name"), (D2D1_PROPERTY_TYPE, "type"), (Pointer(Const(BYTE)), "data"), (UINT32, "dataSize")]),
    StdMethod(HRESULT, "SetValue", [(UINT32, "index"), (D2D1_PROPERTY_TYPE, "type"), (Pointer(Const(BYTE)), "data"), (UINT32, "dataSize")]),
    StdMethod(HRESULT, "GetValueByName", [(PCWSTR, "name"), (D2D1_PROPERTY_TYPE, "type"), Out(Pointer(BYTE), "data"), (UINT32, "dataSize")], const=True, sideeffects=False),
    StdMethod(HRESULT, "GetValue", [(UINT32, "index"), (D2D1_PROPERTY_TYPE, "type"), Out(Pointer(BYTE), "data"), (UINT32, "dataSize")], const=True, sideeffects=False),
    StdMethod(UINT32, "GetValueSize", [(UINT32, "index")], const=True, sideeffects=False),
    StdMethod(HRESULT, "GetSubProperties", [(UINT32, "index"), Out(Pointer(ObjPointer(ID2D1Properties)), "subProperties")], const=True),
]

ID2D1Effect.methods += [
    StdMethod(Void, "SetInput", [(UINT32, "index"), (ObjPointer(ID2D1Image), "input"), (BOOL, "invalidate")]),
    StdMethod(HRESULT, "SetInputCount", [(UINT32, "inputCount")]),
    StdMethod(Void, "GetInput", [(UINT32, "index"), Out(Pointer(ObjPointer(ID2D1Image)), "input")], const=True),
    StdMethod(UINT32, "GetInputCount", [], const=True, sideeffects=False),
    StdMethod(Void, "GetOutput", [(Pointer(ObjPointer(ID2D1Image)), "outputImage")], const=True, sideeffects=False),
]

ID2D1Bitmap1.methods += [
    StdMethod(Void, "GetColorContext", [(Pointer(ObjPointer(ID2D1ColorContext)), "colorContext")], const=True, sideeffects=False),
    StdMethod(D2D1_BITMAP_OPTIONS, "GetOptions", [], const=True, sideeffects=False),
    StdMethod(HRESULT, "GetSurface", [(Pointer(ObjPointer(IDXGISurface)), "dxgiSurface")], const=True, sideeffects=False),
    StdMethod(HRESULT, "Map", [(D2D1_MAP_OPTIONS, "options"), Out(Pointer(D2D1_MAPPED_RECT), "mappedRect")]),
    StdMethod(HRESULT, "Unmap", []),
]

ID2D1ColorContext.methods += [
    StdMethod(D2D1_COLOR_SPACE, "GetColorSpace", [], const=True, sideeffects=False),
    StdMethod(UINT32, "GetProfileSize", [], const=True, sideeffects=False),
    StdMethod(HRESULT, "GetProfile", [Out(Pointer(BYTE), "profile"), (UINT32, "profileSize")], const=True, sideeffects=False),
]

ID2D1GradientStopCollection1.methods += [
    StdMethod(Void, "GetGradientStops1", [Out(Array(D2D1_GRADIENT_STOP, "gradientStopsCount"), "gradientStops"), (UINT32, "gradientStopsCount")], const=True, sideeffects=False),
    StdMethod(D2D1_COLOR_SPACE, "GetPreInterpolationSpace", [], const=True, sideeffects=False),
    StdMethod(D2D1_COLOR_SPACE, "GetPostInterpolationSpace", [], const=True, sideeffects=False),
    StdMethod(D2D1_BUFFER_PRECISION, "GetBufferPrecision", [], const=True, sideeffects=False),
    StdMethod(D2D1_COLOR_INTERPOLATION_MODE, "GetColorInterpolationMode", [], const=True, sideeffects=False),
]

ID2D1DrawingStateBlock1.methods += [
    StdMethod(Void, "GetDescription", [Out(Pointer(D2D1_DRAWING_STATE_DESCRIPTION1), "stateDescription")], const=True, sideeffects=False),
    StdMethod(Void, "SetDescription", [(Pointer(Const(D2D1_DRAWING_STATE_DESCRIPTION1)), "stateDescription")]),
]

ID2D1DeviceContext.methods += [
    StdMethod(HRESULT, "CreateBitmap", [(D2D1_SIZE_U, "size"), (OpaquePointer(Const(Void)), "sourceData"), (UINT32, "pitch"), (Pointer(Const(D2D1_BITMAP_PROPERTIES1)), "bitmapProperties"), Out(Pointer(ObjPointer(ID2D1Bitmap1)), "bitmap")]),
    StdMethod(HRESULT, "CreateBitmapFromWicBitmap", [(Opaque("IWICBitmapSource *"), "wicBitmapSource"), (Pointer(Const(D2D1_BITMAP_PROPERTIES1)), "bitmapProperties"), Out(Pointer(ObjPointer(ID2D1Bitmap1)), "bitmap")]),
    StdMethod(HRESULT, "CreateColorContext", [(D2D1_COLOR_SPACE, "space"), (Pointer(Const(BYTE)), "profile"), (UINT32, "profileSize"), Out(Pointer(ObjPointer(ID2D1ColorContext)), "colorContext")]),
    StdMethod(HRESULT, "CreateColorContextFromFilename", [(PCWSTR, "filename"), Out(Pointer(ObjPointer(ID2D1ColorContext)), "colorContext")]),
    StdMethod(HRESULT, "CreateColorContextFromWicColorContext", [(Opaque("IWICColorContext *"), "wicColorContext"), Out(Pointer(ObjPointer(ID2D1ColorContext)), "colorContext")]),
    StdMethod(HRESULT, "CreateBitmapFromDxgiSurface", [(ObjPointer(IDXGISurface), "surface"), (Pointer(Const(D2D1_BITMAP_PROPERTIES1)), "bitmapProperties"), Out(Pointer(ObjPointer(ID2D1Bitmap1)), "bitmap")]),
    StdMethod(HRESULT, "CreateEffect", [(REFCLSID, "effectId"), Out(Pointer(ObjPointer(ID2D1Effect)), "effect")]),
    StdMethod(HRESULT, "CreateGradientStopCollection", [(Pointer(Const(D2D1_GRADIENT_STOP)), "straightAlphaGradientStops"), (UINT32, "straightAlphaGradientStopsCount"), (D2D1_COLOR_SPACE, "preInterpolationSpace"), (D2D1_COLOR_SPACE, "postInterpolationSpace"), (D2D1_BUFFER_PRECISION, "bufferPrecision"), (D2D1_EXTEND_MODE, "extendMode"), (D2D1_COLOR_INTERPOLATION_MODE, "colorInterpolationMode"), Out(Pointer(ObjPointer(ID2D1GradientStopCollection1)), "gradientStopCollection1")]),
    StdMethod(HRESULT, "CreateImageBrush", [(ObjPointer(ID2D1Image), "image"), (Pointer(Const(D2D1_IMAGE_BRUSH_PROPERTIES)), "imageBrushProperties"), (Pointer(Const(D2D1_BRUSH_PROPERTIES)), "brushProperties"), Out(Pointer(ObjPointer(ID2D1ImageBrush)), "imageBrush")]),
    StdMethod(HRESULT, "CreateBitmapBrush", [(ObjPointer(ID2D1Bitmap), "bitmap"), (Pointer(Const(D2D1_BITMAP_BRUSH_PROPERTIES1)), "bitmapBrushProperties"), (Pointer(Const(D2D1_BRUSH_PROPERTIES)), "brushProperties"), Out(Pointer(ObjPointer(ID2D1BitmapBrush1)), "bitmapBrush")]),
    StdMethod(HRESULT, "CreateCommandList", [(Pointer(ObjPointer(ID2D1CommandList)), "commandList")]),
    StdMethod(BOOL, "IsDxgiFormatSupported", [(DXGI_FORMAT, "format")], const=True, sideeffects=False),
    StdMethod(BOOL, "IsBufferPrecisionSupported", [(D2D1_BUFFER_PRECISION, "bufferPrecision")], const=True, sideeffects=False),
    StdMethod(HRESULT, "GetImageLocalBounds", [(ObjPointer(ID2D1Image), "image"), Out(Pointer(D2D1_RECT_F), "localBounds")], const=True, sideeffects=False),
    StdMethod(HRESULT, "GetImageWorldBounds", [(ObjPointer(ID2D1Image), "image"), Out(Pointer(D2D1_RECT_F), "worldBounds")], const=True, sideeffects=False),
    StdMethod(HRESULT, "GetGlyphRunWorldBounds", [(D2D1_POINT_2F, "baselineOrigin"), (Pointer(Const(DWRITE_GLYPH_RUN)), "glyphRun"), (DWRITE_MEASURING_MODE, "measuringMode"), Out(Pointer(D2D1_RECT_F), "bounds")], const=True, sideeffects=False),
    StdMethod(Void, "GetDevice", [(Pointer(ObjPointer(ID2D1Device)), "device")], const=True, sideeffects=False),
    StdMethod(Void, "SetTarget", [(ObjPointer(ID2D1Image), "image")]),
    StdMethod(Void, "GetTarget", [(Pointer(ObjPointer(ID2D1Image)), "image")], const=True, sideeffects=False),
    StdMethod(Void, "SetRenderingControls", [(Pointer(Const(D2D1_RENDERING_CONTROLS)), "renderingControls")]),
    StdMethod(Void, "GetRenderingControls", [Out(Pointer(D2D1_RENDERING_CONTROLS), "renderingControls")], const=True, sideeffects=False),
    StdMethod(Void, "SetPrimitiveBlend", [(D2D1_PRIMITIVE_BLEND, "primitiveBlend")]),
    StdMethod(D2D1_PRIMITIVE_BLEND, "GetPrimitiveBlend", [], const=True, sideeffects=False),
    StdMethod(Void, "SetUnitMode", [(D2D1_UNIT_MODE, "unitMode")]),
    StdMethod(D2D1_UNIT_MODE, "GetUnitMode", [], const=True, sideeffects=False),
    StdMethod(Void, "DrawGlyphRun", [(D2D1_POINT_2F, "baselineOrigin"), (Pointer(Const(DWRITE_GLYPH_RUN)), "glyphRun"), (Pointer(Const(DWRITE_GLYPH_RUN_DESCRIPTION)), "glyphRunDescription"), (ObjPointer(ID2D1Brush), "foregroundBrush"), (DWRITE_MEASURING_MODE, "measuringMode")]),
    StdMethod(Void, "DrawImage", [(ObjPointer(ID2D1Image), "image"), (Pointer(Const(D2D1_POINT_2F)), "targetOffset"), (Pointer(Const(D2D1_RECT_F)), "imageRectangle"), (D2D1_INTERPOLATION_MODE, "interpolationMode"), (D2D1_COMPOSITE_MODE, "compositeMode")]),
    StdMethod(Void, "DrawGdiMetafile", [(ObjPointer(ID2D1GdiMetafile), "gdiMetafile"), (Pointer(Const(D2D1_POINT_2F)), "targetOffset")]),
    StdMethod(Void, "DrawBitmap", [(ObjPointer(ID2D1Bitmap), "bitmap"), (Pointer(Const(D2D1_RECT_F)), "destinationRectangle"), (FLOAT, "opacity"), (D2D1_INTERPOLATION_MODE, "interpolationMode"), (Pointer(Const(D2D1_RECT_F)), "sourceRectangle"), (Pointer(Const(D2D1_MATRIX_4X4_F)), "perspectiveTransform")]),
    StdMethod(Void, "PushLayer", [(Pointer(Const(D2D1_LAYER_PARAMETERS1)), "layerParameters"), (ObjPointer(ID2D1Layer), "layer")]),
    StdMethod(HRESULT, "InvalidateEffectInputRectangle", [(ObjPointer(ID2D1Effect), "effect"), (UINT32, "input"), (Pointer(Const(D2D1_RECT_F)), "inputRectangle")]),
    StdMethod(HRESULT, "GetEffectInvalidRectangleCount", [(ObjPointer(ID2D1Effect), "effect"), Out(Pointer(UINT32), "rectangleCount")]),
    StdMethod(HRESULT, "GetEffectInvalidRectangles", [(ObjPointer(ID2D1Effect), "effect"), Out(Pointer(D2D1_RECT_F), "rectangles"), (UINT32, "rectanglesCount")]),
    StdMethod(HRESULT, "GetEffectRequiredInputRectangles", [(ObjPointer(ID2D1Effect), "renderEffect"), (Pointer(Const(D2D1_RECT_F)), "renderImageRectangle"), (Pointer(Const(D2D1_EFFECT_INPUT_DESCRIPTION)), "inputDescriptions"), Out(Pointer(D2D1_RECT_F), "requiredInputRects"), (UINT32, "inputCount")]),
    StdMethod(Void, "FillOpacityMask", [(ObjPointer(ID2D1Bitmap), "opacityMask"), (ObjPointer(ID2D1Brush), "brush"), (Pointer(Const(D2D1_RECT_F)), "destinationRectangle"), (Pointer(Const(D2D1_RECT_F)), "sourceRectangle")]),
]

ID2D1Device.methods += [
    StdMethod(HRESULT, "CreateDeviceContext", [(D2D1_DEVICE_CONTEXT_OPTIONS, "options"), Out(Pointer(ObjPointer(ID2D1DeviceContext)), "deviceContext")]),
    StdMethod(HRESULT, "CreatePrintControl", [(Opaque("IWICImagingFactory *"), "wicFactory"), (Opaque("IPrintDocumentPackageTarget *"), "documentTarget"), (Pointer(Const(D2D1_PRINT_CONTROL_PROPERTIES)), "printControlProperties"), Out(Pointer(ObjPointer(ID2D1PrintControl)), "printControl")]),
    StdMethod(Void, "SetMaximumTextureMemory", [(UINT64, "maximumInBytes")]),
    StdMethod(UINT64, "GetMaximumTextureMemory", [], const=True, sideeffects=False),
    StdMethod(Void, "ClearResources", [(UINT32, "millisecondsSinceUse")]),
]

PD2D1_PROPERTY_SET_FUNCTION = Opaque("PD2D1_PROPERTY_SET_FUNCTION")
PD2D1_PROPERTY_GET_FUNCTION = Opaque("PD2D1_PROPERTY_GET_FUNCTION")
D2D1_PROPERTY_BINDING = Struct("D2D1_PROPERTY_BINDING", [
    (PCWSTR, "propertyName"),
    (PD2D1_PROPERTY_SET_FUNCTION, "setFunction"),
    (PD2D1_PROPERTY_GET_FUNCTION, "getFunction"),
])

PD2D1_EFFECT_FACTORY = Opaque("PD2D1_EFFECT_FACTORY")

ID2D1Factory1.methods += [
    StdMethod(HRESULT, "CreateDevice", [(Opaque("IDXGIDevice *"), "dxgiDevice"), Out(Pointer(ObjPointer(ID2D1Device)), "d2dDevice")]),
    StdMethod(HRESULT, "CreateStrokeStyle", [(Pointer(Const(D2D1_STROKE_STYLE_PROPERTIES1)), "strokeStyleProperties"), (Pointer(Const(FLOAT)), "dashes"), (UINT32, "dashesCount"), Out(Pointer(ObjPointer(ID2D1StrokeStyle1)), "strokeStyle")]),
    StdMethod(HRESULT, "CreatePathGeometry", [(Pointer(ObjPointer(ID2D1PathGeometry1)), "pathGeometry")]),
    StdMethod(HRESULT, "CreateDrawingStateBlock", [(Pointer(Const(D2D1_DRAWING_STATE_DESCRIPTION1)), "drawingStateDescription"), (ObjPointer(IDWriteRenderingParams), "textRenderingParams"), Out(Pointer(ObjPointer(ID2D1DrawingStateBlock1)), "drawingStateBlock")]),
    StdMethod(HRESULT, "CreateGdiMetafile", [(Opaque("IStream *"), "metafileStream"), Out(Pointer(ObjPointer(ID2D1GdiMetafile)), "metafile")]),
    StdMethod(HRESULT, "RegisterEffectFromStream", [(REFCLSID, "classId"), (Opaque("IStream *"), "propertyXml"), (Pointer(Const(D2D1_PROPERTY_BINDING)), "bindings"), (UINT32, "bindingsCount"), (Const(PD2D1_EFFECT_FACTORY), "effectFactory")]),
    StdMethod(HRESULT, "RegisterEffectFromString", [(REFCLSID, "classId"), (PCWSTR, "propertyXml"), (Pointer(Const(D2D1_PROPERTY_BINDING)), "bindings"), (UINT32, "bindingsCount"), (Const(PD2D1_EFFECT_FACTORY), "effectFactory")]),
    StdMethod(HRESULT, "UnregisterEffect", [(REFCLSID, "classId")]),
    StdMethod(HRESULT, "GetRegisteredEffects", [Out(Pointer(CLSID), "effects"), (UINT32, "effectsCount"), Out(Pointer(UINT32), "effectsReturned"), Out(Pointer(UINT32), "effectsRegistered")], const=True, sideeffects=False),
    StdMethod(HRESULT, "GetEffectProperties", [(REFCLSID, "effectId"), Out(Pointer(ObjPointer(ID2D1Properties)), "properties")], const=True),
]

ID2D1Multithread.methods += [
    StdMethod(BOOL, "GetMultithreadProtected", [], const=True, sideeffects=False),
    StdMethod(Void, "Enter", []),
    StdMethod(Void, "Leave", []),
]

d2d1.addInterfaces([
    ID2D1Factory1,
    ID2D1Multithread,
])
d2d1.addFunctions([
    StdFunction(HRESULT, "D2D1CreateDevice", [(Opaque("IDXGIDevice *"), "dxgiDevice"), (Pointer(Const(D2D1_CREATION_PROPERTIES)), "creationProperties"), Out(Pointer(ObjPointer(ID2D1Device)), "d2dDevice")]),
    StdFunction(HRESULT, "D2D1CreateDeviceContext", [(ObjPointer(IDXGISurface), "dxgiSurface"), (Pointer(Const(D2D1_CREATION_PROPERTIES)), "creationProperties"), Out(Pointer(ObjPointer(ID2D1DeviceContext)), "d2dDeviceContext")]),
    StdFunction(D2D1_COLOR_F, "D2D1ConvertColorSpace", [(D2D1_COLOR_SPACE, "sourceColorSpace"), (D2D1_COLOR_SPACE, "destinationColorSpace"), (Pointer(Const(D2D1_COLOR_F)), "color")]),
    StdFunction(Void, "D2D1SinCos", [(FLOAT, "angle"), Out(Pointer(FLOAT), "s"), Out(Pointer(FLOAT), "c")]),
    StdFunction(FLOAT, "D2D1Tan", [(FLOAT, "angle")]),
    StdFunction(FLOAT, "D2D1Vec3Length", [(FLOAT, "x"), (FLOAT, "y"), (FLOAT, "z")]),
])
