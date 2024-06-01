#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>

#include "../../../External/imgui/imgui.h"
#include "../../../External/imgui/imgui_internal.h"

class Transform
{
public:
	enum class CoordinateSystem
	{
		cRightYup,
		cLeftYup,
		cRightZup,
		cLeftZup,
		cNone,
	};

	Transform() :
		position_(DirectX::XMFLOAT3())
		, scale_(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f))
		, rotation_(DirectX::XMFLOAT4())
		, scaleFactor_(1.0f)
		, coordinateSystem_(static_cast<int>(CoordinateSystem::cRightYup))
	{}
	~Transform() {}

	// ---------- ImGui用関数 ----------
	void DrawDebug();
	void Reset();
	// ---------- ImGui用関数 ----------

public:// Setter,Getter, etc...
	[[nodiscard]] DirectX::XMMATRIX CalcWorld();								// ワールド行列算出
	[[nodiscard]] DirectX::XMMATRIX CalcWorldMatrix(const float& scaleFactor);	// 座標系を掛けたワールド行列算出
	[[nodiscard]] DirectX::XMFLOAT3 CalcForward()	const;						// 前方向ベクトル算出
	[[nodiscard]] DirectX::XMFLOAT3 CalcUp()		const;						// 上方向ベクトル算出
	[[nodiscard]] DirectX::XMFLOAT3 CalcRight()		const;						// 右方向ベクトル算出

	void SetWorld(DirectX::XMMATRIX world);

	//---------- 位置 ( Position ) ----------
	[[nodiscard]] const DirectX::XMFLOAT3& GetPosition() const { return position_; }
	[[nodiscard]] const float GetPositionX() const { return position_.x; }
	[[nodiscard]] const float GetPositionY() const { return position_.y; }
	[[nodiscard]] const float GetPositionZ() const { return position_.z; }

	void SetPosition(const DirectX::XMFLOAT3& pos) { position_ = pos; }
	void SetPosition(const float& x, const float& y, const float& z)
	{
		position_.x = x;
		position_.y = y;
		position_.z = z;
	}
	void SetPositionX(const float& posX) { position_.x = posX; }
	void SetPositionY(const float& posY) { position_.y = posY; }
	void SetPositionZ(const float& posZ) { position_.z = posZ; }

	void AddPosition(const DirectX::XMFLOAT3& pos)
	{
		position_.x += pos.x;
		position_.y += pos.y;
		position_.z += pos.z;
	}
	void AddPositionX(const float posX) { position_.x += posX; }
	void AddPositionY(const float posY) { position_.y += posY; }
	void AddPositionZ(const float posZ) { position_.z += posZ; }

	//---------- 大きさ ( Scale ) ----------
	[[nodiscard]] DirectX::XMFLOAT3 GetScale() const { return scale_; }

	void SetScale(const DirectX::XMFLOAT3& scl) { scale_ = scl; }

	// ScaleFactor
	[[nodiscard]] float GetScaleFactor() const { return scaleFactor_; }
	void SetScaleFactor(float f) { scaleFactor_ = f; }

	//---------- 回転 ( Rotation ) ----------
	[[nodiscard]] const DirectX::XMFLOAT4 GetRotation() const { return rotation_; }
	[[nodiscard]] const DirectX::XMFLOAT3 GetRotation()
	{
		return DirectX::XMFLOAT3(rotation_.x, rotation_.y, rotation_.z);
	}
	[[nodiscard]] const float GetRotationX() const { return rotation_.x; }
	[[nodiscard]] const float GetRotationY() const { return rotation_.y; }
	[[nodiscard]] const float GetRotationZ() const { return rotation_.z; }

	void SetRotation(const DirectX::XMFLOAT4& rot) { rotation_ = rot; }
	void SetRotation(const DirectX::XMFLOAT3& rot)
	{
		rotation_.x = rot.x;
		rotation_.y = rot.y;
		rotation_.z = rot.z;
	}
	void SetRotation(const float& rotX, const float& rotY, const float& rotZ)
	{
		rotation_.x = rotX;
		rotation_.y = rotY;
		rotation_.z = rotZ;
	}
	void SetRotationX(const float& rotX) { rotation_.x = rotX; }
	void SetRotationY(const float& rotY) { rotation_.y = rotY; }
	void SetRotationZ(const float& rotZ) { rotation_.z = rotZ; }

	void AddRotationX(const float& rotX) { rotation_.x += rotX; }
	void AddRotationY(const float& rotY) { rotation_.y += rotY; }
	void AddRotationZ(const float& rotZ) { rotation_.z += rotZ; }

private:
	DirectX::XMFLOAT3 position_;
	DirectX::XMFLOAT3 scale_;
	DirectX::XMFLOAT4 rotation_;

	float scaleFactor_;

	int coordinateSystem_;
	const DirectX::XMFLOAT4X4 CoordinateSystemTransforms_[static_cast<int>(CoordinateSystem::cNone)]
	{
		{
			-1, 0, 0, 0,
			 0, 1, 0, 0,
			 0, 0, 1, 0,
			 0, 0, 0, 1,
		},

		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		},

		{
			1, 0, 0, 0,
			0, 0, 1, 0,
			0, 1, 0, 0,
			0, 0, 0, 1
		},
		{
			1, 0, 0, 0,
			0, 0, 1, 0,
			0, 1, 0, 0,
			0, 0, 0, 1
		}
	};

	const std::string coordinateSystemName_[static_cast<int>(CoordinateSystem::cNone)]
	{
		"Right Hand, Y up",
		"Left Hand, Y up",
		"Right Hand, Z up",
		"Left Hand, Z up"
	};
};