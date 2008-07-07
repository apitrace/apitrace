"""windows.h"""

from base import *

INT = Intrinsic("INT", "%u")
UINT = Intrinsic("UINT", "%u")
LONG = Intrinsic("LONG", "%li")
ULONG = Intrinsic("ULONG", "%lu")

BYTE = Intrinsic("BYTE", "0x%02lu")
WORD = Intrinsic("WORD", "0x%04lu")
DWORD = Intrinsic("DWORD", "0x%08lu")

BOOL = Intrinsic("BOOL", "%i")

LARGE_INTEGER = Intrinsic("LARGE_INTEGER", "0x%llx")

HRESULT = Alias("HRESULT", Int)

PVOID = Intrinsic("PVOID", "%p")
HWND = Intrinsic("HWND", "%p")
HMONITOR = Intrinsic("HMONITOR", "%p")

REFIID = Alias("REFIID", PVOID)
GUID = Alias("GUID", PVOID)

POINT = Struct("POINT", (
  (LONG, "x"),
  (LONG, "y"),
)) 

RECT = Struct("RECT", (
  (LONG, "left"),
  (LONG, "top"),
  (LONG, "right"), 
  (LONG, "bottom"), 
)) 

PALETTEENTRY = Struct("PALETTEENTRY", (
  (BYTE, "peRed"),
  (BYTE, "peGreen"),
  (BYTE, "peBlue"), 
  (BYTE, "peFlags"), 
)) 

RGNDATA = Struct("RGNDATA", ())
REFGUID = Alias("REFGUID", PVOID)


IUnknown = Interface("IUnknown")

IUnknown.methods = (
	Method(HRESULT, "QueryInterface", ((REFIID, "riid"), (Pointer(Pointer(Void)), "ppvObj"))),
	Method(ULONG, "AddRef", ()),
	Method(ULONG, "Release", ()),
)


class Dll:

    def __init__(self, name):
        self.name = name
        self.functions = []
        if self not in towrap:
            towrap.append(self)

    def wrap_name(self):
        return "Wrap" + self.name

    def wrap_pre_decl(self):
        pass

    def wrap_decl(self):
        print 'static HINSTANCE g_hDll = NULL;'
        print 'Log * g_pLog = NULL;'
        print 'static TCHAR g_szDll[MAX_PATH] = {0};'
        print
        print 'BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);'
        print

    def wrap_impl(self):
        print r'BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {'
        #print r'    Log * pLog;'
        print r'    switch(fdwReason) {'
        print r'    case DLL_PROCESS_ATTACH:'
        print r'        if(!GetSystemDirectory(g_szDll, MAX_PATH))'
        print r'            return FALSE;'
        print r'        _tcscat(g_szDll, TEXT("\\%s.dll"));' % self.name
        #print r'        if ((dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES)' 
        #print r'            return FALSE;' 
        #print r'        if(!g_pLog)'
        #print r'            g_pLog = new Log(TEXT("%s"));' % self.name
        print r'    case DLL_THREAD_ATTACH:'
        #print r'        pLog = new Log(TEXT("%s"));' % self.name
        #print r'        TlsSetValue(dwTlsIndex, pLog);'
        print r'        return TRUE;'
        print r'    case DLL_THREAD_DETACH:'
        #print r'        pLog = (Log *)TlsGetValue(dwTlsIndex);' 
        #print r'        if (pLog != NULL)'
        #print r'            delete pLog;'
        print r'        return TRUE;'
        print r'    case DLL_PROCESS_DETACH:'
        #print r'        pLog = (Log *)TlsGetValue(dwTlsIndex);' 
        #print r'        if (pLog != NULL)'
        #print r'            delete pLog;'
        #print r'        TlsFree(dwTlsIndex);' 
        print r'        if(g_pLog) {'
        print r'            delete g_pLog;'
        print r'            g_pLog = NULL;'
        print r'        }'
        print r'        if(g_hDll) {'
        print r'            FreeLibrary(g_hDll);'
        print r'            g_hDll = NULL;'
        print r'        }'
        print r'        return TRUE;'
        print r'    }'
        print r'    (void)hinstDLL;'
        print r'    (void)lpvReserved;'
        print r'    return TRUE;'
        print r'}'
        print
        for function in self.functions:
            type = 'P' + function.name
            print function.prototype() + ' {'
            if 1:
                print '    if(g_pLog)'
                print '        delete g_pLog;'
                print '    g_pLog = new Log(TEXT("%s"));' % self.name
            #print '    g_pLog->ReOpen();'
            print '    typedef ' + function.prototype('* %s' % type) + ';'
            print '    %s pFunction;' % type
            if function.type is Void:
                result = ''
            else:
                print '    %s result;' % function.type
                result = 'result = '
            print '    if(!g_hDll) {'
            print '        g_hDll = LoadLibrary(g_szDll);'
            print '        if(!g_hDll)'
            print '            ExitProcess(0);'
            print '    }'
            print '    pFunction = (%s)GetProcAddress( g_hDll, "%s");' % (type, function.name)
            print '    if(!pFunction)'
            print '        ExitProcess(0);'
            print '    g_pLog->BeginCall("%s");' % (function.name)
            print '    %spFunction(%s);' % (result, ', '.join([str(name) for type, name in function.args]))
            print '    g_pLog->EndCall();'
            for type, name in function.args:
                if type.isoutput():
                    type.wrap_instance(name)
            if function.type is not Void:
                function.type.wrap_instance('result')
            if function.type is not Void:
                print '    return result;'
            print '}'
            print
        print

