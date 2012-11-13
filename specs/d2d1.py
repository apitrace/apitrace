##########################################################################
#
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
from dxgiformat import DXGI_FORMAT
from dxgi import IDXGISurface
from d2derr import *
from d2dbasetypes import *
from dwrite import *


ID2D1Resource = Interface("ID2D1Resource", IUnknown)
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

D2D1_DRAW_TEXT_OPTIONS = EnumFlag("D2D1_DRAW_TEXT_OPTIONS", [
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

D2D1_PATH_SEGMENT = EnumFlag("D2D1_PATH_SEGMENT", [
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

D2D1_LAYER_OPTIONS = EnumFlag("D2D1_LAYER_OPTIONS", [
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

D2D1_WINDOW_STATE = EnumFlag("D2D1_WINDOW_STATE", [
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

D2D1_RENDER_TARGET_USAGE = EnumFlag("D2D1_RENDER_TARGET_USAGE", [
    "D2D1_RENDER_TARGET_USAGE_NONE",
    "D2D1_RENDER_TARGET_USAGE_FORCE_BITMAP_REMOTING",
    "D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE",
])

D2D1_PRESENT_OPTIONS = EnumFlag("D2D1_PRESENT_OPTIONS", [
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

D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS = EnumFlag("D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS", [
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
    StdMethod(D2D1_SIZE_F, "GetSize", [], const=True),
    StdMethod(D2D1_SIZE_U, "GetPixelSize", [], const=True),
    StdMethod(D2D1_PIXEL_FORMAT, "GetPixelFormat", [], const=True),
    StdMethod(Void, "GetDpi", [Out(Pointer(FLOAT), "dpiX"), Out(Pointer(FLOAT), "dpiY")], const=True),
    StdMethod(HRESULT, "CopyFromBitmap", [(Pointer(Const(D2D1_POINT_2U)), "destPoint"), (ObjPointer(ID2D1Bitmap), "bitmap"), (Pointer(Const(D2D1_RECT_U)), "srcRect")]),
    StdMethod(HRESULT, "CopyFromRenderTarget", [(Pointer(Const(D2D1_POINT_2U)), "destPoint"), (ObjPointer(ID2D1RenderTarget), "renderTarget"), (Pointer(Const(D2D1_RECT_U)), "srcRect")]),
    StdMethod(HRESULT, "CopyFromMemory", [(Pointer(Const(D2D1_RECT_U)), "dstRect"), (OpaquePointer(Const(Void)), "srcData"), (UINT32, "pitch")]),
]

ID2D1GradientStopCollection.methods += [
    StdMethod(UINT32, "GetGradientStopCount", [], const=True),
    StdMethod(Void, "GetGradientStops", [Out(Pointer(D2D1_GRADIENT_STOP), "gradientStops"), (UINT, "gradientStopsCount")], const=True),
    StdMethod(D2D1_GAMMA, "GetColorInterpolationGamma", [], const=True),
    StdMethod(D2D1_EXTEND_MODE, "GetExtendMode", [], const=True),
]

ID2D1Brush.methods += [
    StdMethod(Void, "SetOpacity", [(FLOAT, "opacity")]),
    StdMethod(Void, "SetTransform", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "transform")]),
    StdMethod(FLOAT, "GetOpacity", [], const=True),
    StdMethod(Void, "GetTransform", [Out(Pointer(D2D1_MATRIX_3X2_F), "transform")], const=True),
]

ID2D1BitmapBrush.methods += [
    StdMethod(Void, "SetExtendModeX", [(D2D1_EXTEND_MODE, "extendModeX")]),
    StdMethod(Void, "SetExtendModeY", [(D2D1_EXTEND_MODE, "extendModeY")]),
    StdMethod(Void, "SetInterpolationMode", [(D2D1_BITMAP_INTERPOLATION_MODE, "interpolationMode")]),
    StdMethod(Void, "SetBitmap", [(ObjPointer(ID2D1Bitmap), "bitmap")]),
    StdMethod(D2D1_EXTEND_MODE, "GetExtendModeX", [], const=True),
    StdMethod(D2D1_EXTEND_MODE, "GetExtendModeY", [], const=True),
    StdMethod(D2D1_BITMAP_INTERPOLATION_MODE, "GetInterpolationMode", [], const=True),
    StdMethod(Void, "GetBitmap", [Out(Pointer(ObjPointer(ID2D1Bitmap)), "bitmap")], const=True),
]

ID2D1SolidColorBrush.methods += [
    StdMethod(Void, "SetColor", [(Pointer(Const(D2D1_COLOR_F)), "color")]),
    StdMethod(D2D1_COLOR_F, "GetColor", [], const=True),
]

ID2D1LinearGradientBrush.methods += [
    StdMethod(Void, "SetStartPoint", [(D2D1_POINT_2F, "startPoint")]),
    StdMethod(Void, "SetEndPoint", [(D2D1_POINT_2F, "endPoint")]),
    StdMethod(D2D1_POINT_2F, "GetStartPoint", [], const=True),
    StdMethod(D2D1_POINT_2F, "GetEndPoint", [], const=True),
    StdMethod(Void, "GetGradientStopCollection", [Out(Pointer(ObjPointer(ID2D1GradientStopCollection)), "gradientStopCollection")], const=True),
]

ID2D1RadialGradientBrush.methods += [
    StdMethod(Void, "SetCenter", [(D2D1_POINT_2F, "center")]),
    StdMethod(Void, "SetGradientOriginOffset", [(D2D1_POINT_2F, "gradientOriginOffset")]),
    StdMethod(Void, "SetRadiusX", [(FLOAT, "radiusX")]),
    StdMethod(Void, "SetRadiusY", [(FLOAT, "radiusY")]),
    StdMethod(D2D1_POINT_2F, "GetCenter", [], const=True),
    StdMethod(D2D1_POINT_2F, "GetGradientOriginOffset", [], const=True),
    StdMethod(FLOAT, "GetRadiusX", [], const=True),
    StdMethod(FLOAT, "GetRadiusY", [], const=True),
    StdMethod(Void, "GetGradientStopCollection", [Out(Pointer(ObjPointer(ID2D1GradientStopCollection)), "gradientStopCollection")], const=True),
]

ID2D1StrokeStyle.methods += [
    StdMethod(D2D1_CAP_STYLE, "GetStartCap", [], const=True),
    StdMethod(D2D1_CAP_STYLE, "GetEndCap", [], const=True),
    StdMethod(D2D1_CAP_STYLE, "GetDashCap", [], const=True),
    StdMethod(FLOAT, "GetMiterLimit", [], const=True),
    StdMethod(D2D1_LINE_JOIN, "GetLineJoin", [], const=True),
    StdMethod(FLOAT, "GetDashOffset", [], const=True),
    StdMethod(D2D1_DASH_STYLE, "GetDashStyle", [], const=True),
    StdMethod(UINT32, "GetDashesCount", [], const=True),
    StdMethod(Void, "GetDashes", [Out(Array(FLOAT, "dashesCount"), "dashes"), (UINT, "dashesCount")], const=True),
]

ID2D1Geometry.methods += [
    StdMethod(HRESULT, "GetBounds", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), Out(Pointer(D2D1_RECT_F), "bounds")], const=True),
    StdMethod(HRESULT, "GetWidenedBounds", [(FLOAT, "strokeWidth"), (ObjPointer(ID2D1StrokeStyle), "strokeStyle"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(D2D1_RECT_F), "bounds")], const=True),
    StdMethod(HRESULT, "StrokeContainsPoint", [(D2D1_POINT_2F, "point"), (FLOAT, "strokeWidth"), (ObjPointer(ID2D1StrokeStyle), "strokeStyle"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(BOOL), "contains")], const=True),
    StdMethod(HRESULT, "FillContainsPoint", [(D2D1_POINT_2F, "point"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(BOOL), "contains")], const=True),
    StdMethod(HRESULT, "CompareWithGeometry", [(ObjPointer(ID2D1Geometry), "inputGeometry"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "inputGeometryTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(D2D1_GEOMETRY_RELATION), "relation")], const=True),
    StdMethod(HRESULT, "Simplify", [(D2D1_GEOMETRY_SIMPLIFICATION_OPTION, "simplificationOption"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), (ObjPointer(ID2D1SimplifiedGeometrySink), "geometrySink")], const=True),
    StdMethod(HRESULT, "Tessellate", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), (ObjPointer(ID2D1TessellationSink), "tessellationSink")], const=True),
    StdMethod(HRESULT, "CombineWithGeometry", [(ObjPointer(ID2D1Geometry), "inputGeometry"), (D2D1_COMBINE_MODE, "combineMode"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "inputGeometryTransform"), (FLOAT, "flatteningTolerance"), (ObjPointer(ID2D1SimplifiedGeometrySink), "geometrySink")], const=True),
    StdMethod(HRESULT, "Outline", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), (ObjPointer(ID2D1SimplifiedGeometrySink), "geometrySink")], const=True),
    StdMethod(HRESULT, "ComputeArea", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(FLOAT), "area")], const=True),
    StdMethod(HRESULT, "ComputeLength", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(FLOAT), "length")], const=True),
    StdMethod(HRESULT, "ComputePointAtLength", [(FLOAT, "length"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(D2D1_POINT_2F), "point"), Out(Pointer(D2D1_POINT_2F), "unitTangentVector")], const=True),
    StdMethod(HRESULT, "Widen", [(FLOAT, "strokeWidth"), (ObjPointer(ID2D1StrokeStyle), "strokeStyle"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), (ObjPointer(ID2D1SimplifiedGeometrySink), "geometrySink")], const=True),
]

ID2D1RectangleGeometry.methods += [
    StdMethod(Void, "GetRect", [Out(Pointer(D2D1_RECT_F), "rect")], const=True),
]

ID2D1RoundedRectangleGeometry.methods += [
    StdMethod(Void, "GetRoundedRect", [Out(Pointer(D2D1_ROUNDED_RECT), "roundedRect")], const=True),
]

ID2D1EllipseGeometry.methods += [
    StdMethod(Void, "GetEllipse", [Out(Pointer(D2D1_ELLIPSE), "ellipse")], const=True),
]

ID2D1GeometryGroup.methods += [
    StdMethod(D2D1_FILL_MODE, "GetFillMode", [], const=True),
    StdMethod(UINT32, "GetSourceGeometryCount", [], const=True),
    StdMethod(Void, "GetSourceGeometries", [Out(Array(ObjPointer(ID2D1Geometry), "geometriesCount"), "geometries"), (UINT, "geometriesCount")], const=True),
]

ID2D1TransformedGeometry.methods += [
    StdMethod(Void, "GetSourceGeometry", [Out(Pointer(ObjPointer(ID2D1Geometry)), "sourceGeometry")], const=True),
    StdMethod(Void, "GetTransform", [Out(Pointer(D2D1_MATRIX_3X2_F), "transform")], const=True),
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
    StdMethod(HRESULT, "Stream", [(ObjPointer(ID2D1GeometrySink), "geometrySink")], const=True),
    StdMethod(HRESULT, "GetSegmentCount", [Out(Pointer(UINT32), "count")], const=True),
    StdMethod(HRESULT, "GetFigureCount", [Out(Pointer(UINT32), "count")], const=True),
]

ID2D1Mesh.methods += [
    StdMethod(HRESULT, "Open", [Out(Pointer(ObjPointer(ID2D1TessellationSink)), "tessellationSink")]),
]

ID2D1Layer.methods += [
    StdMethod(D2D1_SIZE_F, "GetSize", [], const=True),
]

ID2D1DrawingStateBlock.methods += [
    StdMethod(Void, "GetDescription", [Out(Pointer(D2D1_DRAWING_STATE_DESCRIPTION), "stateDescription")], const=True),
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
    StdMethod(Void, "DrawText", [(Pointer(Const(WCHAR)), "string"), (UINT, "stringLength"), (ObjPointer(IDWriteTextFormat), "textFormat"), (Pointer(Const(D2D1_RECT_F)), "layoutRect"), (ObjPointer(ID2D1Brush), "defaultForegroundBrush"), (D2D1_DRAW_TEXT_OPTIONS, "options"), (DWRITE_MEASURING_MODE, "measuringMode")]),
    StdMethod(Void, "DrawTextLayout", [(D2D1_POINT_2F, "origin"), (ObjPointer(IDWriteTextLayout), "textLayout"), (ObjPointer(ID2D1Brush), "defaultForegroundBrush"), (D2D1_DRAW_TEXT_OPTIONS, "options")]),
    StdMethod(Void, "DrawGlyphRun", [(D2D1_POINT_2F, "baselineOrigin"), (Pointer(Const(DWRITE_GLYPH_RUN)), "glyphRun"), (ObjPointer(ID2D1Brush), "foregroundBrush"), (DWRITE_MEASURING_MODE, "measuringMode")]),
    StdMethod(Void, "SetTransform", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "transform")]),
    StdMethod(Void, "GetTransform", [Out(Pointer(D2D1_MATRIX_3X2_F), "transform")], const=True),
    StdMethod(Void, "SetAntialiasMode", [(D2D1_ANTIALIAS_MODE, "antialiasMode")]),
    StdMethod(D2D1_ANTIALIAS_MODE, "GetAntialiasMode", [], const=True),
    StdMethod(Void, "SetTextAntialiasMode", [(D2D1_TEXT_ANTIALIAS_MODE, "textAntialiasMode")]),
    StdMethod(D2D1_TEXT_ANTIALIAS_MODE, "GetTextAntialiasMode", [], const=True),
    StdMethod(Void, "SetTextRenderingParams", [(ObjPointer(IDWriteRenderingParams), "textRenderingParams")]),
    StdMethod(Void, "GetTextRenderingParams", [Out(Pointer(ObjPointer(IDWriteRenderingParams)), "textRenderingParams")], const=True),
    StdMethod(Void, "SetTags", [(D2D1_TAG, "tag1"), (D2D1_TAG, "tag2")]),
    StdMethod(Void, "GetTags", [Out(Pointer(D2D1_TAG), "tag1"), Out(Pointer(D2D1_TAG), "tag2")], const=True),
    StdMethod(Void, "PushLayer", [(Pointer(Const(D2D1_LAYER_PARAMETERS)), "layerParameters"), (ObjPointer(ID2D1Layer), "layer")]),
    StdMethod(Void, "PopLayer", []),
    StdMethod(HRESULT, "Flush", [Out(Pointer(D2D1_TAG), "tag1"), Out(Pointer(D2D1_TAG), "tag2")]),
    StdMethod(Void, "SaveDrawingState", [(ObjPointer(ID2D1DrawingStateBlock), "drawingStateBlock")], const=True),
    StdMethod(Void, "RestoreDrawingState", [(ObjPointer(ID2D1DrawingStateBlock), "drawingStateBlock")]),
    StdMethod(Void, "PushAxisAlignedClip", [(Pointer(Const(D2D1_RECT_F)), "clipRect"), (D2D1_ANTIALIAS_MODE, "antialiasMode")]),
    StdMethod(Void, "PopAxisAlignedClip", []),
    StdMethod(Void, "Clear", [(Pointer(Const(D2D1_COLOR_F)), "clearColor")]),
    StdMethod(Void, "BeginDraw", []),
    StdMethod(HRESULT, "EndDraw", [Out(Pointer(D2D1_TAG), "tag1"), Out(Pointer(D2D1_TAG), "tag2")]),
    StdMethod(D2D1_PIXEL_FORMAT, "GetPixelFormat", [], const=True),
    StdMethod(Void, "SetDpi", [(FLOAT, "dpiX"), (FLOAT, "dpiY")]),
    StdMethod(Void, "GetDpi", [Out(Pointer(FLOAT), "dpiX"), Out(Pointer(FLOAT), "dpiY")], const=True),
    StdMethod(D2D1_SIZE_F, "GetSize", [], const=True),
    StdMethod(D2D1_SIZE_U, "GetPixelSize", [], const=True),
    StdMethod(UINT32, "GetMaximumBitmapSize", [], const=True),
    StdMethod(BOOL, "IsSupported", [(Pointer(Const(D2D1_RENDER_TARGET_PROPERTIES)), "renderTargetProperties")], const=True),
]

ID2D1BitmapRenderTarget.methods += [
    StdMethod(HRESULT, "GetBitmap", [Out(Pointer(ObjPointer(ID2D1Bitmap)), "bitmap")]),
]

ID2D1HwndRenderTarget.methods += [
    StdMethod(D2D1_WINDOW_STATE, "CheckWindowState", []),
    StdMethod(HRESULT, "Resize", [(Pointer(Const(D2D1_SIZE_U)), "pixelSize")]),
    StdMethod(HWND, "GetHwnd", [], const=True),
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
    StdMethod(HRESULT, "CreateGeometryGroup", [(D2D1_FILL_MODE, "fillMode"), (Pointer(ObjPointer(ID2D1Geometry)), "geometries"), (UINT, "geometriesCount"), Out(Pointer(ObjPointer(ID2D1GeometryGroup)), "geometryGroup")]),
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
