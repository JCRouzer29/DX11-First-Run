
TextureCube SkyboxTexture : register(t0);
SamplerState filter[1] : register(s0);

float4 main(float3 texCoord : TEXCOORD, float4 colorFromRasterizer : COLOR) : SV_TARGET {
	return SkyboxTexture.Sample(filter[0], texCoord);
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
}