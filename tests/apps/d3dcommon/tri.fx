struct VS_OUTPUT {
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

VS_OUTPUT VS(float4 Pos : POSITION, float4 Color : COLOR) {
    VS_OUTPUT Out;
    Out.Pos = Pos;
    Out.Color = Color;
    return Out;
}

float4 PS(VS_OUTPUT In) : SV_Target {
    return In.Color;
}
