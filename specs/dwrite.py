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
from dcommon import *


ID2D1SimplifiedGeometrySink = Interface("ID2D1SimplifiedGeometrySink", IUnknown)
IDWriteGeometrySink = Alias("IDWriteGeometrySink", ID2D1SimplifiedGeometrySink)

IDWriteFontFileLoader = Interface("IDWriteFontFileLoader", IUnknown)
IDWriteLocalFontFileLoader = Interface("IDWriteLocalFontFileLoader", IDWriteFontFileLoader)
IDWriteFontFileStream = Interface("IDWriteFontFileStream", IUnknown)
IDWriteFontFile = Interface("IDWriteFontFile", IUnknown)
IDWriteRenderingParams = Interface("IDWriteRenderingParams", IUnknown)
IDWriteFontFace = Interface("IDWriteFontFace", IUnknown)
IDWriteFontCollectionLoader = Interface("IDWriteFontCollectionLoader", IUnknown)
IDWriteFontFileEnumerator = Interface("IDWriteFontFileEnumerator", IUnknown)
IDWriteLocalizedStrings = Interface("IDWriteLocalizedStrings", IUnknown)
IDWriteFontCollection = Interface("IDWriteFontCollection", IUnknown)
IDWriteFontList = Interface("IDWriteFontList", IUnknown)
IDWriteFontFamily = Interface("IDWriteFontFamily", IDWriteFontList)
IDWriteFont = Interface("IDWriteFont", IUnknown)
IDWriteTextFormat = Interface("IDWriteTextFormat", IUnknown)
IDWriteTypography = Interface("IDWriteTypography", IUnknown)
IDWriteNumberSubstitution = Interface("IDWriteNumberSubstitution", IUnknown)
IDWriteTextAnalysisSource = Interface("IDWriteTextAnalysisSource", IUnknown)
IDWriteTextAnalysisSink = Interface("IDWriteTextAnalysisSink", IUnknown)
IDWriteTextAnalyzer = Interface("IDWriteTextAnalyzer", IUnknown)
IDWriteInlineObject = Interface("IDWriteInlineObject", IUnknown)
IDWritePixelSnapping = Interface("IDWritePixelSnapping", IUnknown)
IDWriteTextRenderer = Interface("IDWriteTextRenderer", IDWritePixelSnapping)
IDWriteTextLayout = Interface("IDWriteTextLayout", IDWriteTextFormat)
IDWriteBitmapRenderTarget = Interface("IDWriteBitmapRenderTarget", IUnknown)
IDWriteGdiInterop = Interface("IDWriteGdiInterop", IUnknown)
IDWriteGlyphRunAnalysis = Interface("IDWriteGlyphRunAnalysis", IUnknown)
IDWriteFactory = Interface("IDWriteFactory", IUnknown)


def EnumFlag(expr, values):
    return Flags(Alias(expr, DWORD), values)


DWRITE_FONT_FILE_TYPE = Enum("DWRITE_FONT_FILE_TYPE", [
    "DWRITE_FONT_FILE_TYPE_UNKNOWN",
    "DWRITE_FONT_FILE_TYPE_CFF",
    "DWRITE_FONT_FILE_TYPE_TRUETYPE",
    "DWRITE_FONT_FILE_TYPE_TRUETYPE_COLLECTION",
    "DWRITE_FONT_FILE_TYPE_TYPE1_PFM",
    "DWRITE_FONT_FILE_TYPE_TYPE1_PFB",
    "DWRITE_FONT_FILE_TYPE_VECTOR",
    "DWRITE_FONT_FILE_TYPE_BITMAP",
])

DWRITE_FONT_FACE_TYPE = Enum("DWRITE_FONT_FACE_TYPE", [
    "DWRITE_FONT_FACE_TYPE_CFF",
    "DWRITE_FONT_FACE_TYPE_TRUETYPE",
    "DWRITE_FONT_FACE_TYPE_TRUETYPE_COLLECTION",
    "DWRITE_FONT_FACE_TYPE_TYPE1",
    "DWRITE_FONT_FACE_TYPE_VECTOR",
    "DWRITE_FONT_FACE_TYPE_BITMAP",
    "DWRITE_FONT_FACE_TYPE_UNKNOWN",
])

DWRITE_FONT_SIMULATIONS = EnumFlag("DWRITE_FONT_SIMULATIONS", [
    "DWRITE_FONT_SIMULATIONS_NONE",
    "DWRITE_FONT_SIMULATIONS_BOLD",
    "DWRITE_FONT_SIMULATIONS_OBLIQUE",
])

DWRITE_FONT_WEIGHT = Enum("DWRITE_FONT_WEIGHT", [
    "DWRITE_FONT_WEIGHT_THIN",
    "DWRITE_FONT_WEIGHT_EXTRA_LIGHT",
    #"DWRITE_FONT_WEIGHT_ULTRA_LIGHT",
    "DWRITE_FONT_WEIGHT_LIGHT",
    "DWRITE_FONT_WEIGHT_NORMAL",
    #"DWRITE_FONT_WEIGHT_REGULAR",
    "DWRITE_FONT_WEIGHT_MEDIUM",
    "DWRITE_FONT_WEIGHT_DEMI_BOLD",
    #"DWRITE_FONT_WEIGHT_SEMI_BOLD",
    "DWRITE_FONT_WEIGHT_BOLD",
    "DWRITE_FONT_WEIGHT_EXTRA_BOLD",
    #"DWRITE_FONT_WEIGHT_ULTRA_BOLD",
    "DWRITE_FONT_WEIGHT_BLACK",
    #"DWRITE_FONT_WEIGHT_HEAVY",
    "DWRITE_FONT_WEIGHT_EXTRA_BLACK",
    #"DWRITE_FONT_WEIGHT_ULTRA_BLACK",
])

DWRITE_FONT_STRETCH = Enum("DWRITE_FONT_STRETCH", [
    "DWRITE_FONT_STRETCH_UNDEFINED",
    "DWRITE_FONT_STRETCH_ULTRA_CONDENSED",
    "DWRITE_FONT_STRETCH_EXTRA_CONDENSED",
    "DWRITE_FONT_STRETCH_CONDENSED",
    "DWRITE_FONT_STRETCH_SEMI_CONDENSED",
    "DWRITE_FONT_STRETCH_NORMAL",
    #"DWRITE_FONT_STRETCH_MEDIUM",
    "DWRITE_FONT_STRETCH_SEMI_EXPANDED",
    "DWRITE_FONT_STRETCH_EXPANDED",
    "DWRITE_FONT_STRETCH_EXTRA_EXPANDED",
    "DWRITE_FONT_STRETCH_ULTRA_EXPANDED",
])

DWRITE_FONT_STYLE = Enum("DWRITE_FONT_STYLE", [
    "DWRITE_FONT_STYLE_NORMAL",
    "DWRITE_FONT_STYLE_OBLIQUE",
    "DWRITE_FONT_STYLE_ITALIC",
])

DWRITE_INFORMATIONAL_STRING_ID = Enum("DWRITE_INFORMATIONAL_STRING_ID", [
    "DWRITE_INFORMATIONAL_STRING_NONE",
    "DWRITE_INFORMATIONAL_STRING_COPYRIGHT_NOTICE",
    "DWRITE_INFORMATIONAL_STRING_VERSION_STRINGS",
    "DWRITE_INFORMATIONAL_STRING_TRADEMARK",
    "DWRITE_INFORMATIONAL_STRING_MANUFACTURER",
    "DWRITE_INFORMATIONAL_STRING_DESIGNER",
    "DWRITE_INFORMATIONAL_STRING_DESIGNER_URL",
    "DWRITE_INFORMATIONAL_STRING_DESCRIPTION",
    "DWRITE_INFORMATIONAL_STRING_FONT_VENDOR_URL",
    "DWRITE_INFORMATIONAL_STRING_LICENSE_DESCRIPTION",
    "DWRITE_INFORMATIONAL_STRING_LICENSE_INFO_URL",
    "DWRITE_INFORMATIONAL_STRING_WIN32_FAMILY_NAMES",
    "DWRITE_INFORMATIONAL_STRING_WIN32_SUBFAMILY_NAMES",
    "DWRITE_INFORMATIONAL_STRING_PREFERRED_FAMILY_NAMES",
    "DWRITE_INFORMATIONAL_STRING_PREFERRED_SUBFAMILY_NAMES",
    "DWRITE_INFORMATIONAL_STRING_SAMPLE_TEXT",
])

DWRITE_FONT_METRICS = Struct("DWRITE_FONT_METRICS", [
    (UINT16, "designUnitsPerEm"),
    (UINT16, "ascent"),
    (UINT16, "descent"),
    (INT16, "lineGap"),
    (UINT16, "capHeight"),
    (UINT16, "xHeight"),
    (INT16, "underlinePosition"),
    (UINT16, "underlineThickness"),
    (INT16, "strikethroughPosition"),
    (UINT16, "strikethroughThickness"),
])

DWRITE_GLYPH_METRICS = Struct("DWRITE_GLYPH_METRICS", [
    (INT32, "leftSideBearing"),
    (UINT32, "advanceWidth"),
    (INT32, "rightSideBearing"),
    (INT32, "topSideBearing"),
    (UINT32, "advanceHeight"),
    (INT32, "bottomSideBearing"),
    (INT32, "verticalOriginY"),
])

DWRITE_GLYPH_OFFSET = Struct("DWRITE_GLYPH_OFFSET", [
    (FLOAT, "advanceOffset"),
    (FLOAT, "ascenderOffset"),
])

DWRITE_FACTORY_TYPE = Enum("DWRITE_FACTORY_TYPE", [
    "DWRITE_FACTORY_TYPE_SHARED",
    "DWRITE_FACTORY_TYPE_ISOLATED",
])

IDWriteFontFileLoader.methods += [
    StdMethod(HRESULT, "CreateStreamFromKey", [(OpaquePointer(Const(Void)), "fontFileReferenceKey"), (UINT32, "fontFileReferenceKeySize"), Out(Pointer(ObjPointer(IDWriteFontFileStream)), "fontFileStream")]),
]

IDWriteLocalFontFileLoader.methods += [
    StdMethod(HRESULT, "GetFilePathLengthFromKey", [(OpaquePointer(Const(Void)), "fontFileReferenceKey"), (UINT32, "fontFileReferenceKeySize"), Out(Pointer(UINT32), "filePathLength")]),
    StdMethod(HRESULT, "GetFilePathFromKey", [(OpaquePointer(Const(Void)), "fontFileReferenceKey"), (UINT32, "fontFileReferenceKeySize"), Out(Pointer(WCHAR), "filePath"), (UINT32, "filePathSize")]),
    StdMethod(HRESULT, "GetLastWriteTimeFromKey", [(OpaquePointer(Const(Void)), "fontFileReferenceKey"), (UINT32, "fontFileReferenceKeySize"), Out(Pointer(FILETIME), "lastWriteTime")]),
]

IDWriteFontFileStream.methods += [
    StdMethod(HRESULT, "ReadFileFragment", [Out(Pointer(OpaquePointer(Const(Void))), "fragmentStart"), (UINT64, "fileOffset"), (UINT64, "fragmentSize"), Out(Pointer(OpaquePointer(Void)), "fragmentContext")]),
    StdMethod(Void, "ReleaseFileFragment", [(OpaquePointer(Void), "fragmentContext")]),
    StdMethod(HRESULT, "GetFileSize", [Out(Pointer(UINT64), "fileSize")]),
    StdMethod(HRESULT, "GetLastWriteTime", [Out(Pointer(UINT64), "lastWriteTime")]),
]

IDWriteFontFile.methods += [
    StdMethod(HRESULT, "GetReferenceKey", [Out(Pointer(OpaquePointer(Const(Void))), "fontFileReferenceKey"), Out(Pointer(UINT32), "fontFileReferenceKeySize")]),
    StdMethod(HRESULT, "GetLoader", [Out(Pointer(ObjPointer(IDWriteFontFileLoader)), "fontFileLoader")]),
    StdMethod(HRESULT, "Analyze", [Out(Pointer(BOOL), "isSupportedFontType"), Out(Pointer(DWRITE_FONT_FILE_TYPE), "fontFileType"), Out(Pointer(DWRITE_FONT_FACE_TYPE), "fontFaceType"), Out(Pointer(UINT32), "numberOfFaces")]),
]

DWRITE_PIXEL_GEOMETRY = Enum("DWRITE_PIXEL_GEOMETRY", [
    "DWRITE_PIXEL_GEOMETRY_FLAT",
    "DWRITE_PIXEL_GEOMETRY_RGB",
    "DWRITE_PIXEL_GEOMETRY_BGR",
])

DWRITE_RENDERING_MODE = Enum("DWRITE_RENDERING_MODE", [
    "DWRITE_RENDERING_MODE_DEFAULT",
    "DWRITE_RENDERING_MODE_ALIASED",
    "DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC",
    "DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL",
    "DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL",
    "DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC",
    "DWRITE_RENDERING_MODE_OUTLINE",
])

DWRITE_MATRIX = Struct("DWRITE_MATRIX", [
    (FLOAT, "m11"),
    (FLOAT, "m12"),
    (FLOAT, "m21"),
    (FLOAT, "m22"),
    (FLOAT, "dx"),
    (FLOAT, "dy"),
])

IDWriteRenderingParams.methods += [
    StdMethod(FLOAT, "GetGamma", []),
    StdMethod(FLOAT, "GetEnhancedContrast", []),
    StdMethod(FLOAT, "GetClearTypeLevel", []),
    StdMethod(DWRITE_PIXEL_GEOMETRY, "GetPixelGeometry", []),
    StdMethod(DWRITE_RENDERING_MODE, "GetRenderingMode", []),
]

IDWriteFontFace.methods += [
    StdMethod(DWRITE_FONT_FACE_TYPE, "GetType", []),
    StdMethod(HRESULT, "GetFiles", [(OpaquePointer(UINT32), "numberOfFiles"), Out(Pointer(ObjPointer(IDWriteFontFile)), "fontFiles")]),
    StdMethod(UINT32, "GetIndex", []),
    StdMethod(DWRITE_FONT_SIMULATIONS, "GetSimulations", []),
    StdMethod(BOOL, "IsSymbolFont", []),
    StdMethod(Void, "GetMetrics", [Out(Pointer(DWRITE_FONT_METRICS), "fontFaceMetrics")]),
    StdMethod(UINT16, "GetGlyphCount", []),
    StdMethod(HRESULT, "GetDesignGlyphMetrics", [(Pointer(Const(UINT16)), "glyphIndices"), (UINT32, "glyphCount"), Out(Pointer(DWRITE_GLYPH_METRICS), "glyphMetrics"), (BOOL, "isSideways")]),
    StdMethod(HRESULT, "GetGlyphIndices", [(Pointer(Const(UINT32)), "codePoints"), (UINT32, "codePointCount"), Out(Pointer(UINT16), "glyphIndices")]),
    StdMethod(HRESULT, "TryGetFontTable", [(UINT32, "openTypeTableTag"), Out(Pointer(OpaquePointer(Const(Void))), "tableData"), Out(Pointer(UINT32), "tableSize"), Out(Pointer(OpaquePointer(Void)), "tableContext"), Out(Pointer(BOOL), "exists")]),
    StdMethod(Void, "ReleaseFontTable", [(OpaquePointer(Void), "tableContext")]),
    StdMethod(HRESULT, "GetGlyphRunOutline", [(FLOAT, "emSize"), (Pointer(Const(UINT16)), "glyphIndices"), (Pointer(Const(FLOAT)), "glyphAdvances"), (Pointer(Const(DWRITE_GLYPH_OFFSET)), "glyphOffsets"), (UINT32, "glyphCount"), (BOOL, "isSideways"), (BOOL, "isRightToLeft"), (ObjPointer(IDWriteGeometrySink), "geometrySink")]),
    StdMethod(HRESULT, "GetRecommendedRenderingMode", [(FLOAT, "emSize"), (FLOAT, "pixelsPerDip"), (DWRITE_MEASURING_MODE, "measuringMode"), (ObjPointer(IDWriteRenderingParams), "renderingParams"), Out(Pointer(DWRITE_RENDERING_MODE), "renderingMode")]),
    StdMethod(HRESULT, "GetGdiCompatibleMetrics", [(FLOAT, "emSize"), (FLOAT, "pixelsPerDip"), (Pointer(Const(DWRITE_MATRIX)), "transform"), Out(Pointer(DWRITE_FONT_METRICS), "fontFaceMetrics")]),
    StdMethod(HRESULT, "GetGdiCompatibleGlyphMetrics", [(FLOAT, "emSize"), (FLOAT, "pixelsPerDip"), (Pointer(Const(DWRITE_MATRIX)), "transform"), (BOOL, "useGdiNatural"), (Array(Const(UINT16), "glyphCount"), "glyphIndices"), (UINT32, "glyphCount"), Out(Array(DWRITE_GLYPH_METRICS, "glyphCount"), "glyphMetrics"), (BOOL, "isSideways")]),
]

IDWriteFontCollectionLoader.methods += [
    StdMethod(HRESULT, "CreateEnumeratorFromKey", [(ObjPointer(IDWriteFactory), "factory"), (OpaquePointer(Const(Void)), "collectionKey"), (UINT32, "collectionKeySize"), Out(Pointer(ObjPointer(IDWriteFontFileEnumerator)), "fontFileEnumerator")]),
]

IDWriteFontFileEnumerator.methods += [
    StdMethod(HRESULT, "MoveNext", [Out(Pointer(BOOL), "hasCurrentFile")]),
    StdMethod(HRESULT, "GetCurrentFontFile", [Out(Pointer(ObjPointer(IDWriteFontFile)), "fontFile")]),
]

IDWriteLocalizedStrings.methods += [
    StdMethod(UINT32, "GetCount", []),
    StdMethod(HRESULT, "FindLocaleName", [(Pointer(Const(WCHAR)), "localeName"), Out(Pointer(UINT32), "index"), Out(Pointer(BOOL), "exists")]),
    StdMethod(HRESULT, "GetLocaleNameLength", [(UINT32, "index"), Out(Pointer(UINT32), "length")]),
    StdMethod(HRESULT, "GetLocaleName", [(UINT32, "index"), Out(Pointer(WCHAR), "localeName"), (UINT32, "size")]),
    StdMethod(HRESULT, "GetStringLength", [(UINT32, "index"), Out(Pointer(UINT32), "length")]),
    StdMethod(HRESULT, "GetString", [(UINT32, "index"), Out(Pointer(WCHAR), "stringBuffer"), (UINT32, "size")]),
]

IDWriteFontCollection.methods += [
    StdMethod(UINT32, "GetFontFamilyCount", []),
    StdMethod(HRESULT, "GetFontFamily", [(UINT32, "index"), Out(Pointer(ObjPointer(IDWriteFontFamily)), "fontFamily")]),
    StdMethod(HRESULT, "FindFamilyName", [(Pointer(Const(WCHAR)), "familyName"), Out(Pointer(UINT32), "index"), Out(Pointer(BOOL), "exists")]),
    StdMethod(HRESULT, "GetFontFromFontFace", [(ObjPointer(IDWriteFontFace), "fontFace"), Out(Pointer(ObjPointer(IDWriteFont)), "font")]),
]

IDWriteFontList.methods += [
    StdMethod(HRESULT, "GetFontCollection", [Out(Pointer(ObjPointer(IDWriteFontCollection)), "fontCollection")]),
    StdMethod(UINT32, "GetFontCount", []),
    StdMethod(HRESULT, "GetFont", [(UINT32, "index"), Out(Pointer(ObjPointer(IDWriteFont)), "font")]),
]

IDWriteFontFamily.methods += [
    StdMethod(HRESULT, "GetFamilyNames", [Out(Pointer(ObjPointer(IDWriteLocalizedStrings)), "names")]),
    StdMethod(HRESULT, "GetFirstMatchingFont", [(DWRITE_FONT_WEIGHT, "weight"), (DWRITE_FONT_STRETCH, "stretch"), (DWRITE_FONT_STYLE, "style"), Out(Pointer(ObjPointer(IDWriteFont)), "matchingFont")]),
    StdMethod(HRESULT, "GetMatchingFonts", [(DWRITE_FONT_WEIGHT, "weight"), (DWRITE_FONT_STRETCH, "stretch"), (DWRITE_FONT_STYLE, "style"), Out(Pointer(ObjPointer(IDWriteFontList)), "matchingFonts")]),
]

IDWriteFont.methods += [
    StdMethod(HRESULT, "GetFontFamily", [Out(Pointer(ObjPointer(IDWriteFontFamily)), "fontFamily")]),
    StdMethod(DWRITE_FONT_WEIGHT, "GetWeight", []),
    StdMethod(DWRITE_FONT_STRETCH, "GetStretch", []),
    StdMethod(DWRITE_FONT_STYLE, "GetStyle", []),
    StdMethod(BOOL, "IsSymbolFont", []),
    StdMethod(HRESULT, "GetFaceNames", [Out(Pointer(ObjPointer(IDWriteLocalizedStrings)), "names")]),
    StdMethod(HRESULT, "GetInformationalStrings", [(DWRITE_INFORMATIONAL_STRING_ID, "informationalStringID"), Out(Pointer(ObjPointer(IDWriteLocalizedStrings)), "informationalStrings"), Out(Pointer(BOOL), "exists")]),
    StdMethod(DWRITE_FONT_SIMULATIONS, "GetSimulations", []),
    StdMethod(Void, "GetMetrics", [Out(Pointer(DWRITE_FONT_METRICS), "fontMetrics")]),
    StdMethod(HRESULT, "HasCharacter", [(UINT32, "unicodeValue"), Out(Pointer(BOOL), "exists")]),
    StdMethod(HRESULT, "CreateFontFace", [Out(Pointer(ObjPointer(IDWriteFontFace)), "fontFace")]),
]

DWRITE_READING_DIRECTION = Enum("DWRITE_READING_DIRECTION", [
    "DWRITE_READING_DIRECTION_LEFT_TO_RIGHT",
    "DWRITE_READING_DIRECTION_RIGHT_TO_LEFT",
])

DWRITE_FLOW_DIRECTION = Enum("DWRITE_FLOW_DIRECTION", [
    "DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM",
])

DWRITE_TEXT_ALIGNMENT = Enum("DWRITE_TEXT_ALIGNMENT", [
    "DWRITE_TEXT_ALIGNMENT_LEADING",
    "DWRITE_TEXT_ALIGNMENT_TRAILING",
    "DWRITE_TEXT_ALIGNMENT_CENTER",
])

DWRITE_PARAGRAPH_ALIGNMENT = Enum("DWRITE_PARAGRAPH_ALIGNMENT", [
    "DWRITE_PARAGRAPH_ALIGNMENT_NEAR",
    "DWRITE_PARAGRAPH_ALIGNMENT_FAR",
    "DWRITE_PARAGRAPH_ALIGNMENT_CENTER",
])

DWRITE_WORD_WRAPPING = Enum("DWRITE_WORD_WRAPPING", [
    "DWRITE_WORD_WRAPPING_WRAP",
    "DWRITE_WORD_WRAPPING_NO_WRAP",
])

DWRITE_LINE_SPACING_METHOD = Enum("DWRITE_LINE_SPACING_METHOD", [
    "DWRITE_LINE_SPACING_METHOD_DEFAULT",
    "DWRITE_LINE_SPACING_METHOD_UNIFORM",
])

DWRITE_TRIMMING_GRANULARITY = Enum("DWRITE_TRIMMING_GRANULARITY", [
    "DWRITE_TRIMMING_GRANULARITY_NONE",
    "DWRITE_TRIMMING_GRANULARITY_CHARACTER",
    "DWRITE_TRIMMING_GRANULARITY_WORD",
])

DWRITE_FONT_FEATURE_TAG = Enum("DWRITE_FONT_FEATURE_TAG", [
    "DWRITE_FONT_FEATURE_TAG_ALTERNATIVE_FRACTIONS",
    "DWRITE_FONT_FEATURE_TAG_PETITE_CAPITALS_FROM_CAPITALS",
    "DWRITE_FONT_FEATURE_TAG_SMALL_CAPITALS_FROM_CAPITALS",
    "DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_ALTERNATES",
    "DWRITE_FONT_FEATURE_TAG_CASE_SENSITIVE_FORMS",
    "DWRITE_FONT_FEATURE_TAG_GLYPH_COMPOSITION_DECOMPOSITION",
    "DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_LIGATURES",
    "DWRITE_FONT_FEATURE_TAG_CAPITAL_SPACING",
    "DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_SWASH",
    "DWRITE_FONT_FEATURE_TAG_CURSIVE_POSITIONING",
    "DWRITE_FONT_FEATURE_TAG_DEFAULT",
    "DWRITE_FONT_FEATURE_TAG_DISCRETIONARY_LIGATURES",
    "DWRITE_FONT_FEATURE_TAG_EXPERT_FORMS",
    "DWRITE_FONT_FEATURE_TAG_FRACTIONS",
    "DWRITE_FONT_FEATURE_TAG_FULL_WIDTH",
    "DWRITE_FONT_FEATURE_TAG_HALF_FORMS",
    "DWRITE_FONT_FEATURE_TAG_HALANT_FORMS",
    "DWRITE_FONT_FEATURE_TAG_ALTERNATE_HALF_WIDTH",
    "DWRITE_FONT_FEATURE_TAG_HISTORICAL_FORMS",
    "DWRITE_FONT_FEATURE_TAG_HORIZONTAL_KANA_ALTERNATES",
    "DWRITE_FONT_FEATURE_TAG_HISTORICAL_LIGATURES",
    "DWRITE_FONT_FEATURE_TAG_HALF_WIDTH",
    "DWRITE_FONT_FEATURE_TAG_HOJO_KANJI_FORMS",
    "DWRITE_FONT_FEATURE_TAG_JIS04_FORMS",
    "DWRITE_FONT_FEATURE_TAG_JIS78_FORMS",
    "DWRITE_FONT_FEATURE_TAG_JIS83_FORMS",
    "DWRITE_FONT_FEATURE_TAG_JIS90_FORMS",
    "DWRITE_FONT_FEATURE_TAG_KERNING",
    "DWRITE_FONT_FEATURE_TAG_STANDARD_LIGATURES",
    "DWRITE_FONT_FEATURE_TAG_LINING_FIGURES",
    "DWRITE_FONT_FEATURE_TAG_LOCALIZED_FORMS",
    "DWRITE_FONT_FEATURE_TAG_MARK_POSITIONING",
    "DWRITE_FONT_FEATURE_TAG_MATHEMATICAL_GREEK",
    "DWRITE_FONT_FEATURE_TAG_MARK_TO_MARK_POSITIONING",
    "DWRITE_FONT_FEATURE_TAG_ALTERNATE_ANNOTATION_FORMS",
    "DWRITE_FONT_FEATURE_TAG_NLC_KANJI_FORMS",
    "DWRITE_FONT_FEATURE_TAG_OLD_STYLE_FIGURES",
    "DWRITE_FONT_FEATURE_TAG_ORDINALS",
    "DWRITE_FONT_FEATURE_TAG_PROPORTIONAL_ALTERNATE_WIDTH",
    "DWRITE_FONT_FEATURE_TAG_PETITE_CAPITALS",
    "DWRITE_FONT_FEATURE_TAG_PROPORTIONAL_FIGURES",
    "DWRITE_FONT_FEATURE_TAG_PROPORTIONAL_WIDTHS",
    "DWRITE_FONT_FEATURE_TAG_QUARTER_WIDTHS",
    "DWRITE_FONT_FEATURE_TAG_REQUIRED_LIGATURES",
    "DWRITE_FONT_FEATURE_TAG_RUBY_NOTATION_FORMS",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_ALTERNATES",
    "DWRITE_FONT_FEATURE_TAG_SCIENTIFIC_INFERIORS",
    "DWRITE_FONT_FEATURE_TAG_SMALL_CAPITALS",
    "DWRITE_FONT_FEATURE_TAG_SIMPLIFIED_FORMS",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_1",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_2",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_3",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_4",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_5",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_6",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_7",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_8",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_9",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_10",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_11",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_12",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_13",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_14",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_15",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_16",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_17",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_18",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_19",
    "DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_20",
    "DWRITE_FONT_FEATURE_TAG_SUBSCRIPT",
    "DWRITE_FONT_FEATURE_TAG_SUPERSCRIPT",
    "DWRITE_FONT_FEATURE_TAG_SWASH",
    "DWRITE_FONT_FEATURE_TAG_TITLING",
    "DWRITE_FONT_FEATURE_TAG_TRADITIONAL_NAME_FORMS",
    "DWRITE_FONT_FEATURE_TAG_TABULAR_FIGURES",
    "DWRITE_FONT_FEATURE_TAG_TRADITIONAL_FORMS",
    "DWRITE_FONT_FEATURE_TAG_THIRD_WIDTHS",
    "DWRITE_FONT_FEATURE_TAG_UNICASE",
    "DWRITE_FONT_FEATURE_TAG_SLASHED_ZERO",
])

DWRITE_TEXT_RANGE = Struct("DWRITE_TEXT_RANGE", [
    (UINT32, "startPosition"),
    (UINT32, "length"),
])

DWRITE_FONT_FEATURE = Struct("DWRITE_FONT_FEATURE", [
    (DWRITE_FONT_FEATURE_TAG, "nameTag"),
    (UINT32, "parameter"),
])

DWRITE_TYPOGRAPHIC_FEATURES = Struct("DWRITE_TYPOGRAPHIC_FEATURES", [
    (OpaquePointer(DWRITE_FONT_FEATURE), "features"),
    (UINT32, "featureCount"),
])

DWRITE_TRIMMING = Struct("DWRITE_TRIMMING", [
    (DWRITE_TRIMMING_GRANULARITY, "granularity"),
    (UINT32, "delimiter"),
    (UINT32, "delimiterCount"),
])

IDWriteTextFormat.methods += [
    StdMethod(HRESULT, "SetTextAlignment", [(DWRITE_TEXT_ALIGNMENT, "textAlignment")]),
    StdMethod(HRESULT, "SetParagraphAlignment", [(DWRITE_PARAGRAPH_ALIGNMENT, "paragraphAlignment")]),
    StdMethod(HRESULT, "SetWordWrapping", [(DWRITE_WORD_WRAPPING, "wordWrapping")]),
    StdMethod(HRESULT, "SetReadingDirection", [(DWRITE_READING_DIRECTION, "readingDirection")]),
    StdMethod(HRESULT, "SetFlowDirection", [(DWRITE_FLOW_DIRECTION, "flowDirection")]),
    StdMethod(HRESULT, "SetIncrementalTabStop", [(FLOAT, "incrementalTabStop")]),
    StdMethod(HRESULT, "SetTrimming", [(Pointer(Const(DWRITE_TRIMMING)), "trimmingOptions"), (OpaquePointer(IDWriteInlineObject), "trimmingSign")]),
    StdMethod(HRESULT, "SetLineSpacing", [(DWRITE_LINE_SPACING_METHOD, "lineSpacingMethod"), (FLOAT, "lineSpacing"), (FLOAT, "baseline")]),
    StdMethod(DWRITE_TEXT_ALIGNMENT, "GetTextAlignment", []),
    StdMethod(DWRITE_PARAGRAPH_ALIGNMENT, "GetParagraphAlignment", []),
    StdMethod(DWRITE_WORD_WRAPPING, "GetWordWrapping", []),
    StdMethod(DWRITE_READING_DIRECTION, "GetReadingDirection", []),
    StdMethod(DWRITE_FLOW_DIRECTION, "GetFlowDirection", []),
    StdMethod(FLOAT, "GetIncrementalTabStop", []),
    StdMethod(HRESULT, "GetTrimming", [Out(Pointer(DWRITE_TRIMMING), "trimmingOptions"), Out(Pointer(OpaquePointer(IDWriteInlineObject)), "trimmingSign")]),
    StdMethod(HRESULT, "GetLineSpacing", [Out(Pointer(DWRITE_LINE_SPACING_METHOD), "lineSpacingMethod"), Out(Pointer(FLOAT), "lineSpacing"), Out(Pointer(FLOAT), "baseline")]),
    StdMethod(HRESULT, "GetFontCollection", [Out(Pointer(ObjPointer(IDWriteFontCollection)), "fontCollection")]),
    StdMethod(UINT32, "GetFontFamilyNameLength", []),
    StdMethod(HRESULT, "GetFontFamilyName", [Out(Pointer(WCHAR), "fontFamilyName"), (UINT32, "nameSize")]),
    StdMethod(DWRITE_FONT_WEIGHT, "GetFontWeight", []),
    StdMethod(DWRITE_FONT_STYLE, "GetFontStyle", []),
    StdMethod(DWRITE_FONT_STRETCH, "GetFontStretch", []),
    StdMethod(FLOAT, "GetFontSize", []),
    StdMethod(UINT32, "GetLocaleNameLength", []),
    StdMethod(HRESULT, "GetLocaleName", [Out(Pointer(WCHAR), "localeName"), (UINT32, "nameSize")]),
]

IDWriteTypography.methods += [
    StdMethod(HRESULT, "AddFontFeature", [(DWRITE_FONT_FEATURE, "fontFeature")]),
    StdMethod(UINT32, "GetFontFeatureCount", []),
    StdMethod(HRESULT, "GetFontFeature", [(UINT32, "fontFeatureIndex"), Out(Pointer(DWRITE_FONT_FEATURE), "fontFeature")]),
]

DWRITE_SCRIPT_SHAPES = EnumFlag("DWRITE_SCRIPT_SHAPES", [
    "DWRITE_SCRIPT_SHAPES_DEFAULT",
    "DWRITE_SCRIPT_SHAPES_NO_VISUAL",
])

DWRITE_SCRIPT_ANALYSIS = Struct("DWRITE_SCRIPT_ANALYSIS", [
    (UINT16, "script"),
    (DWRITE_SCRIPT_SHAPES, "shapes"),
])

DWRITE_BREAK_CONDITION = Enum("DWRITE_BREAK_CONDITION", [
    "DWRITE_BREAK_CONDITION_NEUTRAL",
    "DWRITE_BREAK_CONDITION_CAN_BREAK",
    "DWRITE_BREAK_CONDITION_MAY_NOT_BREAK",
    "DWRITE_BREAK_CONDITION_MUST_BREAK",
])

DWRITE_LINE_BREAKPOINT = Struct("DWRITE_LINE_BREAKPOINT", [
    (UINT8, "breakConditionBefore"),
    (UINT8, "breakConditionAfter"),
    (UINT8, "isWhitespace"),
    (UINT8, "isSoftHyphen"),
    (UINT8, "padding"),
])

DWRITE_NUMBER_SUBSTITUTION_METHOD = Enum("DWRITE_NUMBER_SUBSTITUTION_METHOD", [
    "DWRITE_NUMBER_SUBSTITUTION_METHOD_FROM_CULTURE",
    "DWRITE_NUMBER_SUBSTITUTION_METHOD_CONTEXTUAL",
    "DWRITE_NUMBER_SUBSTITUTION_METHOD_NONE",
    "DWRITE_NUMBER_SUBSTITUTION_METHOD_NATIONAL",
    "DWRITE_NUMBER_SUBSTITUTION_METHOD_TRADITIONAL",
])

IDWriteNumberSubstitution.methods += [
]

DWRITE_SHAPING_TEXT_PROPERTIES = Struct("DWRITE_SHAPING_TEXT_PROPERTIES", [
    (UINT16, "isShapedAlone"),
    (UINT16, "reserved"),
])

DWRITE_SHAPING_GLYPH_PROPERTIES = Struct("DWRITE_SHAPING_GLYPH_PROPERTIES", [
    (UINT16, "justification"),
    (UINT16, "isClusterStart"),
    (UINT16, "isDiacritic"),
    (UINT16, "isZeroWidthSpace"),
    (UINT16, "reserved"),
])

IDWriteTextAnalysisSource.methods += [
    StdMethod(HRESULT, "GetTextAtPosition", [(UINT32, "textPosition"), Out(Pointer(Pointer(Const(WCHAR))), "textString"), Out(Pointer(UINT32), "textLength")]),
    StdMethod(HRESULT, "GetTextBeforePosition", [(UINT32, "textPosition"), Out(Pointer(Pointer(Const(WCHAR))), "textString"), Out(Pointer(UINT32), "textLength")]),
    StdMethod(DWRITE_READING_DIRECTION, "GetParagraphReadingDirection", []),
    StdMethod(HRESULT, "GetLocaleName", [(UINT32, "textPosition"), Out(Pointer(UINT32), "textLength"), Out(Pointer(Pointer(Const(WCHAR))), "localeName")]),
    StdMethod(HRESULT, "GetNumberSubstitution", [(UINT32, "textPosition"), Out(Pointer(UINT32), "textLength"), Out(Pointer(ObjPointer(IDWriteNumberSubstitution)), "numberSubstitution")]),
]

IDWriteTextAnalysisSink.methods += [
    StdMethod(HRESULT, "SetScriptAnalysis", [(UINT32, "textPosition"), (UINT32, "textLength"), (Pointer(Const(DWRITE_SCRIPT_ANALYSIS)), "scriptAnalysis")]),
    StdMethod(HRESULT, "SetLineBreakpoints", [(UINT32, "textPosition"), (UINT32, "textLength"), (Pointer(Const(DWRITE_LINE_BREAKPOINT)), "lineBreakpoints")]),
    StdMethod(HRESULT, "SetBidiLevel", [(UINT32, "textPosition"), (UINT32, "textLength"), (UINT8, "explicitLevel"), (UINT8, "resolvedLevel")]),
    StdMethod(HRESULT, "SetNumberSubstitution", [(UINT32, "textPosition"), (UINT32, "textLength"), (ObjPointer(IDWriteNumberSubstitution), "numberSubstitution")]),
]

IDWriteTextAnalyzer.methods += [
    StdMethod(HRESULT, "AnalyzeScript", [(ObjPointer(IDWriteTextAnalysisSource), "analysisSource"), (UINT32, "textPosition"), (UINT32, "textLength"), (ObjPointer(IDWriteTextAnalysisSink), "analysisSink")]),
    StdMethod(HRESULT, "AnalyzeBidi", [(ObjPointer(IDWriteTextAnalysisSource), "analysisSource"), (UINT32, "textPosition"), (UINT32, "textLength"), (ObjPointer(IDWriteTextAnalysisSink), "analysisSink")]),
    StdMethod(HRESULT, "AnalyzeNumberSubstitution", [(ObjPointer(IDWriteTextAnalysisSource), "analysisSource"), (UINT32, "textPosition"), (UINT32, "textLength"), (ObjPointer(IDWriteTextAnalysisSink), "analysisSink")]),
    StdMethod(HRESULT, "AnalyzeLineBreakpoints", [(ObjPointer(IDWriteTextAnalysisSource), "analysisSource"), (UINT32, "textPosition"), (UINT32, "textLength"), (ObjPointer(IDWriteTextAnalysisSink), "analysisSink")]),
    StdMethod(HRESULT, "GetGlyphs", [(Pointer(Const(WCHAR)), "textString"), (UINT32, "textLength"), (ObjPointer(IDWriteFontFace), "fontFace"), (BOOL, "isSideways"), (BOOL, "isRightToLeft"), (Pointer(Const(DWRITE_SCRIPT_ANALYSIS)), "scriptAnalysis"), (Pointer(Const(WCHAR)), "localeName"), (ObjPointer(IDWriteNumberSubstitution), "numberSubstitution"), (OpaquePointer(Pointer(Const(DWRITE_TYPOGRAPHIC_FEATURES))), "features"), (Pointer(Const(UINT32)), "featureRangeLengths"), (UINT32, "featureRanges"), (UINT32, "maxGlyphCount"), Out(Pointer(UINT16), "clusterMap"), Out(Pointer(DWRITE_SHAPING_TEXT_PROPERTIES), "textProps"), Out(Pointer(UINT16), "glyphIndices"), Out(Pointer(DWRITE_SHAPING_GLYPH_PROPERTIES), "glyphProps"), Out(Pointer(UINT32), "actualGlyphCount")]),
    StdMethod(HRESULT, "GetGlyphPlacements", [(Array(Const(WCHAR), "textLength"), "textString"), (Array(Const(UINT16), "textLength"), "clusterMap"), (Array(DWRITE_SHAPING_TEXT_PROPERTIES, "textLength"), "textProps"), (UINT32, "textLength"), (Array(Const(UINT16), "glyphCount"), "glyphIndices"), (Array(Const(DWRITE_SHAPING_GLYPH_PROPERTIES), "glyphCount"), "glyphProps"), (UINT32, "glyphCount"), (ObjPointer(IDWriteFontFace), "fontFace"), (FLOAT, "fontEmSize"), (BOOL, "isSideways"), (BOOL, "isRightToLeft"), (Pointer(Const(DWRITE_SCRIPT_ANALYSIS)), "scriptAnalysis"), (LPCWSTR, "localeName"), (Array(Pointer(Const(DWRITE_TYPOGRAPHIC_FEATURES)), "featureRanges"), "features"), (Array(Const(UINT32), "featureRanges"), "featureRangeLengths"), (UINT32, "featureRanges"), Out(Array(FLOAT, "glyphCount"), "glyphAdvances"), Out(Array(DWRITE_GLYPH_OFFSET, "glyphCount"), "glyphOffsets")]), 
    StdMethod(HRESULT, "GetGdiCompatibleGlyphPlacements", [(Array(Const(WCHAR), "textLength"), "textString"), (Array(Const(UINT16), "textLength"), "clusterMap"), (Array(DWRITE_SHAPING_TEXT_PROPERTIES, "textLength"), "textProps"), (UINT32, "textLength"), (Array(Const(UINT16), "glyphCount"), "glyphIndices"), (Array(Const(DWRITE_SHAPING_GLYPH_PROPERTIES), "glyphCount"), "glyphProps"), (UINT32, "glyphCount"), (ObjPointer(IDWriteFontFace), "fontFace"), (FLOAT, "fontEmSize"), (FLOAT, "pixelsPerDip"), (Pointer(Const(DWRITE_MATRIX)), "transform"), (BOOL, "useGdiNatural"), (BOOL, "isSideways"), (BOOL, "isRightToLeft"), (Pointer(Const(DWRITE_SCRIPT_ANALYSIS)), "scriptAnalysis"), (LPCWSTR, "localeName"), (Array(Pointer(Const(DWRITE_TYPOGRAPHIC_FEATURES)), "featureRanges"), "features"), (Array(Const(UINT32), "featureRanges"), "featureRangeLengths"), (UINT32, "featureRanges"), Out(Array(FLOAT, "glyphCount"), "glyphAdvances"), Out(Array(DWRITE_GLYPH_OFFSET, "glyphCount"), "glyphOffsets")]),
]

DWRITE_GLYPH_RUN = Struct("DWRITE_GLYPH_RUN", [
    (ObjPointer(IDWriteFontFace), "fontFace"),
    (FLOAT, "fontEmSize"),
    (UINT32, "glyphCount"),
    (Pointer(Const(UINT16)), "glyphIndices"),
    (Pointer(Const(FLOAT)), "glyphAdvances"),
    (Pointer(Const(DWRITE_GLYPH_OFFSET)), "glyphOffsets"),
    (BOOL, "isSideways"),
    (UINT32, "bidiLevel"),
])

DWRITE_GLYPH_RUN_DESCRIPTION = Struct("DWRITE_GLYPH_RUN_DESCRIPTION", [
    (Pointer(Const(WCHAR)), "localeName"),
    (Pointer(Const(WCHAR)), "string"),
    (UINT32, "stringLength"),
    (Pointer(Const(UINT16)), "clusterMap"),
    (UINT32, "textPosition"),
])

DWRITE_UNDERLINE = Struct("DWRITE_UNDERLINE", [
    (FLOAT, "width"),
    (FLOAT, "thickness"),
    (FLOAT, "offset"),
    (FLOAT, "runHeight"),
    (DWRITE_READING_DIRECTION, "readingDirection"),
    (DWRITE_FLOW_DIRECTION, "flowDirection"),
    (Pointer(Const(WCHAR)), "localeName"),
    (DWRITE_MEASURING_MODE, "measuringMode"),
])

DWRITE_STRIKETHROUGH = Struct("DWRITE_STRIKETHROUGH", [
    (FLOAT, "width"),
    (FLOAT, "thickness"),
    (FLOAT, "offset"),
    (DWRITE_READING_DIRECTION, "readingDirection"),
    (DWRITE_FLOW_DIRECTION, "flowDirection"),
    (Pointer(Const(WCHAR)), "localeName"),
    (DWRITE_MEASURING_MODE, "measuringMode"),
])

DWRITE_LINE_METRICS = Struct("DWRITE_LINE_METRICS", [
    (UINT32, "length"),
    (UINT32, "trailingWhitespaceLength"),
    (UINT32, "newlineLength"),
    (FLOAT, "height"),
    (FLOAT, "baseline"),
    (BOOL, "isTrimmed"),
])

DWRITE_CLUSTER_METRICS = Struct("DWRITE_CLUSTER_METRICS", [
    (FLOAT, "width"),
    (UINT16, "length"),
    (UINT16, "canWrapLineAfter"),
    (UINT16, "isWhitespace"),
    (UINT16, "isNewline"),
    (UINT16, "isSoftHyphen"),
    (UINT16, "isRightToLeft"),
    (UINT16, "padding"),
])

DWRITE_TEXT_METRICS = Struct("DWRITE_TEXT_METRICS", [
    (FLOAT, "left"),
    (FLOAT, "top"),
    (FLOAT, "width"),
    (FLOAT, "widthIncludingTrailingWhitespace"),
    (FLOAT, "height"),
    (FLOAT, "layoutWidth"),
    (FLOAT, "layoutHeight"),
    (UINT32, "maxBidiReorderingDepth"),
    (UINT32, "lineCount"),
])

DWRITE_INLINE_OBJECT_METRICS = Struct("DWRITE_INLINE_OBJECT_METRICS", [
    (FLOAT, "width"),
    (FLOAT, "height"),
    (FLOAT, "baseline"),
    (BOOL, "supportsSideways"),
])

DWRITE_OVERHANG_METRICS = Struct("DWRITE_OVERHANG_METRICS", [
    (FLOAT, "left"),
    (FLOAT, "top"),
    (FLOAT, "right"),
    (FLOAT, "bottom"),
])

DWRITE_HIT_TEST_METRICS = Struct("DWRITE_HIT_TEST_METRICS", [
    (UINT32, "textPosition"),
    (UINT32, "length"),
    (FLOAT, "left"),
    (FLOAT, "top"),
    (FLOAT, "width"),
    (FLOAT, "height"),
    (UINT32, "bidiLevel"),
    (BOOL, "isText"),
    (BOOL, "isTrimmed"),
])

IDWriteInlineObject.methods += [
    StdMethod(HRESULT, "Draw", [(OpaquePointer(Void), "clientDrawingContext"), (OpaquePointer(IDWriteTextRenderer), "renderer"), (FLOAT, "originX"), (FLOAT, "originY"), (BOOL, "isSideways"), (BOOL, "isRightToLeft"), (ObjPointer(IUnknown), "clientDrawingEffect")]),
    StdMethod(HRESULT, "GetMetrics", [Out(Pointer(DWRITE_INLINE_OBJECT_METRICS), "metrics")]),
    StdMethod(HRESULT, "GetOverhangMetrics", [Out(Pointer(DWRITE_OVERHANG_METRICS), "overhangs")]),
    StdMethod(HRESULT, "GetBreakConditions", [Out(Pointer(DWRITE_BREAK_CONDITION), "breakConditionBefore"), Out(Pointer(DWRITE_BREAK_CONDITION), "breakConditionAfter")]),
]

IDWritePixelSnapping.methods += [
    StdMethod(HRESULT, "IsPixelSnappingDisabled", [(OpaquePointer(Void), "clientDrawingContext"), Out(Pointer(BOOL), "isDisabled")]),
    StdMethod(HRESULT, "GetCurrentTransform", [(OpaquePointer(Void), "clientDrawingContext"), Out(Pointer(DWRITE_MATRIX), "transform")]),
    StdMethod(HRESULT, "GetPixelsPerDip", [(OpaquePointer(Void), "clientDrawingContext"), Out(Pointer(FLOAT), "pixelsPerDip")]),
]

IDWriteTextRenderer.methods += [
    StdMethod(HRESULT, "DrawGlyphRun", [(OpaquePointer(Void), "clientDrawingContext"), (FLOAT, "baselineOriginX"), (FLOAT, "baselineOriginY"), (DWRITE_MEASURING_MODE, "measuringMode"), (Pointer(Const(DWRITE_GLYPH_RUN)), "glyphRun"), (Pointer(Const(DWRITE_GLYPH_RUN_DESCRIPTION)), "glyphRunDescription"), (ObjPointer(IUnknown), "clientDrawingEffect")]),
    StdMethod(HRESULT, "DrawUnderline", [(OpaquePointer(Void), "clientDrawingContext"), (FLOAT, "baselineOriginX"), (FLOAT, "baselineOriginY"), (Pointer(Const(DWRITE_UNDERLINE)), "underline"), (ObjPointer(IUnknown), "clientDrawingEffect")]),
    StdMethod(HRESULT, "DrawStrikethrough", [(OpaquePointer(Void), "clientDrawingContext"), (FLOAT, "baselineOriginX"), (FLOAT, "baselineOriginY"), (Pointer(Const(DWRITE_STRIKETHROUGH)), "strikethrough"), (ObjPointer(IUnknown), "clientDrawingEffect")]),
    StdMethod(HRESULT, "DrawInlineObject", [(OpaquePointer(Void), "clientDrawingContext"), (FLOAT, "originX"), (FLOAT, "originY"), (ObjPointer(IDWriteInlineObject), "inlineObject"), (BOOL, "isSideways"), (BOOL, "isRightToLeft"), (ObjPointer(IUnknown), "clientDrawingEffect")]),
]

IDWriteTextLayout.methods += [
    StdMethod(HRESULT, "SetMaxWidth", [(FLOAT, "maxWidth")]),
    StdMethod(HRESULT, "SetMaxHeight", [(FLOAT, "maxHeight")]),
    StdMethod(HRESULT, "SetFontCollection", [(ObjPointer(IDWriteFontCollection), "fontCollection"), (DWRITE_TEXT_RANGE, "textRange")]),
    StdMethod(HRESULT, "SetFontFamilyName", [(Pointer(Const(WCHAR)), "fontFamilyName"), (DWRITE_TEXT_RANGE, "textRange")]),
    StdMethod(HRESULT, "SetFontWeight", [(DWRITE_FONT_WEIGHT, "fontWeight"), (DWRITE_TEXT_RANGE, "textRange")]),
    StdMethod(HRESULT, "SetFontStyle", [(DWRITE_FONT_STYLE, "fontStyle"), (DWRITE_TEXT_RANGE, "textRange")]),
    StdMethod(HRESULT, "SetFontStretch", [(DWRITE_FONT_STRETCH, "fontStretch"), (DWRITE_TEXT_RANGE, "textRange")]),
    StdMethod(HRESULT, "SetFontSize", [(FLOAT, "fontSize"), (DWRITE_TEXT_RANGE, "textRange")]),
    StdMethod(HRESULT, "SetUnderline", [(BOOL, "hasUnderline"), (DWRITE_TEXT_RANGE, "textRange")]),
    StdMethod(HRESULT, "SetStrikethrough", [(BOOL, "hasStrikethrough"), (DWRITE_TEXT_RANGE, "textRange")]),
    StdMethod(HRESULT, "SetDrawingEffect", [(ObjPointer(IUnknown), "drawingEffect"), (DWRITE_TEXT_RANGE, "textRange")]),
    StdMethod(HRESULT, "SetInlineObject", [(ObjPointer(IDWriteInlineObject), "inlineObject"), (DWRITE_TEXT_RANGE, "textRange")]),
    StdMethod(HRESULT, "SetTypography", [(ObjPointer(IDWriteTypography), "typography"), (DWRITE_TEXT_RANGE, "textRange")]),
    StdMethod(HRESULT, "SetLocaleName", [(Pointer(Const(WCHAR)), "localeName"), (DWRITE_TEXT_RANGE, "textRange")]),
    StdMethod(FLOAT, "GetMaxWidth", []),
    StdMethod(FLOAT, "GetMaxHeight", []),
    StdMethod(HRESULT, "GetFontCollection", [(UINT32, "currentPosition"), Out(Pointer(ObjPointer(IDWriteFontCollection)), "fontCollection"), Out(Pointer(DWRITE_TEXT_RANGE), "textRange")]),
    StdMethod(HRESULT, "GetFontFamilyNameLength", [(UINT32, "currentPosition"), Out(Pointer(UINT32), "nameLength"), Out(Pointer(DWRITE_TEXT_RANGE), "textRange")]),
    StdMethod(HRESULT, "GetFontFamilyName", [(UINT32, "currentPosition"), Out(Pointer(WCHAR), "fontFamilyName"), (UINT32, "nameSize"), Out(Pointer(DWRITE_TEXT_RANGE), "textRange")]),
    StdMethod(HRESULT, "GetFontWeight", [(UINT32, "currentPosition"), Out(Pointer(DWRITE_FONT_WEIGHT), "fontWeight"), Out(Pointer(DWRITE_TEXT_RANGE), "textRange")]),
    StdMethod(HRESULT, "GetFontStyle", [(UINT32, "currentPosition"), Out(Pointer(DWRITE_FONT_STYLE), "fontStyle"), Out(Pointer(DWRITE_TEXT_RANGE), "textRange")]),
    StdMethod(HRESULT, "GetFontStretch", [(UINT32, "currentPosition"), Out(Pointer(DWRITE_FONT_STRETCH), "fontStretch"), Out(Pointer(DWRITE_TEXT_RANGE), "textRange")]),
    StdMethod(HRESULT, "GetFontSize", [(UINT32, "currentPosition"), Out(Pointer(FLOAT), "fontSize"), Out(Pointer(DWRITE_TEXT_RANGE), "textRange")]),
    StdMethod(HRESULT, "GetUnderline", [(UINT32, "currentPosition"), Out(Pointer(BOOL), "hasUnderline"), Out(Pointer(DWRITE_TEXT_RANGE), "textRange")]),
    StdMethod(HRESULT, "GetStrikethrough", [(UINT32, "currentPosition"), Out(Pointer(BOOL), "hasStrikethrough"), Out(Pointer(DWRITE_TEXT_RANGE), "textRange")]),
    StdMethod(HRESULT, "GetDrawingEffect", [(UINT32, "currentPosition"), Out(Pointer(ObjPointer(IUnknown)), "drawingEffect"), Out(Pointer(DWRITE_TEXT_RANGE), "textRange")]),
    StdMethod(HRESULT, "GetInlineObject", [(UINT32, "currentPosition"), Out(Pointer(ObjPointer(IDWriteInlineObject)), "inlineObject"), Out(Pointer(DWRITE_TEXT_RANGE), "textRange")]),
    StdMethod(HRESULT, "GetTypography", [(UINT32, "currentPosition"), Out(Pointer(ObjPointer(IDWriteTypography)), "typography"), Out(Pointer(DWRITE_TEXT_RANGE), "textRange")]),
    StdMethod(HRESULT, "GetLocaleNameLength", [(UINT32, "currentPosition"), Out(Pointer(UINT32), "nameLength"), Out(Pointer(DWRITE_TEXT_RANGE), "textRange")]),
    StdMethod(HRESULT, "GetLocaleName", [(UINT32, "currentPosition"), Out(Pointer(WCHAR), "localeName"), (UINT32, "nameSize"), Out(Pointer(DWRITE_TEXT_RANGE), "textRange")]),
    StdMethod(HRESULT, "Draw", [(OpaquePointer(Void), "clientDrawingContext"), (OpaquePointer(IDWriteTextRenderer), "renderer"), (FLOAT, "originX"), (FLOAT, "originY")]),
    StdMethod(HRESULT, "GetLineMetrics", [Out(Pointer(DWRITE_LINE_METRICS), "lineMetrics"), (UINT32, "maxLineCount"), Out(Pointer(UINT32), "actualLineCount")]),
    StdMethod(HRESULT, "GetMetrics", [Out(Pointer(DWRITE_TEXT_METRICS), "textMetrics")]),
    StdMethod(HRESULT, "GetOverhangMetrics", [Out(Pointer(DWRITE_OVERHANG_METRICS), "overhangs")]),
    StdMethod(HRESULT, "GetClusterMetrics", [Out(Pointer(DWRITE_CLUSTER_METRICS), "clusterMetrics"), (UINT32, "maxClusterCount"), Out(Pointer(UINT32), "actualClusterCount")]),
    StdMethod(HRESULT, "DetermineMinWidth", [Out(Pointer(FLOAT), "minWidth")]),
    StdMethod(HRESULT, "HitTestPoint", [(FLOAT, "pointX"), (FLOAT, "pointY"), Out(Pointer(BOOL), "isTrailingHit"), Out(Pointer(BOOL), "isInside"), Out(Pointer(DWRITE_HIT_TEST_METRICS), "hitTestMetrics")]),
    StdMethod(HRESULT, "HitTestTextPosition", [(UINT32, "textPosition"), (BOOL, "isTrailingHit"), Out(Pointer(FLOAT), "pointX"), Out(Pointer(FLOAT), "pointY"), Out(Pointer(DWRITE_HIT_TEST_METRICS), "hitTestMetrics")]),
    StdMethod(HRESULT, "HitTestTextRange", [(UINT32, "textPosition"), (UINT32, "textLength"), (FLOAT, "originX"), (FLOAT, "originY"), Out(Pointer(DWRITE_HIT_TEST_METRICS), "hitTestMetrics"), (UINT32, "maxHitTestMetricsCount"), Out(Pointer(UINT32), "actualHitTestMetricsCount")]),
]

IDWriteBitmapRenderTarget.methods += [
    StdMethod(HRESULT, "DrawGlyphRun", [(FLOAT, "baselineOriginX"), (FLOAT, "baselineOriginY"), (DWRITE_MEASURING_MODE, "measuringMode"), (Pointer(Const(DWRITE_GLYPH_RUN)), "glyphRun"), (ObjPointer(IDWriteRenderingParams), "renderingParams"), (COLORREF, "textColor"), Out(Pointer(RECT), "blackBoxRect")]),
    StdMethod(HDC, "GetMemoryDC", []),
    StdMethod(FLOAT, "GetPixelsPerDip", []),
    StdMethod(HRESULT, "SetPixelsPerDip", [(FLOAT, "pixelsPerDip")]),
    StdMethod(HRESULT, "GetCurrentTransform", [Out(Pointer(DWRITE_MATRIX), "transform")]),
    StdMethod(HRESULT, "SetCurrentTransform", [(Pointer(Const(DWRITE_MATRIX)), "transform")]),
    StdMethod(HRESULT, "GetSize", [Out(Pointer(SIZE), "size")]),
    StdMethod(HRESULT, "Resize", [(UINT32, "width"), (UINT32, "height")]),
]

IDWriteGdiInterop.methods += [
    StdMethod(HRESULT, "CreateFontFromLOGFONT", [(Pointer(Const(LOGFONTW)), "logFont"), Out(Pointer(ObjPointer(IDWriteFont)), "font")]),
    StdMethod(HRESULT, "ConvertFontToLOGFONT", [(ObjPointer(IDWriteFont), "font"), Out(Pointer(LOGFONTW), "logFont"), Out(Pointer(BOOL), "isSystemFont")]),
    StdMethod(HRESULT, "ConvertFontFaceToLOGFONT", [(ObjPointer(IDWriteFontFace), "font"), Out(Pointer(LOGFONTW), "logFont")]),
    StdMethod(HRESULT, "CreateFontFaceFromHdc", [(HDC, "hdc"), Out(Pointer(ObjPointer(IDWriteFontFace)), "fontFace")]),
    StdMethod(HRESULT, "CreateBitmapRenderTarget", [(HDC, "hdc"), (UINT32, "width"), (UINT32, "height"), Out(Pointer(ObjPointer(IDWriteBitmapRenderTarget)), "renderTarget")]),
]

DWRITE_TEXTURE_TYPE = Enum("DWRITE_TEXTURE_TYPE", [
    "DWRITE_TEXTURE_ALIASED_1x1",
    "DWRITE_TEXTURE_CLEARTYPE_3x1",
])

IDWriteGlyphRunAnalysis.methods += [
    StdMethod(HRESULT, "GetAlphaTextureBounds", [(DWRITE_TEXTURE_TYPE, "textureType"), Out(Pointer(RECT), "textureBounds")]),
    StdMethod(HRESULT, "CreateAlphaTexture", [(DWRITE_TEXTURE_TYPE, "textureType"), (Pointer(Const(RECT)), "textureBounds"), Out(Pointer(BYTE), "alphaValues"), (UINT32, "bufferSize")]),
    StdMethod(HRESULT, "GetAlphaBlendParams", [(ObjPointer(IDWriteRenderingParams), "renderingParams"), Out(Pointer(FLOAT), "blendGamma"), Out(Pointer(FLOAT), "blendEnhancedContrast"), Out(Pointer(FLOAT), "blendClearTypeLevel")]),
]

IDWriteFactory.methods += [
    StdMethod(HRESULT, "GetSystemFontCollection", [Out(Pointer(ObjPointer(IDWriteFontCollection)), "fontCollection"), (BOOL, "checkForUpdates")]),
    StdMethod(HRESULT, "CreateCustomFontCollection", [(ObjPointer(IDWriteFontCollectionLoader), "collectionLoader"), (OpaquePointer(Const(Void)), "collectionKey"), (UINT32, "collectionKeySize"), Out(Pointer(ObjPointer(IDWriteFontCollection)), "fontCollection")]),
    StdMethod(HRESULT, "RegisterFontCollectionLoader", [(ObjPointer(IDWriteFontCollectionLoader), "fontCollectionLoader")]),
    StdMethod(HRESULT, "UnregisterFontCollectionLoader", [(ObjPointer(IDWriteFontCollectionLoader), "fontCollectionLoader")]),
    StdMethod(HRESULT, "CreateFontFileReference", [(Pointer(Const(WCHAR)), "filePath"), (Pointer(Const(FILETIME)), "lastWriteTime"), Out(Pointer(ObjPointer(IDWriteFontFile)), "fontFile")]),
    StdMethod(HRESULT, "CreateCustomFontFileReference", [(OpaquePointer(Const(Void)), "fontFileReferenceKey"), (UINT32, "fontFileReferenceKeySize"), (ObjPointer(IDWriteFontFileLoader), "fontFileLoader"), Out(Pointer(ObjPointer(IDWriteFontFile)), "fontFile")]),
    StdMethod(HRESULT, "CreateFontFace", [(DWRITE_FONT_FACE_TYPE, "fontFaceType"), (UINT32, "numberOfFiles"), (Array(Const(ObjPointer(IDWriteFontFile)), "numberOfFiles"), "fontFiles"), (UINT32, "faceIndex"), (DWRITE_FONT_SIMULATIONS, "fontFaceSimulationFlags"), Out(Pointer(ObjPointer(IDWriteFontFace)), "fontFace")]),
    StdMethod(HRESULT, "CreateRenderingParams", [Out(Pointer(ObjPointer(IDWriteRenderingParams)), "renderingParams")]),
    StdMethod(HRESULT, "CreateMonitorRenderingParams", [(HMONITOR, "monitor"), Out(Pointer(ObjPointer(IDWriteRenderingParams)), "renderingParams")]),
    StdMethod(HRESULT, "CreateCustomRenderingParams", [(FLOAT, "gamma"), (FLOAT, "enhancedContrast"), (FLOAT, "clearTypeLevel"), (DWRITE_PIXEL_GEOMETRY, "pixelGeometry"), (DWRITE_RENDERING_MODE, "renderingMode"), Out(Pointer(ObjPointer(IDWriteRenderingParams)), "renderingParams")]),
    StdMethod(HRESULT, "RegisterFontFileLoader", [(ObjPointer(IDWriteFontFileLoader), "fontFileLoader")]),
    StdMethod(HRESULT, "UnregisterFontFileLoader", [(ObjPointer(IDWriteFontFileLoader), "fontFileLoader")]),
    StdMethod(HRESULT, "CreateTextFormat", [(Pointer(Const(WCHAR)), "fontFamilyName"), (ObjPointer(IDWriteFontCollection), "fontCollection"), (DWRITE_FONT_WEIGHT, "fontWeight"), (DWRITE_FONT_STYLE, "fontStyle"), (DWRITE_FONT_STRETCH, "fontStretch"), (FLOAT, "fontSize"), (Pointer(Const(WCHAR)), "localeName"), Out(Pointer(ObjPointer(IDWriteTextFormat)), "textFormat")]),
    StdMethod(HRESULT, "CreateTypography", [Out(Pointer(ObjPointer(IDWriteTypography)), "typography")]),
    StdMethod(HRESULT, "GetGdiInterop", [Out(Pointer(ObjPointer(IDWriteGdiInterop)), "gdiInterop")]),
    StdMethod(HRESULT, "CreateTextLayout", [(Pointer(Const(WCHAR)), "string"), (UINT32, "stringLength"), (ObjPointer(IDWriteTextFormat), "textFormat"), (FLOAT, "maxWidth"), (FLOAT, "maxHeight"), Out(Pointer(ObjPointer(IDWriteTextLayout)), "textLayout")]),
    StdMethod(HRESULT, "CreateGdiCompatibleTextLayout", [(Pointer(Const(WCHAR)), "string"), (UINT32, "stringLength"), (ObjPointer(IDWriteTextFormat), "textFormat"), (FLOAT, "layoutWidth"), (FLOAT, "layoutHeight"), (FLOAT, "pixelsPerDip"), (Pointer(Const(DWRITE_MATRIX)), "transform"), (BOOL, "useGdiNatural"), Out(Pointer(ObjPointer(IDWriteTextLayout)), "textLayout")]),
    StdMethod(HRESULT, "CreateEllipsisTrimmingSign", [(ObjPointer(IDWriteTextFormat), "textFormat"), Out(Pointer(ObjPointer(IDWriteInlineObject)), "trimmingSign")]),
    StdMethod(HRESULT, "CreateTextAnalyzer", [Out(Pointer(ObjPointer(IDWriteTextAnalyzer)), "textAnalyzer")]),
    StdMethod(HRESULT, "CreateNumberSubstitution", [(DWRITE_NUMBER_SUBSTITUTION_METHOD, "substitutionMethod"), (Pointer(Const(WCHAR)), "localeName"), (BOOL, "ignoreUserOverride"), Out(Pointer(ObjPointer(IDWriteNumberSubstitution)), "numberSubstitution")]),
    StdMethod(HRESULT, "CreateGlyphRunAnalysis", [(Pointer(Const(DWRITE_GLYPH_RUN)), "glyphRun"), (FLOAT, "pixelsPerDip"), (Pointer(Const(DWRITE_MATRIX)), "transform"), (DWRITE_RENDERING_MODE, "renderingMode"), (DWRITE_MEASURING_MODE, "measuringMode"), (FLOAT, "baselineOriginX"), (FLOAT, "baselineOriginY"), Out(Pointer(ObjPointer(IDWriteGlyphRunAnalysis)), "glyphRunAnalysis")]),
]

dwrite = Module("dwrite")
dwrite.addInterfaces([
    IDWriteFactory
])
dwrite.addFunctions([
    StdFunction(HRESULT, "DWriteCreateFactory", [(DWRITE_FACTORY_TYPE, "factoryType"), (REFIID, "iid"), Out(Pointer(ObjPointer(IUnknown)), "factory")]),
])
