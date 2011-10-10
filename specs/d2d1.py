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
    "D2D1_DRAW_TEXT_OPTIONS_NO_CLIP",
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
    (Pointer(ID2D1Geometry), "geometricMask"),
    (D2D1_ANTIALIAS_MODE, "maskAntialiasMode"),
    (D2D1_MATRIX_3X2_F, "maskTransform"),
    (FLOAT, "opacity"),
    (Pointer(ID2D1Brush), "opacityBrush"),
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
    Method(Void, "GetFactory", [Out(Pointer(Pointer(ID2D1Factory)), "factory")], const=True),
]

ID2D1Bitmap.methods += [
    Method(D2D1_SIZE_F, "GetSize", [], const=True),
    Method(D2D1_SIZE_U, "GetPixelSize", [], const=True),
    Method(D2D1_PIXEL_FORMAT, "GetPixelFormat", [], const=True),
    Method(Void, "GetDpi", [Out(Pointer(FLOAT), "dpiX"), Out(Pointer(FLOAT), "dpiY")], const=True),
    Method(HRESULT, "CopyFromBitmap", [(Pointer(Const(D2D1_POINT_2U)), "destPoint"), (Pointer(ID2D1Bitmap), "bitmap"), (Pointer(Const(D2D1_RECT_U)), "srcRect")]),
    Method(HRESULT, "CopyFromRenderTarget", [(Pointer(Const(D2D1_POINT_2U)), "destPoint"), (Pointer(ID2D1RenderTarget), "renderTarget"), (Pointer(Const(D2D1_RECT_U)), "srcRect")]),
    Method(HRESULT, "CopyFromMemory", [(Pointer(Const(D2D1_RECT_U)), "dstRect"), (OpaquePointer(Const(Void)), "srcData"), (UINT32, "pitch")]),
]

ID2D1GradientStopCollection.methods += [
    Method(UINT32, "GetGradientStopCount", [], const=True),
    Method(Void, "GetGradientStops", [Out(Pointer(D2D1_GRADIENT_STOP), "gradientStops"), (UINT, "gradientStopsCount")], const=True),
    Method(D2D1_GAMMA, "GetColorInterpolationGamma", [], const=True),
    Method(D2D1_EXTEND_MODE, "GetExtendMode", [], const=True),
]

ID2D1Brush.methods += [
    Method(Void, "SetOpacity", [(FLOAT, "opacity")]),
    Method(Void, "SetTransform", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "transform")]),
    Method(FLOAT, "GetOpacity", [], const=True),
    Method(Void, "GetTransform", [Out(Pointer(D2D1_MATRIX_3X2_F), "transform")], const=True),
]

ID2D1BitmapBrush.methods += [
    Method(Void, "SetExtendModeX", [(D2D1_EXTEND_MODE, "extendModeX")]),
    Method(Void, "SetExtendModeY", [(D2D1_EXTEND_MODE, "extendModeY")]),
    Method(Void, "SetInterpolationMode", [(D2D1_BITMAP_INTERPOLATION_MODE, "interpolationMode")]),
    Method(Void, "SetBitmap", [(Pointer(ID2D1Bitmap), "bitmap")]),
    Method(D2D1_EXTEND_MODE, "GetExtendModeX", [], const=True),
    Method(D2D1_EXTEND_MODE, "GetExtendModeY", [], const=True),
    Method(D2D1_BITMAP_INTERPOLATION_MODE, "GetInterpolationMode", [], const=True),
    Method(Void, "GetBitmap", [Out(Pointer(Pointer(ID2D1Bitmap)), "bitmap")], const=True),
]

ID2D1SolidColorBrush.methods += [
    Method(Void, "SetColor", [(Pointer(Const(D2D1_COLOR_F)), "color")]),
    Method(D2D1_COLOR_F, "GetColor", [], const=True),
]

ID2D1LinearGradientBrush.methods += [
    Method(Void, "SetStartPoint", [(D2D1_POINT_2F, "startPoint")]),
    Method(Void, "SetEndPoint", [(D2D1_POINT_2F, "endPoint")]),
    Method(D2D1_POINT_2F, "GetStartPoint", [], const=True),
    Method(D2D1_POINT_2F, "GetEndPoint", [], const=True),
    Method(Void, "GetGradientStopCollection", [Out(Pointer(Pointer(ID2D1GradientStopCollection)), "gradientStopCollection")], const=True),
]

ID2D1RadialGradientBrush.methods += [
    Method(Void, "SetCenter", [(D2D1_POINT_2F, "center")]),
    Method(Void, "SetGradientOriginOffset", [(D2D1_POINT_2F, "gradientOriginOffset")]),
    Method(Void, "SetRadiusX", [(FLOAT, "radiusX")]),
    Method(Void, "SetRadiusY", [(FLOAT, "radiusY")]),
    Method(D2D1_POINT_2F, "GetCenter", [], const=True),
    Method(D2D1_POINT_2F, "GetGradientOriginOffset", [], const=True),
    Method(FLOAT, "GetRadiusX", [], const=True),
    Method(FLOAT, "GetRadiusY", [], const=True),
    Method(Void, "GetGradientStopCollection", [Out(Pointer(Pointer(ID2D1GradientStopCollection)), "gradientStopCollection")], const=True),
]

ID2D1StrokeStyle.methods += [
    Method(D2D1_CAP_STYLE, "GetStartCap", [], const=True),
    Method(D2D1_CAP_STYLE, "GetEndCap", [], const=True),
    Method(D2D1_CAP_STYLE, "GetDashCap", [], const=True),
    Method(FLOAT, "GetMiterLimit", [], const=True),
    Method(D2D1_LINE_JOIN, "GetLineJoin", [], const=True),
    Method(FLOAT, "GetDashOffset", [], const=True),
    Method(D2D1_DASH_STYLE, "GetDashStyle", [], const=True),
    Method(UINT32, "GetDashesCount", [], const=True),
    Method(Void, "GetDashes", [Out(Pointer(FLOAT), "dashes"), (UINT, "dashesCount")], const=True),
]

ID2D1Geometry.methods += [
    Method(HRESULT, "GetBounds", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), Out(Pointer(D2D1_RECT_F), "bounds")], const=True),
    Method(HRESULT, "GetWidenedBounds", [(FLOAT, "strokeWidth"), (Pointer(ID2D1StrokeStyle), "strokeStyle"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(D2D1_RECT_F), "bounds")], const=True),
    Method(HRESULT, "StrokeContainsPoint", [(D2D1_POINT_2F, "point"), (FLOAT, "strokeWidth"), (Pointer(ID2D1StrokeStyle), "strokeStyle"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(BOOL), "contains")], const=True),
    Method(HRESULT, "FillContainsPoint", [(D2D1_POINT_2F, "point"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(BOOL), "contains")], const=True),
    Method(HRESULT, "CompareWithGeometry", [(Pointer(ID2D1Geometry), "inputGeometry"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "inputGeometryTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(D2D1_GEOMETRY_RELATION), "relation")], const=True),
    Method(HRESULT, "Simplify", [(D2D1_GEOMETRY_SIMPLIFICATION_OPTION, "simplificationOption"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), (Pointer(ID2D1SimplifiedGeometrySink), "geometrySink")], const=True),
    Method(HRESULT, "Tessellate", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), (Pointer(ID2D1TessellationSink), "tessellationSink")], const=True),
    Method(HRESULT, "CombineWithGeometry", [(Pointer(ID2D1Geometry), "inputGeometry"), (D2D1_COMBINE_MODE, "combineMode"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "inputGeometryTransform"), (FLOAT, "flatteningTolerance"), (Pointer(ID2D1SimplifiedGeometrySink), "geometrySink")], const=True),
    Method(HRESULT, "Outline", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), (Pointer(ID2D1SimplifiedGeometrySink), "geometrySink")], const=True),
    Method(HRESULT, "ComputeArea", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(FLOAT), "area")], const=True),
    Method(HRESULT, "ComputeLength", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(FLOAT), "length")], const=True),
    Method(HRESULT, "ComputePointAtLength", [(FLOAT, "length"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), Out(Pointer(D2D1_POINT_2F), "point"), Out(Pointer(D2D1_POINT_2F), "unitTangentVector")], const=True),
    Method(HRESULT, "Widen", [(FLOAT, "strokeWidth"), (Pointer(ID2D1StrokeStyle), "strokeStyle"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "worldTransform"), (FLOAT, "flatteningTolerance"), (Pointer(ID2D1SimplifiedGeometrySink), "geometrySink")], const=True),
]

ID2D1RectangleGeometry.methods += [
    Method(Void, "GetRect", [Out(Pointer(D2D1_RECT_F), "rect")], const=True),
]

ID2D1RoundedRectangleGeometry.methods += [
    Method(Void, "GetRoundedRect", [Out(Pointer(D2D1_ROUNDED_RECT), "roundedRect")], const=True),
]

ID2D1EllipseGeometry.methods += [
    Method(Void, "GetEllipse", [Out(Pointer(D2D1_ELLIPSE), "ellipse")], const=True),
]

ID2D1GeometryGroup.methods += [
    Method(D2D1_FILL_MODE, "GetFillMode", [], const=True),
    Method(UINT32, "GetSourceGeometryCount", [], const=True),
    Method(Void, "GetSourceGeometries", [Out(Pointer(Pointer(ID2D1Geometry)), "geometries"), (UINT, "geometriesCount")], const=True),
]

ID2D1TransformedGeometry.methods += [
    Method(Void, "GetSourceGeometry", [Out(Pointer(Pointer(ID2D1Geometry)), "sourceGeometry")], const=True),
    Method(Void, "GetTransform", [Out(Pointer(D2D1_MATRIX_3X2_F), "transform")], const=True),
]

ID2D1SimplifiedGeometrySink.methods += [
    Method(Void, "SetFillMode", [(D2D1_FILL_MODE, "fillMode")]),
    Method(Void, "SetSegmentFlags", [(D2D1_PATH_SEGMENT, "vertexFlags")]),
    Method(Void, "BeginFigure", [(D2D1_POINT_2F, "startPoint"), (D2D1_FIGURE_BEGIN, "figureBegin")]),
    Method(Void, "AddLines", [(Pointer(Const(D2D1_POINT_2F)), "points"), (UINT, "pointsCount")]),
    Method(Void, "AddBeziers", [(Pointer(Const(D2D1_BEZIER_SEGMENT)), "beziers"), (UINT, "beziersCount")]),
    Method(Void, "EndFigure", [(D2D1_FIGURE_END, "figureEnd")]),
    Method(HRESULT, "Close", []),
]

ID2D1GeometrySink.methods += [
    Method(Void, "AddLine", [(D2D1_POINT_2F, "point")]),
    Method(Void, "AddBezier", [(Pointer(Const(D2D1_BEZIER_SEGMENT)), "bezier")]),
    Method(Void, "AddQuadraticBezier", [(Pointer(Const(D2D1_QUADRATIC_BEZIER_SEGMENT)), "bezier")]),
    Method(Void, "AddQuadraticBeziers", [(Pointer(Const(D2D1_QUADRATIC_BEZIER_SEGMENT)), "beziers"), (UINT, "beziersCount")]),
    Method(Void, "AddArc", [(Pointer(Const(D2D1_ARC_SEGMENT)), "arc")]),
]

ID2D1TessellationSink.methods += [
    Method(Void, "AddTriangles", [(Pointer(Const(D2D1_TRIANGLE)), "triangles"), (UINT, "trianglesCount")]),
    Method(HRESULT, "Close", []),
]

ID2D1PathGeometry.methods += [
    Method(HRESULT, "Open", [Out(Pointer(Pointer(ID2D1GeometrySink)), "geometrySink")]),
    Method(HRESULT, "Stream", [(Pointer(ID2D1GeometrySink), "geometrySink")], const=True),
    Method(HRESULT, "GetSegmentCount", [Out(Pointer(UINT32), "count")], const=True),
    Method(HRESULT, "GetFigureCount", [Out(Pointer(UINT32), "count")], const=True),
]

ID2D1Mesh.methods += [
    Method(HRESULT, "Open", [Out(Pointer(Pointer(ID2D1TessellationSink)), "tessellationSink")]),
]

ID2D1Layer.methods += [
    Method(D2D1_SIZE_F, "GetSize", [], const=True),
]

ID2D1DrawingStateBlock.methods += [
    Method(Void, "GetDescription", [Out(Pointer(D2D1_DRAWING_STATE_DESCRIPTION), "stateDescription")], const=True),
    Method(Void, "SetDescription", [(Pointer(Const(D2D1_DRAWING_STATE_DESCRIPTION)), "stateDescription")]),
    Method(Void, "SetTextRenderingParams", [(Pointer(IDWriteRenderingParams), "textRenderingParams")]),
    Method(Void, "GetTextRenderingParams", [Out(Pointer(Pointer(IDWriteRenderingParams)), "textRenderingParams")], const=True),
]

ID2D1RenderTarget.methods += [
    Method(HRESULT, "CreateBitmap", [(D2D1_SIZE_U, "size"), (OpaquePointer(Const(Void)), "srcData"), (UINT32, "pitch"), (Pointer(Const(D2D1_BITMAP_PROPERTIES)), "bitmapProperties"), Out(Pointer(Pointer(ID2D1Bitmap)), "bitmap")]),
    Method(HRESULT, "CreateBitmapFromWicBitmap", [(Opaque("IWICBitmapSource *"), "wicBitmapSource"), (Pointer(Const(D2D1_BITMAP_PROPERTIES)), "bitmapProperties"), Out(Pointer(Pointer(ID2D1Bitmap)), "bitmap")]),
    Method(HRESULT, "CreateSharedBitmap", [(REFIID, "riid"), Out(OpaquePointer(Void), "data"), (Pointer(Const(D2D1_BITMAP_PROPERTIES)), "bitmapProperties"), Out(Pointer(Pointer(ID2D1Bitmap)), "bitmap")]),
    Method(HRESULT, "CreateBitmapBrush", [(Pointer(ID2D1Bitmap), "bitmap"), (Pointer(Const(D2D1_BITMAP_BRUSH_PROPERTIES)), "bitmapBrushProperties"), (Pointer(Const(D2D1_BRUSH_PROPERTIES)), "brushProperties"), Out(Pointer(Pointer(ID2D1BitmapBrush)), "bitmapBrush")]),
    Method(HRESULT, "CreateSolidColorBrush", [(Pointer(Const(D2D1_COLOR_F)), "color"), (Pointer(Const(D2D1_BRUSH_PROPERTIES)), "brushProperties"), Out(Pointer(Pointer(ID2D1SolidColorBrush)), "solidColorBrush")]),
    Method(HRESULT, "CreateGradientStopCollection", [(Pointer(Const(D2D1_GRADIENT_STOP)), "gradientStops"), (UINT, "gradientStopsCount"), (D2D1_GAMMA, "colorInterpolationGamma"), (D2D1_EXTEND_MODE, "extendMode"), Out(Pointer(Pointer(ID2D1GradientStopCollection)), "gradientStopCollection")]),
    Method(HRESULT, "CreateLinearGradientBrush", [(Pointer(Const(D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES)), "linearGradientBrushProperties"), (Pointer(Const(D2D1_BRUSH_PROPERTIES)), "brushProperties"), (Pointer(ID2D1GradientStopCollection), "gradientStopCollection"), Out(Pointer(Pointer(ID2D1LinearGradientBrush)), "linearGradientBrush")]),
    Method(HRESULT, "CreateRadialGradientBrush", [(Pointer(Const(D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES)), "radialGradientBrushProperties"), (Pointer(Const(D2D1_BRUSH_PROPERTIES)), "brushProperties"), (Pointer(ID2D1GradientStopCollection), "gradientStopCollection"), Out(Pointer(Pointer(ID2D1RadialGradientBrush)), "radialGradientBrush")]),
    Method(HRESULT, "CreateCompatibleRenderTarget", [(Pointer(Const(D2D1_SIZE_F)), "desiredSize"), (Pointer(Const(D2D1_SIZE_U)), "desiredPixelSize"), (Pointer(Const(D2D1_PIXEL_FORMAT)), "desiredFormat"), (D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS, "options"), Out(Pointer(Pointer(ID2D1BitmapRenderTarget)), "bitmapRenderTarget")]),
    Method(HRESULT, "CreateLayer", [(Pointer(Const(D2D1_SIZE_F)), "size"), Out(Pointer(Pointer(ID2D1Layer)), "layer")]),
    Method(HRESULT, "CreateMesh", [Out(Pointer(Pointer(ID2D1Mesh)), "mesh")]),
    Method(Void, "DrawLine", [(D2D1_POINT_2F, "point0"), (D2D1_POINT_2F, "point1"), (Pointer(ID2D1Brush), "brush"), (FLOAT, "strokeWidth"), (Pointer(ID2D1StrokeStyle), "strokeStyle")]),
    Method(Void, "DrawRectangle", [(Pointer(Const(D2D1_RECT_F)), "rect"), (Pointer(ID2D1Brush), "brush"), (FLOAT, "strokeWidth"), (Pointer(ID2D1StrokeStyle), "strokeStyle")]),
    Method(Void, "FillRectangle", [(Pointer(Const(D2D1_RECT_F)), "rect"), (Pointer(ID2D1Brush), "brush")]),
    Method(Void, "DrawRoundedRectangle", [(Pointer(Const(D2D1_ROUNDED_RECT)), "roundedRect"), (Pointer(ID2D1Brush), "brush"), (FLOAT, "strokeWidth"), (Pointer(ID2D1StrokeStyle), "strokeStyle")]),
    Method(Void, "FillRoundedRectangle", [(Pointer(Const(D2D1_ROUNDED_RECT)), "roundedRect"), (Pointer(ID2D1Brush), "brush")]),
    Method(Void, "DrawEllipse", [(Pointer(Const(D2D1_ELLIPSE)), "ellipse"), (Pointer(ID2D1Brush), "brush"), (FLOAT, "strokeWidth"), (Pointer(ID2D1StrokeStyle), "strokeStyle")]),
    Method(Void, "FillEllipse", [(Pointer(Const(D2D1_ELLIPSE)), "ellipse"), (Pointer(ID2D1Brush), "brush")]),
    Method(Void, "DrawGeometry", [(Pointer(ID2D1Geometry), "geometry"), (Pointer(ID2D1Brush), "brush"), (FLOAT, "strokeWidth"), (Pointer(ID2D1StrokeStyle), "strokeStyle")]),
    Method(Void, "FillGeometry", [(Pointer(ID2D1Geometry), "geometry"), (Pointer(ID2D1Brush), "brush"), (Pointer(ID2D1Brush), "opacityBrush")]),
    Method(Void, "FillMesh", [(Pointer(ID2D1Mesh), "mesh"), (Pointer(ID2D1Brush), "brush")]),
    Method(Void, "FillOpacityMask", [(Pointer(ID2D1Bitmap), "opacityMask"), (Pointer(ID2D1Brush), "brush"), (D2D1_OPACITY_MASK_CONTENT, "content"), (Pointer(Const(D2D1_RECT_F)), "destinationRectangle"), (Pointer(Const(D2D1_RECT_F)), "sourceRectangle")]),
    Method(Void, "DrawBitmap", [(Pointer(ID2D1Bitmap), "bitmap"), (Pointer(Const(D2D1_RECT_F)), "destinationRectangle"), (FLOAT, "opacity"), (D2D1_BITMAP_INTERPOLATION_MODE, "interpolationMode"), (Pointer(Const(D2D1_RECT_F)), "sourceRectangle")]),
    Method(Void, "DrawText", [(Pointer(Const(WCHAR)), "string"), (UINT, "stringLength"), (Pointer(IDWriteTextFormat), "textFormat"), (Pointer(Const(D2D1_RECT_F)), "layoutRect"), (Pointer(ID2D1Brush), "defaultForegroundBrush"), (D2D1_DRAW_TEXT_OPTIONS, "options"), (DWRITE_MEASURING_MODE, "measuringMode")]),
    Method(Void, "DrawTextLayout", [(D2D1_POINT_2F, "origin"), (Pointer(IDWriteTextLayout), "textLayout"), (Pointer(ID2D1Brush), "defaultForegroundBrush"), (D2D1_DRAW_TEXT_OPTIONS, "options")]),
    Method(Void, "DrawGlyphRun", [(D2D1_POINT_2F, "baselineOrigin"), (Pointer(Const(DWRITE_GLYPH_RUN)), "glyphRun"), (Pointer(ID2D1Brush), "foregroundBrush"), (DWRITE_MEASURING_MODE, "measuringMode")]),
    Method(Void, "SetTransform", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "transform")]),
    Method(Void, "GetTransform", [Out(Pointer(D2D1_MATRIX_3X2_F), "transform")], const=True),
    Method(Void, "SetAntialiasMode", [(D2D1_ANTIALIAS_MODE, "antialiasMode")]),
    Method(D2D1_ANTIALIAS_MODE, "GetAntialiasMode", [], const=True),
    Method(Void, "SetTextAntialiasMode", [(D2D1_TEXT_ANTIALIAS_MODE, "textAntialiasMode")]),
    Method(D2D1_TEXT_ANTIALIAS_MODE, "GetTextAntialiasMode", [], const=True),
    Method(Void, "SetTextRenderingParams", [(Pointer(IDWriteRenderingParams), "textRenderingParams")]),
    Method(Void, "GetTextRenderingParams", [Out(Pointer(Pointer(IDWriteRenderingParams)), "textRenderingParams")], const=True),
    Method(Void, "SetTags", [(D2D1_TAG, "tag1"), (D2D1_TAG, "tag2")]),
    Method(Void, "GetTags", [Out(Pointer(D2D1_TAG), "tag1"), Out(Pointer(D2D1_TAG), "tag2")], const=True),
    Method(Void, "PushLayer", [(Pointer(Const(D2D1_LAYER_PARAMETERS)), "layerParameters"), (Pointer(ID2D1Layer), "layer")]),
    Method(Void, "PopLayer", []),
    Method(HRESULT, "Flush", [Out(Pointer(D2D1_TAG), "tag1"), Out(Pointer(D2D1_TAG), "tag2")]),
    Method(Void, "SaveDrawingState", [Out(Pointer(ID2D1DrawingStateBlock), "drawingStateBlock")], const=True),
    Method(Void, "RestoreDrawingState", [(Pointer(ID2D1DrawingStateBlock), "drawingStateBlock")]),
    Method(Void, "PushAxisAlignedClip", [(Pointer(Const(D2D1_RECT_F)), "clipRect"), (D2D1_ANTIALIAS_MODE, "antialiasMode")]),
    Method(Void, "PopAxisAlignedClip", []),
    Method(Void, "Clear", [(Pointer(Const(D2D1_COLOR_F)), "clearColor")]),
    Method(Void, "BeginDraw", []),
    Method(HRESULT, "EndDraw", [Out(Pointer(D2D1_TAG), "tag1"), Out(Pointer(D2D1_TAG), "tag2")]),
    Method(D2D1_PIXEL_FORMAT, "GetPixelFormat", [], const=True),
    Method(Void, "SetDpi", [(FLOAT, "dpiX"), (FLOAT, "dpiY")]),
    Method(Void, "GetDpi", [Out(Pointer(FLOAT), "dpiX"), Out(Pointer(FLOAT), "dpiY")], const=True),
    Method(D2D1_SIZE_F, "GetSize", [], const=True),
    Method(D2D1_SIZE_U, "GetPixelSize", [], const=True),
    Method(UINT32, "GetMaximumBitmapSize", [], const=True),
    Method(BOOL, "IsSupported", [(Pointer(Const(D2D1_RENDER_TARGET_PROPERTIES)), "renderTargetProperties")], const=True),
]

ID2D1BitmapRenderTarget.methods += [
    Method(HRESULT, "GetBitmap", [Out(Pointer(Pointer(ID2D1Bitmap)), "bitmap")]),
]

ID2D1HwndRenderTarget.methods += [
    Method(D2D1_WINDOW_STATE, "CheckWindowState", []),
    Method(HRESULT, "Resize", [(Pointer(Const(D2D1_SIZE_U)), "pixelSize")]),
    Method(HWND, "GetHwnd", [], const=True),
]

ID2D1GdiInteropRenderTarget.methods += [
    Method(HRESULT, "GetDC", [(D2D1_DC_INITIALIZE_MODE, "mode"), Out(Pointer(HDC), "hdc")]),
    Method(HRESULT, "ReleaseDC", [(Pointer(Const(RECT)), "update")]),
]

ID2D1DCRenderTarget.methods += [
    Method(HRESULT, "BindDC", [(Const(HDC), "hDC"), (Pointer(Const(RECT)), "pSubRect")]),
]

ID2D1Factory.methods += [
    Method(HRESULT, "ReloadSystemMetrics", []),
    Method(Void, "GetDesktopDpi", [Out(Pointer(FLOAT), "dpiX"), Out(Pointer(FLOAT), "dpiY")]),
    Method(HRESULT, "CreateRectangleGeometry", [(Pointer(Const(D2D1_RECT_F)), "rectangle"), Out(Pointer(Pointer(ID2D1RectangleGeometry)), "rectangleGeometry")]),
    Method(HRESULT, "CreateRoundedRectangleGeometry", [(Pointer(Const(D2D1_ROUNDED_RECT)), "roundedRectangle"), Out(Pointer(Pointer(ID2D1RoundedRectangleGeometry)), "roundedRectangleGeometry")]),
    Method(HRESULT, "CreateEllipseGeometry", [(Pointer(Const(D2D1_ELLIPSE)), "ellipse"), Out(Pointer(Pointer(ID2D1EllipseGeometry)), "ellipseGeometry")]),
    Method(HRESULT, "CreateGeometryGroup", [(D2D1_FILL_MODE, "fillMode"), (Pointer(Pointer(ID2D1Geometry)), "geometries"), (UINT, "geometriesCount"), Out(Pointer(Pointer(ID2D1GeometryGroup)), "geometryGroup")]),
    Method(HRESULT, "CreateTransformedGeometry", [(Pointer(ID2D1Geometry), "sourceGeometry"), (Pointer(Const(D2D1_MATRIX_3X2_F)), "transform"), Out(Pointer(Pointer(ID2D1TransformedGeometry)), "transformedGeometry")]),
    Method(HRESULT, "CreatePathGeometry", [Out(Pointer(Pointer(ID2D1PathGeometry)), "pathGeometry")]),
    Method(HRESULT, "CreateStrokeStyle", [(Pointer(Const(D2D1_STROKE_STYLE_PROPERTIES)), "strokeStyleProperties"), (Pointer(Const(FLOAT)), "dashes"), (UINT, "dashesCount"), Out(Pointer(Pointer(ID2D1StrokeStyle)), "strokeStyle")]),
    Method(HRESULT, "CreateDrawingStateBlock", [(Pointer(Const(D2D1_DRAWING_STATE_DESCRIPTION)), "drawingStateDescription"), (Pointer(IDWriteRenderingParams), "textRenderingParams"), Out(Pointer(Pointer(ID2D1DrawingStateBlock)), "drawingStateBlock")]),
    Method(HRESULT, "CreateWicBitmapRenderTarget", [(Opaque("IWICBitmap *"), "target"), (Pointer(Const(D2D1_RENDER_TARGET_PROPERTIES)), "renderTargetProperties"), Out(Pointer(Pointer(ID2D1RenderTarget)), "renderTarget")]),
    Method(HRESULT, "CreateHwndRenderTarget", [(Pointer(Const(D2D1_RENDER_TARGET_PROPERTIES)), "renderTargetProperties"), (Pointer(Const(D2D1_HWND_RENDER_TARGET_PROPERTIES)), "hwndRenderTargetProperties"), Out(Pointer(Pointer(ID2D1HwndRenderTarget)), "hwndRenderTarget")]),
    Method(HRESULT, "CreateDxgiSurfaceRenderTarget", [(Pointer(IDXGISurface), "dxgiSurface"), (Pointer(Const(D2D1_RENDER_TARGET_PROPERTIES)), "renderTargetProperties"), Out(Pointer(Pointer(ID2D1RenderTarget)), "renderTarget")]),
    Method(HRESULT, "CreateDCRenderTarget", [(Pointer(Const(D2D1_RENDER_TARGET_PROPERTIES)), "renderTargetProperties"), Out(Pointer(Pointer(ID2D1DCRenderTarget)), "dcRenderTarget")]),
]

d2d1 = API("d2d1")
d2d1.add_interface(ID2D1Factory)
d2d1.add_functions([
    StdFunction(HRESULT, "D2D1CreateFactory", [(D2D1_FACTORY_TYPE, "factoryType"), (REFIID, "riid"), (Pointer(Const(D2D1_FACTORY_OPTIONS)), "pFactoryOptions"), Out(Pointer(OpaquePointer(Void)), "ppIFactory")]),
    StdFunction(Void, "D2D1MakeRotateMatrix", [(FLOAT, "angle"), (D2D1_POINT_2F, "center"), Out(Pointer(D2D1_MATRIX_3X2_F), "matrix")]),
    StdFunction(Void, "D2D1MakeSkewMatrix", [(FLOAT, "angleX"), (FLOAT, "angleY"), (D2D1_POINT_2F, "center"), Out(Pointer(D2D1_MATRIX_3X2_F), "matrix")]),
    StdFunction(BOOL, "D2D1IsMatrixInvertible", [(Pointer(Const(D2D1_MATRIX_3X2_F)), "matrix")]),
    StdFunction(BOOL, "D2D1InvertMatrix", [Out(Pointer(D2D1_MATRIX_3X2_F), "matrix")]),
])
