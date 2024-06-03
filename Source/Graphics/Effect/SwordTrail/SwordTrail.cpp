#include "SwordTrail.h"

// ----- �X�V -----
void SwordTrail::Update(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition)
{
	primitiveRenderer_.ClearVertices();

	// �ۑ����Ă������_�o�b�t�@���P�t���[�������炷
	for (int x = MAX_POLYGON - 1; x > -1; --x)
	{
		const int previousIndex = x - 1;
		if (previousIndex == -1)
		{
			trailPositions[0][0] = { 0,0,0 };
			trailPositions[1][0] = { 0,0,0 };
			texcoord_[0][0] = { 0,0 };
			texcoord_[1][0] = { 0,0 };
		}
		else
		{
			float factor = 1.0f / static_cast<float>(MAX_POLYGON);
			//float factor = 1.0f / static_cast<float>(MAX_POLYGON);
			trailPositions[0][x] = trailPositions[0][previousIndex];
			trailPositions[1][x] = trailPositions[1][previousIndex];
			texcoord_[0][x].y = 1.0f;
			texcoord_[1][x].y = 0.0f;
			texcoord_[0][x].x = texcoord_[0][previousIndex].x + factor;
			texcoord_[1][x].x = texcoord_[1][previousIndex].x + factor;

			texcoord_[0][x].x = min(texcoord_[0][x].x, 1.0f);
			texcoord_[1][x].x = min(texcoord_[1][x].x, 1.0f);
		}
	}

	// ���݂̈ʒu��ۑ�����
	trailPositions[0][0] = startPosition;
	trailPositions[1][0] = endPosition;
	texcoord_[0][0] = { 0,1 };
	texcoord_[1][0] = { 0,0 };

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

	// �ۑ����Ă������_�o�b�t�@�Ń|���S�������
	if (splineCurve_)
	{
#if 0
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
#endif
	}
	else
	{
		for (int i = 0; i < MAX_POLYGON; ++i)
		{
			primitiveRenderer_.AddVertex(trailPositions[0][i], color, texcoord_[0][i]);
			primitiveRenderer_.AddVertex(trailPositions[1][i], color2, texcoord_[1][i]);
		}
	}

	//primitiveRenderer_.SetTexcoord();
}

// ----- �`�� -----
void SwordTrail::Render()
{
	primitiveRenderer_.Render();
}

void SwordTrail::DrawDebug()
{
	ImGui::Checkbox("splineCurve", &splineCurve_);
}
