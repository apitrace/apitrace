/**************************************************************************
 *
 * Copyright 2016 VMware, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/

#pragma once


#include <windows.h>

#include "winsdk_compat.h"

#include <dcomp.h>

// These are necessary on MinGW because DECLSPEC_UUID(x) is a no-op
#ifdef __MINGW32__
EXTERN_C CONST IID IID_IDCompositionDevice;
EXTERN_C CONST IID IID_IDCompositionTarget;
EXTERN_C CONST IID IID_IDCompositionVisual;
EXTERN_C CONST IID IID_IDCompositionEffect;
EXTERN_C CONST IID IID_IDCompositionTransform3D;
EXTERN_C CONST IID IID_IDCompositionTransform;
EXTERN_C CONST IID IID_IDCompositionTranslateTransform;
EXTERN_C CONST IID IID_IDCompositionScaleTransform;
EXTERN_C CONST IID IID_IDCompositionRotateTransform;
EXTERN_C CONST IID IID_IDCompositionSkewTransform;
EXTERN_C CONST IID IID_IDCompositionMatrixTransform;
EXTERN_C CONST IID IID_IDCompositionEffectGroup;
EXTERN_C CONST IID IID_IDCompositionTranslateTransform3D;
EXTERN_C CONST IID IID_IDCompositionScaleTransform3D;
EXTERN_C CONST IID IID_IDCompositionRotateTransform3D;
EXTERN_C CONST IID IID_IDCompositionMatrixTransform3D;
EXTERN_C CONST IID IID_IDCompositionClip;
EXTERN_C CONST IID IID_IDCompositionRectangleClip;
EXTERN_C CONST IID IID_IDCompositionSurface;
EXTERN_C CONST IID IID_IDCompositionVirtualSurface;
EXTERN_C CONST IID IID_IDCompositionDevice2;
EXTERN_C CONST IID IID_IDCompositionDesktopDevice;
EXTERN_C CONST IID IID_IDCompositionDeviceDebug;
EXTERN_C CONST IID IID_IDCompositionSurfaceFactory;
EXTERN_C CONST IID IID_IDCompositionVisual2;
EXTERN_C CONST IID IID_IDCompositionVisualDebug;
EXTERN_C CONST IID IID_IDCompositionDevice3;
EXTERN_C CONST IID IID_IDCompositionFilterEffect;
EXTERN_C CONST IID IID_IDCompositionGaussianBlurEffect;
EXTERN_C CONST IID IID_IDCompositionBrightnessEffect;
EXTERN_C CONST IID IID_IDCompositionColorMatrixEffect;
EXTERN_C CONST IID IID_IDCompositionShadowEffect;
EXTERN_C CONST IID IID_IDCompositionHueRotationEffect;
EXTERN_C CONST IID IID_IDCompositionSaturationEffect;
EXTERN_C CONST IID IID_IDCompositionTurbulenceEffect;
EXTERN_C CONST IID IID_IDCompositionLinearTransferEffect;
EXTERN_C CONST IID IID_IDCompositionTableTransferEffect;
EXTERN_C CONST IID IID_IDCompositionCompositeEffect;
EXTERN_C CONST IID IID_IDCompositionBlendEffect;
EXTERN_C CONST IID IID_IDCompositionArithmeticCompositeEffect;
EXTERN_C CONST IID IID_IDCompositionAffineTransform2DEffect;
#endif
