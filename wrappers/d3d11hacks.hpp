#include <set>


static std::set<ID3D11Resource *>
g_pResources;


static void
createID3D11Resource(ID3D11Resource *pResource)
{
    D3D11_RESOURCE_DIMENSION Type = D3D11_RESOURCE_DIMENSION_UNKNOWN;
    pResource->GetType(&Type);
    if (Type != D3D11_RESOURCE_DIMENSION_TEXTURE2D) {
        return;
    }

    std::pair< std::set<ID3D11Resource *>::iterator, bool > ret;
    ret = g_pResources.insert(pResource);
    if (!ret.second) {
        return;
    }

    D3D11_TEXTURE2D_DESC Desc;
    static_cast<ID3D11Texture2D *>(pResource)->GetDesc(&Desc);

    ID3D11Device *pDevice;
    pResource->GetDevice(&pDevice);

    static const char * _args[4] = {"this", "pDesc", "pInitialData", "ppTexture2D"};
    static const trace::FunctionSig _sig = {9999, "ID3D11Device1::CreateTexture2D", 4, _args};

    unsigned _call = trace::localWriter.beginEnter(&_sig);
    trace::localWriter.beginArg(0);
    trace::localWriter.writePointer((uintptr_t)pDevice);
    trace::localWriter.endArg();

    // pDesc
    trace::localWriter.beginArg(1);
        trace::localWriter.beginArray(1);
        trace::localWriter.beginElement();
        trace::localWriter.beginStruct(&_structD3D11_TEXTURE2D_DESC_sig);
        trace::localWriter.writeUInt(Desc.Width);
        trace::localWriter.writeUInt(Desc.Height);
        trace::localWriter.writeUInt(Desc.MipLevels);
        trace::localWriter.writeUInt(Desc.ArraySize);
        trace::localWriter.writeEnum(&_enumDXGI_FORMAT_sig, Desc.Format);
        trace::localWriter.beginStruct(&_structDXGI_SAMPLE_DESC_sig);
        trace::localWriter.writeUInt((Desc.SampleDesc).Count);
        trace::localWriter.writeUInt((Desc.SampleDesc).Quality);
        trace::localWriter.endStruct();
        trace::localWriter.writeEnum(&_enumD3D11_USAGE_sig, Desc.Usage);
        trace::localWriter.writeUInt(Desc.BindFlags);
        trace::localWriter.writeUInt(Desc.CPUAccessFlags);
        trace::localWriter.writeUInt(Desc.MiscFlags);
        trace::localWriter.endStruct();
        trace::localWriter.endElement();
        trace::localWriter.endArray();
    trace::localWriter.endArg();

    // pInitialData
    trace::localWriter.beginArg(2);
        trace::localWriter.writeNull();
    trace::localWriter.endArg();

    trace::localWriter.endEnter();

    trace::localWriter.beginLeave(_call);

    trace::localWriter.beginArg(3);
    trace::localWriter.beginArray(1);
    trace::localWriter.beginElement();
    trace::localWriter.writePointer((uintptr_t)pResource);
    trace::localWriter.endElement();
    trace::localWriter.endArray();
    trace::localWriter.endArg();

    trace::localWriter.beginReturn();
    trace::localWriter.writeEnum(&_enumHRESULT_sig, S_OK);
    trace::localWriter.endReturn();

    trace::localWriter.endLeave();
}
