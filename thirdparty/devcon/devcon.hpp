/***************************************************************************
 *
 * Copyright (c) Microsoft Corporation.  All rights reserved.
 *
 * This source is subject to the Microsoft Public License.
 * See http://www.microsoft.com/en-us/openness/licenses.aspx#MPL.
 * All other rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 ***************************************************************************/

/*
 * Interface for managing devices.
 *
 * http://code.msdn.microsoft.com/windowshardware/DevCon-Sample-4e95d71c
 * http://support.microsoft.com/kb/311272
 */




#pragma once


#include <windows.h>
#include <tchar.h>


//
// exit codes
//
#define DEVCON_OK      (0)
#define DEVCON_REBOOT  (1)
#define DEVCON_FAIL    (2)
#define DEVCON_USAGE   (3)


#define DEVCON_CLASS_DISPLAY TEXT("=DISPLAY")


int
devconEnable(int argc, PCTSTR argv[]);

int
devconDisable(int argc, PCTSTR argv[]);

int
devconRestart(int argc, PCTSTR argv[]);


static inline int
devconEnable(PCTSTR arg)
{
    return devconEnable(1, &arg);
}

static inline int
devconDisable(PCTSTR arg)
{
    return devconDisable(1, &arg);
}

static inline int
devconRestart(PCTSTR arg)
{
    return devconRestart(1, &arg);
}
