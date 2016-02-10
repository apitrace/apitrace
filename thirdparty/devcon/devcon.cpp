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


#include "devcon.hpp"

#include <string.h>
#include <stdio.h>

#include <windows.h>
#include <tchar.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <newdev.h>
#include <strsafe.h>


// Not defined on MinGW and Windows SDKs before Windows 8.0
#define PZPTSTR PTSTR *

#ifndef ARRAYSIZE
#define ARRAYSIZE(a)                (sizeof(a)/sizeof(a[0]))
#endif

#define INSTANCEID_PREFIX_CHAR TEXT('@') // character used to prefix instance ID's
#define CLASS_PREFIX_CHAR      TEXT('=') // character used to prefix class name
#define WILD_CHAR              TEXT('*') // wild character
#define QUOTE_PREFIX_CHAR      TEXT('\'') // prefix character to ignore wild characters

#define MSG_FIND_TAIL_NONE_LOCAL TEXT("No matching devices found.\n")
#define MSG_ENABLE_TAIL TEXT("%lu device(s) are enabled.\n")
#define MSG_ENABLE_TAIL_REBOOT TEXT("The %lu device(s) are ready to be enabled. To enable the devices, restart the devices or reboot the system.\n")
#define MSG_DISABLE_TAIL TEXT("%lu device(s) are disabled.\n")
#define MSG_DISABLE_TAIL_REBOOT TEXT("The %lu device(s) are ready to be disabled. To disable the devices, restart the devices or reboot the system.\n")
#define MSG_RESTART_TAIL TEXT("%lu device(s) are restarted.\n")
#define MSG_RESTART_TAIL_REBOOT TEXT("The %lu device(s) are ready to be restarted. To restart the devices, reboot the system.\n")

#define IDS_ENABLED          TEXT("Enabled")
#define IDS_ENABLED_REBOOT   TEXT("Enabled on reboot")
#define IDS_ENABLE_FAILED    TEXT("Enable failed")
#define IDS_DISABLED         TEXT("Disabled")
#define IDS_DISABLED_REBOOT  TEXT("Disabled on reboot")
#define IDS_DISABLE_FAILED   TEXT("Disable failed")
#define IDS_RESTARTED        TEXT("Restarted")
#define IDS_REQUIRES_REBOOT  TEXT("Requires reboot")
#define IDS_RESTART_FAILED   TEXT("Restart failed")
#define IDS_REMOVED          TEXT("Removed")
#define IDS_REMOVED_REBOOT   TEXT("Removed on reboot")
#define IDS_REMOVE_FAILED    TEXT("Remove failed")

struct IdEntry {
    LPCTSTR String;     // string looking for
    LPCTSTR Wild;       // first wild character if any
    BOOL    InstanceId;
};


static IdEntry
GetIdType(LPCTSTR Id)
/*++

Routine Description:

    Determine if this is instance id or hardware id and if there's any wildcards
    instance ID is prefixed by '@'
    wildcards are '*'


Arguments:

    Id - ptr to string to check

Return Value:

    IdEntry

--*/
{
    IdEntry Entry;

    Entry.InstanceId = FALSE;
    Entry.Wild = NULL;
    Entry.String = Id;

    if(Entry.String[0] == INSTANCEID_PREFIX_CHAR) {
        Entry.InstanceId = TRUE;
        Entry.String = CharNext(Entry.String);
    }
    if(Entry.String[0] == QUOTE_PREFIX_CHAR) {
        //
        // prefix to treat rest of string literally
        //
        Entry.String = CharNext(Entry.String);
    } else {
        //
        // see if any wild characters exist
        //
        Entry.Wild = _tcschr(Entry.String, WILD_CHAR);
    }
    return Entry;
}


static BOOL
WildCardMatch(LPCTSTR Item, const IdEntry & MatchEntry)
/*++

Routine Description:

    Compare a single item against wildcard
    I'm sure there's better ways of implementing this
    Other than a command-line management tools
    it's a bad idea to use wildcards as it implies
    assumptions about the hardware/instance ID
    eg, it might be tempting to enumerate root\* to
    find all root devices, however there is a CfgMgr
    API to query status and determine if a device is
    root enumerated, which doesn't rely on implementation
    details.

Arguments:

    Item - item to find match for eg a\abcd\c
    MatchEntry - eg *\*bc*\*

Return Value:

    TRUE if any match, otherwise FALSE

--*/
{
    LPCTSTR scanItem;
    LPCTSTR wildMark;
    LPCTSTR nextWild;
    size_t matchlen;

    //
    // before attempting anything else
    // try and compare everything up to first wild
    //
    if(!MatchEntry.Wild) {
        return _tcsicmp(Item, MatchEntry.String) ? FALSE : TRUE;
    }
    if(_tcsnicmp(Item, MatchEntry.String, MatchEntry.Wild-MatchEntry.String) != 0) {
        return FALSE;
    }
    wildMark = MatchEntry.Wild;
    scanItem = Item + (MatchEntry.Wild-MatchEntry.String);

    for(;wildMark[0];) {
        //
        // if we get here, we're either at or past a wildcard
        //
        if(wildMark[0] == WILD_CHAR) {
            //
            // so skip wild chars
            //
            wildMark = CharNext(wildMark);
            continue;
        }
        //
        // find next wild-card
        //
        nextWild = _tcschr(wildMark, WILD_CHAR);
        if(nextWild) {
            //
            // substring
            //
            matchlen = nextWild-wildMark;
        } else {
            //
            // last portion of match
            //
            size_t scanlen = lstrlen(scanItem);
            matchlen = lstrlen(wildMark);
            if(scanlen < matchlen) {
                return FALSE;
            }
            return _tcsicmp(scanItem+scanlen-matchlen, wildMark) ? FALSE : TRUE;
        }
        if(_istalpha(wildMark[0])) {
            //
            // scan for either lower or uppercase version of first character
            //

            //
            // the code suppresses the warning 28193 for the calls to _totupper
            // and _totlower.  This suppression is done because those functions
            // have a check return annotation on them.  However, they don't return
            // error codes and the check return annotation is really being used
            // to indicate that the return value of the function should be looked
            // at and/or assigned to a variable.  The check return annotation means
            // the return value should always be checked in all code paths.
            // We assign the return values to variables but the while loop does not
            // examine both values in all code paths (e.g. when scanItem[0] == 0,
            // neither u nor l will be examined) and it doesn't need to examine
            // the values in all code paths.
            //
#ifdef _MSC_VER
#pragma warning( suppress: 28193)
#endif
            TCHAR u = _totupper(wildMark[0]);
#ifdef _MSC_VER
#pragma warning( suppress: 28193)
#endif
            TCHAR l = _totlower(wildMark[0]);
            while(scanItem[0] && scanItem[0]!=u && scanItem[0]!=l) {
                scanItem = CharNext(scanItem);
            }
            if(!scanItem[0]) {
                //
                // ran out of string
                //
                return FALSE;
            }
        } else {
            //
            // scan for first character (no case)
            //
            scanItem = _tcschr(scanItem, wildMark[0]);
            if(!scanItem) {
                //
                // ran out of string
                //
                return FALSE;
            }
        }
        //
        // try and match the sub-string at wildMark against scanItem
        //
        if(_tcsnicmp(scanItem, wildMark, matchlen)!=0) {
            //
            // nope, try again
            //
            scanItem = CharNext(scanItem);
            continue;
        }
        //
        // substring matched
        //
        scanItem += matchlen;
        wildMark += matchlen;
    }
    return (wildMark[0] ? FALSE : TRUE);
}


static BOOL
WildCompareHwIds(PZPTSTR Array, const IdEntry & MatchEntry)
/*++

Routine Description:

    Compares all strings in Array against Id
    Use WildCardMatch to do real compare

Arguments:

    Array - pointer returned by GetDevMultiSz
    MatchEntry - string to compare against

Return Value:

    TRUE if any match, otherwise FALSE

--*/
{
    if(Array) {
        while(Array[0]) {
            if(WildCardMatch(Array[0], MatchEntry)) {
                return TRUE;
            }
            Array++;
        }
    }
    return FALSE;
}


static LPTSTR *
GetMultiSzIndexArray(LPTSTR MultiSz)
/*++

Routine Description:

    Get an index array pointing to the MultiSz passed in

Arguments:

    MultiSz - well formed multi-sz string

Return Value:

    array of strings. last entry+1 of array contains NULL
    returns NULL on failure

--*/
{
    LPTSTR scan;
    LPTSTR * array;
    int elements;

    for(scan = MultiSz, elements = 0; scan[0] ;elements++) {
        scan += lstrlen(scan)+1;
    }
    array = new LPTSTR[elements+2];
    if(!array) {
        return NULL;
    }
    array[0] = MultiSz;
    array++;
    if(elements) {
        for(scan = MultiSz, elements = 0; scan[0]; elements++) {
            array[elements] = scan;
            scan += lstrlen(scan)+1;
        }
    }
    array[elements] = NULL;
    return array;
}


static void
DelMultiSz(PZPTSTR Array)
/*++

Routine Description:

    Deletes the string array allocated by GetDevMultiSz/GetRegMultiSz/GetMultiSzIndexArray

Arguments:

    Array - pointer returned by GetMultiSzIndexArray

Return Value:

    None

--*/
{
    if(Array) {
        Array--;
        if(Array[0]) {
            delete [] Array[0];
        }
        delete [] Array;
    }
}


static LPTSTR *
GetDevMultiSz(HDEVINFO Devs, PSP_DEVINFO_DATA DevInfo, DWORD Prop)
/*++

Routine Description:

    Get a multi-sz device property
    and return as an array of strings

Arguments:

    Devs    - HDEVINFO containing DevInfo
    DevInfo - Specific device
    Prop    - SPDRP_HARDWAREID or SPDRP_COMPATIBLEIDS

Return Value:

    array of strings. last entry+1 of array contains NULL
    returns NULL on failure

--*/
{
    LPTSTR buffer;
    DWORD size;
    DWORD reqSize;
    DWORD dataType;
    LPTSTR * array;
    DWORD szChars;

    size = 8192; // initial guess, nothing magic about this
    buffer = new TCHAR[(size/sizeof(TCHAR))+2];
    if(!buffer) {
        return NULL;
    }
    while(!SetupDiGetDeviceRegistryProperty(Devs, DevInfo, Prop, &dataType, (LPBYTE)buffer, size, &reqSize)) {
        if(GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            goto failed;
        }
        if(dataType != REG_MULTI_SZ) {
            goto failed;
        }
        size = reqSize;
        delete [] buffer;
        buffer = new TCHAR[(size/sizeof(TCHAR))+2];
        if(!buffer) {
            goto failed;
        }
    }
    szChars = reqSize/sizeof(TCHAR);
    buffer[szChars] = TEXT('\0');
    buffer[szChars+1] = TEXT('\0');
    array = GetMultiSzIndexArray(buffer);
    if(array) {
        return array;
    }

failed:
    if(buffer) {
        delete [] buffer;
    }
    return NULL;
}


typedef int (*CallbackFunc)(HDEVINFO Devs, PSP_DEVINFO_DATA DevInfo, DWORD Index, LPVOID Context);


static int
EnumerateDevices(DWORD Flags, int argc, PCTSTR* argv, CallbackFunc Callback, LPVOID Context)
/*++

Routine Description:

    Generic enumerator for devices that will be passed the following arguments:
    <id> [<id>...]
    =<class> [<id>...]
    where <id> can either be @instance-id, or hardware-id and may contain wildcards
    <class> is a class name

Arguments:

    Flags    - extra enumeration flags (eg DIGCF_PRESENT)
    argc/argv - remaining arguments on command line
    Callback - function to call for each hit
    Context  - data to pass function for each hit

Return Value:

    DEVCON_xxxx

--*/
{
    HDEVINFO devs = INVALID_HANDLE_VALUE;
    IdEntry * templ = NULL;
    int failcode = DEVCON_FAIL;
    int retcode;
    int argIndex;
    DWORD devIndex;
    SP_DEVINFO_DATA devInfo;
    SP_DEVINFO_LIST_DETAIL_DATA devInfoListDetail;
    BOOL doSearch = FALSE;
    BOOL match;
    BOOL all = FALSE;
    GUID cls;
    DWORD numClass = 0;
    int skip = 0;

    if(!argc) {
        return DEVCON_USAGE;
    }

    templ = new IdEntry[argc];
    if(!templ) {
        goto final;
    }

    //
    // determine if a class is specified
    //
    if(argc>skip && argv[skip][0]==CLASS_PREFIX_CHAR && argv[skip][1]) {
        if(!SetupDiClassGuidsFromNameEx(argv[skip]+1, &cls, 1, &numClass, NULL, NULL) &&
            GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            goto final;
        }
        if(!numClass) {
            failcode = DEVCON_OK;
            goto final;
        }
        skip++;
    }
    if(argc>skip && argv[skip][0]==WILD_CHAR && !argv[skip][1]) {
        //
        // catch convinient case of specifying a single argument '*'
        //
        all = TRUE;
        skip++;
    } else if(argc<=skip) {
        //
        // at least one parameter, but no <id>'s
        //
        all = TRUE;
    }

    //
    // determine if any instance id's were specified
    //
    // note, if =<class> was specified with no id's
    // we'll mark it as not doSearch
    // but will go ahead and add them all
    //
    for(argIndex=skip;argIndex<argc;argIndex++) {
        templ[argIndex] = GetIdType(argv[argIndex]);
        if(templ[argIndex].Wild || !templ[argIndex].InstanceId) {
            //
            // anything other than simple InstanceId's require a search
            //
            doSearch = TRUE;
        }
    }
    if(doSearch || all) {
        //
        // add all id's to list
        // if there's a class, filter on specified class
        //
        devs = SetupDiGetClassDevsEx(numClass ? &cls : NULL,
                                     NULL,
                                     NULL,
                                     (numClass ? 0 : DIGCF_ALLCLASSES) | Flags,
                                     NULL,
                                     NULL,
                                     NULL);

    } else {
        //
        // blank list, we'll add instance id's by hand
        //
        devs = SetupDiCreateDeviceInfoListEx(numClass ? &cls : NULL,
                                             NULL,
                                             NULL,
                                             NULL);
    }
    if(devs == INVALID_HANDLE_VALUE) {
        goto final;
    }
    for(argIndex=skip;argIndex<argc;argIndex++) {
        //
        // add explicit instances to list (even if enumerated all,
        // this gets around DIGCF_PRESENT)
        // do this even if wildcards appear to be detected since they
        // might actually be part of the instance ID of a non-present device
        //
        if(templ[argIndex].InstanceId) {
            SetupDiOpenDeviceInfo(devs, templ[argIndex].String, NULL, 0, NULL);
        }
    }

    devInfoListDetail.cbSize = sizeof(devInfoListDetail);
    if(!SetupDiGetDeviceInfoListDetail(devs, &devInfoListDetail)) {
        goto final;
    }

    //
    // now enumerate them
    //
    if(all) {
        doSearch = FALSE;
    }

    devInfo.cbSize = sizeof(devInfo);
    for(devIndex=0;SetupDiEnumDeviceInfo(devs, devIndex, &devInfo);devIndex++) {

        if(doSearch) {
            for(argIndex=skip, match=FALSE;(argIndex<argc) && !match;argIndex++) {
                TCHAR devID[MAX_DEVICE_ID_LEN];
                LPTSTR *hwIds = NULL;
                LPTSTR *compatIds = NULL;
                //
                // determine instance ID
                //
                if(CM_Get_Device_ID_Ex(devInfo.DevInst, devID, MAX_DEVICE_ID_LEN, 0, devInfoListDetail.RemoteMachineHandle)!=CR_SUCCESS) {
                    devID[0] = TEXT('\0');
                }

                if(templ[argIndex].InstanceId) {
                    //
                    // match on the instance ID
                    //
                    if(WildCardMatch(devID, templ[argIndex])) {
                        match = TRUE;
                    }
                } else {
                    //
                    // determine hardware ID's
                    // and search for matches
                    //
                    hwIds = GetDevMultiSz(devs, &devInfo, SPDRP_HARDWAREID);
                    compatIds = GetDevMultiSz(devs, &devInfo, SPDRP_COMPATIBLEIDS);

                    if(WildCompareHwIds(hwIds, templ[argIndex]) ||
                        WildCompareHwIds(compatIds, templ[argIndex])) {
                        match = TRUE;
                    }
                }
                DelMultiSz(hwIds);
                DelMultiSz(compatIds);
            }
        } else {
            match = TRUE;
        }
        if(match) {
            retcode = Callback(devs, &devInfo, devIndex, Context);
            if(retcode) {
                failcode = retcode;
                goto final;
            }
        }
    }

    failcode = DEVCON_OK;

final:
    if(templ) {
        delete [] templ;
    }
    if(devs != INVALID_HANDLE_VALUE) {
        SetupDiDestroyDeviceInfoList(devs);
    }
    return failcode;

}


struct GenericContext {
    DWORD count;
    DWORD control;
    BOOL  reboot;
    LPCTSTR strSuccess;
    LPCTSTR strReboot;
    LPCTSTR strFail;
};


static BOOL
DumpDeviceWithInfo(HDEVINFO Devs, PSP_DEVINFO_DATA DevInfo, LPCTSTR Info)
/*++

Routine Description:

    Write device instance & info to stdout

Arguments:

    Devs    )_ uniquely identify device
    DevInfo )

Return Value:

    none

--*/
{
    TCHAR devID[MAX_DEVICE_ID_LEN];
    BOOL b = TRUE;
    SP_DEVINFO_LIST_DETAIL_DATA devInfoListDetail;

    devInfoListDetail.cbSize = sizeof(devInfoListDetail);
    if((!SetupDiGetDeviceInfoListDetail(Devs, &devInfoListDetail)) ||
            (CM_Get_Device_ID_Ex(DevInfo->DevInst, devID, MAX_DEVICE_ID_LEN, 0, devInfoListDetail.RemoteMachineHandle)!=CR_SUCCESS)) {
        StringCchCopy(devID, ARRAYSIZE(devID), TEXT("?"));
        b = FALSE;
    }

    if(Info) {
        _tprintf(TEXT("%-60s: %s\n"), devID, Info);
    } else {
        _tprintf(TEXT("%s\n"), devID);
    }
    return b;
}


static int
ControlCallback(HDEVINFO Devs, PSP_DEVINFO_DATA DevInfo, DWORD Index, LPVOID Context)
/*++

Routine Description:

    Callback for use by Enable/Disable/Restart
    Invokes DIF_PROPERTYCHANGE with correct parameters
    uses SetupDiCallClassInstaller so cannot be done for remote devices
    Don't use CM_xxx API's, they bypass class/co-installers and this is bad.

    In Enable case, we try global first, and if still disabled, enable local

Arguments:

    Devs    )_ uniquely identify the device
    DevInfo )
    Index    - index of device
    Context  - GenericContext

Return Value:

    DEVCON_xxxx

--*/
{
    SP_PROPCHANGE_PARAMS pcp;
    GenericContext *pControlContext = (GenericContext*)Context;
    SP_DEVINSTALL_PARAMS devParams;

    UNREFERENCED_PARAMETER(Index);

    switch(pControlContext->control) {
        case DICS_ENABLE:
            //
            // enable both on global and config-specific profile
            // do global first and see if that succeeded in enabling the device
            // (global enable doesn't mark reboot required if device is still
            // disabled on current config whereas vice-versa isn't true)
            //
            pcp.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
            pcp.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
            pcp.StateChange = pControlContext->control;
            pcp.Scope = DICS_FLAG_GLOBAL;
            pcp.HwProfile = 0;
            //
            // don't worry if this fails, we'll get an error when we try config-
            // specific.
            if(SetupDiSetClassInstallParams(Devs, DevInfo, &pcp.ClassInstallHeader, sizeof(pcp))) {
               SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, Devs, DevInfo);
            }
            //
            // now enable on config-specific
            //
            pcp.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
            pcp.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
            pcp.StateChange = pControlContext->control;
            pcp.Scope = DICS_FLAG_CONFIGSPECIFIC;
            pcp.HwProfile = 0;
            break;

        default:
            //
            // operate on config-specific profile
            //
            pcp.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
            pcp.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
            pcp.StateChange = pControlContext->control;
            pcp.Scope = DICS_FLAG_CONFIGSPECIFIC;
            pcp.HwProfile = 0;
            break;

    }

    if(!SetupDiSetClassInstallParams(Devs, DevInfo, &pcp.ClassInstallHeader, sizeof(pcp)) ||
       !SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, Devs, DevInfo)) {
        //
        // failed to invoke DIF_PROPERTYCHANGE
        //
        DumpDeviceWithInfo(Devs, DevInfo, pControlContext->strFail);
    } else {
        //
        // see if device needs reboot
        //
        devParams.cbSize = sizeof(devParams);
        if(SetupDiGetDeviceInstallParams(Devs, DevInfo, &devParams) && (devParams.Flags & (DI_NEEDRESTART|DI_NEEDREBOOT))) {
                DumpDeviceWithInfo(Devs, DevInfo, pControlContext->strReboot);
                pControlContext->reboot = TRUE;
        } else {
            //
            // appears to have succeeded
            //
            DumpDeviceWithInfo(Devs, DevInfo, pControlContext->strSuccess);
        }
        pControlContext->count++;
    }
    return DEVCON_OK;
}


int
devconEnable(int argc, PCTSTR argv[])
/*++

Routine Description:

    ENABLE <id> ...
    use EnumerateDevices to do hardwareID matching
    for each match, attempt to enable global, and if needed, config specific

Arguments:

    argc/argv - remaining parameters - passed into EnumerateDevices

Return Value:

    DEVCON_xxxx (DEVCON_REBOOT if reboot is required)

--*/
{
    GenericContext context;
    int failcode = DEVCON_FAIL;

    if(!argc) {
        //
        // arguments required
        //
        return DEVCON_USAGE;
    }

    context.control = DICS_ENABLE; // DICS_PROPCHANGE DICS_ENABLE DICS_DISABLE
    context.reboot = FALSE;
    context.count = 0;
    context.strReboot = IDS_ENABLED_REBOOT;
    context.strSuccess = IDS_ENABLED;
    context.strFail = IDS_ENABLE_FAILED;
    failcode = EnumerateDevices(DIGCF_PRESENT, argc, argv, ControlCallback, &context);

    if(failcode == DEVCON_OK) {

        if(!context.count) {
            fprintf(stdout, MSG_FIND_TAIL_NONE_LOCAL);
        } else if(!context.reboot) {
            fprintf(stdout, MSG_ENABLE_TAIL, context.count);
        } else {
            fprintf(stdout, MSG_ENABLE_TAIL_REBOOT, context.count);
            failcode = DEVCON_REBOOT;
        }
    }
    return failcode;
}


int
devconDisable(int argc, PCTSTR argv[])
/*++

Routine Description:

    DISABLE <id> ...
    use EnumerateDevices to do hardwareID matching
    for each match, attempt to disable global

Arguments:

    argc/argv - remaining parameters - passed into EnumerateDevices

Return Value:

    DEVCON_xxxx (DEVCON_REBOOT if reboot is required)

--*/
{
    GenericContext context;
    int failcode = DEVCON_FAIL;

    if(!argc) {
        //
        // arguments required
        //
        return DEVCON_USAGE;
    }

    context.control = DICS_DISABLE; // DICS_PROPCHANGE DICS_ENABLE DICS_DISABLE
    context.reboot = FALSE;
    context.count = 0;
    context.strReboot = IDS_DISABLED_REBOOT;
    context.strSuccess = IDS_DISABLED;
    context.strFail = IDS_DISABLE_FAILED;
    failcode = EnumerateDevices(DIGCF_PRESENT, argc, argv, ControlCallback, &context);

    if(failcode == DEVCON_OK) {

        if(!context.count) {
            fprintf(stdout, MSG_FIND_TAIL_NONE_LOCAL);
        } else if(!context.reboot) {
            fprintf(stdout, MSG_DISABLE_TAIL, context.count);
        } else {
            fprintf(stdout, MSG_DISABLE_TAIL_REBOOT, context.count);
            failcode = DEVCON_REBOOT;
        }
    }
    return failcode;
}


int
devconRestart(int argc, PCTSTR argv[])
/*++

Routine Description:

    RESTART <id> ...
    use EnumerateDevices to do hardwareID matching
    for each match, attempt to restart by issueing a PROPCHANGE

Arguments:

    argc/argv - remaining parameters - passed into EnumerateDevices

Return Value:

    DEVCON_xxxx (DEVCON_REBOOT if reboot is required)

--*/
{
    GenericContext context;
    int failcode = DEVCON_FAIL;

    if(!argc) {
        //
        // arguments required
        //
        return DEVCON_USAGE;
    }

    context.control = DICS_PROPCHANGE;
    context.reboot = FALSE;
    context.count = 0;
    context.strReboot = IDS_REQUIRES_REBOOT;
    context.strSuccess = IDS_RESTARTED;
    context.strFail = IDS_RESTART_FAILED;
    failcode = EnumerateDevices(DIGCF_PRESENT, argc, argv, ControlCallback, &context);

    if(failcode == DEVCON_OK) {

        if(!context.count) {
            fprintf(stdout, MSG_FIND_TAIL_NONE_LOCAL);
        } else if(!context.reboot) {
            fprintf(stdout, MSG_RESTART_TAIL, context.count);
        } else {
            fprintf(stdout, MSG_RESTART_TAIL_REBOOT, context.count);
            failcode = DEVCON_REBOOT;
        }
    }
    return failcode;
}
