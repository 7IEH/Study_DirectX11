#ifndef _STD2D
#define _STD2D

struct VTX_IN
{
    float3 vPos : POSITION;
    float4 vColor : COLOR;
    float2 vUv : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float4 vColor : COLOR;
    float2 vUv : TEXCOORD;
};

VS_OUT VS_std2D(VTX_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = float4(_in.vPos.xy, 0.f, 1.f);
    output.vColor = float4(_in.vColor.xyzw);
    output.vUv = _in.vUv;
    
    return output;
}

float4 PS_std2D(VS_OUT _in) : SV_Target
{
    return _in.vColor;
}

#endif