void
VS(float4 Pos : POSITION,
   float4 Color : COLOR,
   out float4 oPos : POSITION,
   out float4 oColor : COLOR)
{
    oPos = Pos;
    oColor = Color;
}

void
PS(float4 Color : COLOR,
   out float4 oColor : COLOR)
{
    oColor = Color;
}
