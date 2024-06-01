#include "SwordTrail.h"

// ----- 更新 -----
void SwordTrail::Update(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition)
{
	primitiveRenderer_.ClearVertices();

	// 保存していた頂点バッファを１フレーム分ずらす
	for (int x = MAX_POLYGON - 1; x > -1; --x)
	{
		const int previousIndex = x - 1;
		if (previousIndex == -1)
		{
			trailPositions[0][0] = { 0,0,0 };
			trailPositions[1][0] = { 0,0,0 };
		}
		else
		{
			trailPositions[0][x] = trailPositions[0][previousIndex];
			trailPositions[1][x] = trailPositions[1][previousIndex];
		}
	}

	// 現在の位置を保存する
	trailPositions[0][0] = startPosition;
	trailPositions[1][0] = endPosition;

	DirectX::XMFLOAT4 color = { 1, 0, 0, 1 };
	DirectX::XMFLOAT4 color2 = { 0, 0, 1, 1 };

	const float startTexcoord = 1;
	const float endTexcoord = 0;

#if 0
	static const int tMaxNum = 3;
	const float t[tMaxNum] = { 0.25f, 0.5f, 0.75f };
#else
	static const int tMaxNum = 7;
	const float t[tMaxNum] = { 0.125f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f };
#endif

	// 保存していた頂点バッファでポリゴンを作る
	if (splineCurve_)
	{
		primitiveRenderer_.AddVertex(trailPositions[0][0], color, startTexcoord);
		primitiveRenderer_.AddVertex(trailPositions[1][0], color2, endTexcoord);

#if 0
		for (int partitionIndex = 0; partitionIndex < tMaxNum; ++partitionIndex)
		{
			DirectX::XMFLOAT3 pos0 = {};
			DirectX::XMStoreFloat3(&pos0, DirectX::XMVectorCatmullRom(
				DirectX::XMLoadFloat3(&trailPositions[0][partitionIndex]),
				DirectX::XMLoadFloat3(&trailPositions[0][partitionIndex]),
				DirectX::XMLoadFloat3(&trailPositions[0][partitionIndex + 1]),
				DirectX::XMLoadFloat3(&trailPositions[0][partitionIndex + 2]),
				t[partitionIndex]));

			DirectX::XMFLOAT3 pos1 = {};
			DirectX::XMStoreFloat3(&pos1, DirectX::XMVectorCatmullRom(
				DirectX::XMLoadFloat3(&trailPositions[1][partitionIndex]),
				DirectX::XMLoadFloat3(&trailPositions[1][partitionIndex]),
				DirectX::XMLoadFloat3(&trailPositions[1][partitionIndex + 1]),
				DirectX::XMLoadFloat3(&trailPositions[1][partitionIndex + 2]),
				t[partitionIndex]));

			primitiveRenderer_.AddVertex(pos0, color);
			primitiveRenderer_.AddVertex(pos1, color);
		}
#endif


		for (int i = 0; i < MAX_POLYGON - 3; ++i)
		{
			primitiveRenderer_.AddVertex(trailPositions[0][i + 1], color, startTexcoord);
			primitiveRenderer_.AddVertex(trailPositions[1][i + 1], color2, endTexcoord);

			for (int partitionIndex = 0; partitionIndex < tMaxNum; ++partitionIndex)
			{
                DirectX::XMFLOAT3 pos0 = {};
                DirectX::XMStoreFloat3(&pos0, DirectX::XMVectorCatmullRom(
                    DirectX::XMLoadFloat3(&trailPositions[0][i]),
                    DirectX::XMLoadFloat3(&trailPositions[0][i + 1]),
                    DirectX::XMLoadFloat3(&trailPositions[0][i + 2]),
                    DirectX::XMLoadFloat3(&trailPositions[0][i + 3]),
                    t[partitionIndex]));

                DirectX::XMFLOAT3 pos1 = {};
                DirectX::XMStoreFloat3(&pos1, DirectX::XMVectorCatmullRom(
                    DirectX::XMLoadFloat3(&trailPositions[1][i]),
                    DirectX::XMLoadFloat3(&trailPositions[1][i + 1]),
                    DirectX::XMLoadFloat3(&trailPositions[1][i + 2]),
                    DirectX::XMLoadFloat3(&trailPositions[1][i + 3]),
                    t[partitionIndex]));

                primitiveRenderer_.AddVertex(pos0, color, startTexcoord);
                primitiveRenderer_.AddVertex(pos1, color2, endTexcoord);
			}
		}

		primitiveRenderer_.AddVertex(trailPositions[0][MAX_POLYGON - 2], color, startTexcoord);
		primitiveRenderer_.AddVertex(trailPositions[1][MAX_POLYGON - 2], color2, endTexcoord);
		primitiveRenderer_.AddVertex(trailPositions[0][MAX_POLYGON - 1], color, startTexcoord);
		primitiveRenderer_.AddVertex(trailPositions[1][MAX_POLYGON - 1], color2, endTexcoord);
	}
	else
	{
		for (int i = 0; i < MAX_POLYGON; ++i)
		{
			primitiveRenderer_.AddVertex(trailPositions[0][i], color, startTexcoord);
			primitiveRenderer_.AddVertex(trailPositions[1][i], color2, endTexcoord);
		}
	}

	primitiveRenderer_.SetTexcoord();
}

// ----- 描画 -----
void SwordTrail::Render()
{
	primitiveRenderer_.Render();
}

void SwordTrail::DrawDebug()
{
	ImGui::Checkbox("splineCurve", &splineCurve_);
}
