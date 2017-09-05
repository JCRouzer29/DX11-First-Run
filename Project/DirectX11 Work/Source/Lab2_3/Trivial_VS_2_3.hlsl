#pragma pack_matrix(row_major)

struct INPUT_VERTEX{
	float4 color : COLOR;
	float3 coordinate : POSITION;
	float  padding : PADDING;
};

struct OUTPUT_VERTEX{
	float4 colorOut : COLOR;
	float4 projectedCoordinate : SV_POSITION;
};

cbuffer CAMERA : register(b0){
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}
cbuffer OBJECT : register(b1){
	float4x4 objWorldMatrix;
}

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer){
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;
	sendToRasterizer.colorOut = fromVertexBuffer.color;
	
	//Get Local Space Vertex from Vertex Buffer
	float4 localH = float4(fromVertexBuffer.coordinate, 1);
	
	//Move Local Space Vertex into World Space
	localH = mul(localH, objWorldMatrix);

	//Move World Space Vertex based on location of Camera
	localH = mul(localH, worldMatrix);

	//Move into View Space
	localH = mul(localH, viewMatrix);

	//Move into Projection Space
	localH = mul(localH, projectionMatrix);

	sendToRasterizer.projectedCoordinate = localH;
	//Send Projected Vertex to Rasterizer Stage
	return sendToRasterizer;
}