
texture2D bTexture : register(t0);
SamplerState filter[1] : register(s0);

float4 main(float2 texCoord : TEXCOORD, float4 colorFromRasterizer : COLOR) : SV_TARGET {
	float4 _return = bTexture.Sample(filter[0], texCoord);
	return _return;

//	return colorFromRasterizer;
}