#include "ComputeParticleEmitter.h"
#include "MathHelper.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "Application.h"

void ComputeParticleEmitter::EmitParameter::DrawDebug()
{
	if (ImGui::TreeNodeEx("Emit Parameter", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragInt("Emit Num", &emitNum_);

		const DirectX::XMUINT2 spritCount = ComputeParticleSystem::Instance().GetTextureSplitCount();
		ImGui::SliderInt("Texture Type", &textureType_, 0, spritCount.x * spritCount.y - 1);

		ImGui::DragFloat("Lifespan", &lifespan_, 0.01f, 0.0f);
		ImGui::DragFloat("Lifespan Amplitude", &lifespanAmplitude_, 0.01f, 0.0f);

		ImGui::ColorEdit4("Color", &color_.x, ImGuiColorEditFlags_PickerHueWheel);
		ImGui::DragFloat4("Color Amplitud", &colorAmplitud_.x, 0.001f, 0.0f);

		ImGui::DragFloat("Brightness", &intensity_, 0.01f);

		if (ImGui::TreeNodeEx("----- Position -----", ImGuiTreeNodeFlags_Framed))
		{
			ImGui::DragFloat3("Amplitude", &positionAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat3("Velocity Init", &velocity_.x, 0.1f);
			ImGui::DragFloat3("Velocity Amplitude", &velocityAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat3("Accel", &acceleration_.x, 0.1f);
			ImGui::DragFloat3("Accel Amplitude", &accelerationAmplitud_.x, 0.1f, 0.0f);

			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("----- Scale -----", ImGuiTreeNodeFlags_Framed))
		{
			ImGui::DragFloat3("Init", &scaleInit_.x, 0.01f, 0.0f);
			ImGui::DragFloat2("Amplitude", &scaleAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat2("Velocity Init", &scaleVelocity_.x, 0.1f);
			ImGui::DragFloat2("Velocity Amplitude", &scaleVelocityAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat2("Accel", &scaleAcceleration_.x, 0.1f, 0.0f);
			ImGui::DragFloat2("Accel Amplitude", &scaleAccelerationAmplitud_.x, 0.1f, 0.0f);

			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("----- Rotation -----", ImGuiTreeNodeFlags_Framed))
		{
			ImGui::DragFloat3("Amplitude", &rotationAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat3("Velocity Init", &rotationVelocity_.x, 0.1f);
			ImGui::DragFloat3("Velocity Amplitude", &rotationVelocityAmplitude_.x, 0.1f, 0.0f);
			ImGui::DragFloat3("Accel", &rotationAcceleration_.x, 0.1f, 0.0f);
			ImGui::DragFloat3("Accel Amplitude", &rotationAccelerationAmplitud_.x, 0.1f, 0.0f);

			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("----- Sphere -----", ImGuiTreeNodeFlags_Framed))
		{
			int activeFlag = sphere_.x;
			ImGui::DragInt("IsActive", &activeFlag, 1, 0, 1);
			sphere_.x = activeFlag;

			ImGui::DragFloat("Radius", &sphere_.y);
			ImGui::DragFloat("Speed", &sphere_.z);
			ImGui::DragFloat("Acceleration", &sphere_.w);

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

void ComputeParticleEmitter::EmitParticle(const EmitParameter& param)
{
	const DirectX::XMMATRIX worldMatrix = transform_.CalcWorldMatrix(0.01f);

	//各要素の振れ幅を算出
	const DirectX::XMFLOAT3 posAmp = param.positionAmplitude_ / 2.0f;
	const DirectX::XMFLOAT3 veloAmp = param.velocityAmplitude_ / 2.0f;
	const DirectX::XMFLOAT3 accelAmp = param.accelerationAmplitud_ / 2.0f;

	const DirectX::XMFLOAT2 scaleAmp = param.scaleAmplitude_ / 2.0f;
	const DirectX::XMFLOAT2 scaleVeloAmp = param.scaleVelocityAmplitude_ / 2.0f;
	const DirectX::XMFLOAT2 scaleAccelAmp = param.scaleAccelerationAmplitud_ / 2.0f;

	const DirectX::XMFLOAT3 rotAmp = param.rotationAmplitude_ / 2.0f;
	const DirectX::XMFLOAT3 rotVeloAmp = param.rotationVelocityAmplitude_ / 2.0f;
	const DirectX::XMFLOAT3 rotAccelAmp = param.rotationAccelerationAmplitud_ / 2.0f;

	const float lifespanAmp = param.lifespanAmplitude_ / 2.0f;

	const DirectX::XMFLOAT4 colorAmp = param.colorAmplitud_ / 2.0f;

	for (int i = 0; i < param.emitNum_; i++)
	{
		//	発生位置
		DirectX::XMFLOAT3 p = transform_.GetPosition();
		p.x = p.x + XMFloatRandomRange(-posAmp.x, posAmp.x);
		p.y = p.y + XMFloatRandomRange(-posAmp.y, posAmp.y);
		p.z = p.z + XMFloatRandomRange(-posAmp.z, posAmp.z);

		//	発生方向
		DirectX::XMFLOAT3 v = param.velocity_;
		v.x += XMFloatRandomRange(-veloAmp.x, veloAmp.x);
		v.y += XMFloatRandomRange(-veloAmp.y, veloAmp.y);
		v.z += XMFloatRandomRange(-veloAmp.z, veloAmp.z);
		//	加速度
		DirectX::XMFLOAT3 a = param.acceleration_;
		a.x += XMFloatRandomRange(-accelAmp.x, accelAmp.x);
		a.y += XMFloatRandomRange(-accelAmp.y, accelAmp.y);
		a.z += XMFloatRandomRange(-accelAmp.z, accelAmp.z);

		//　スケール
		DirectX::XMFLOAT3 s = transform_.GetScale() * param.scaleInit_;
		s = s * transform_.GetScaleFactor();
		s.x += XMFloatRandomRange(-scaleAmp.x, scaleAmp.x);
		s.y += XMFloatRandomRange(-scaleAmp.y, scaleAmp.y);
		//　スケール速度
		DirectX::XMFLOAT2 sv = param.scaleVelocity_;
		sv.x += XMFloatRandomRange(-scaleVeloAmp.x, scaleVeloAmp.x);
		sv.y += XMFloatRandomRange(-scaleVeloAmp.y, scaleVeloAmp.y);
		//　スケール加速度
		DirectX::XMFLOAT2 sa = param.scaleAcceleration_;
		sa.x += XMFloatRandomRange(-scaleAccelAmp.x, scaleAccelAmp.x);
		sa.y += XMFloatRandomRange(-scaleAccelAmp.y, scaleAccelAmp.y);

		//　回転
		DirectX::XMFLOAT3 r = {};
		r.x += XMFloatRandomRange(-rotAmp.x, rotAmp.x);
		r.y += XMFloatRandomRange(-rotAmp.y, rotAmp.y);
		r.z += XMFloatRandomRange(-rotAmp.z, rotAmp.z);
		//　回転速度
		DirectX::XMFLOAT3 rv = param.rotationVelocity_;
		rv.x += XMFloatRandomRange(-rotVeloAmp.x, rotVeloAmp.x);
		rv.y += XMFloatRandomRange(-rotVeloAmp.y, rotVeloAmp.y);
		rv.z += XMFloatRandomRange(-rotVeloAmp.z, rotVeloAmp.z);
		//　回転加速度
		DirectX::XMFLOAT3 ra = param.rotationAcceleration_;
		ra.x += XMFloatRandomRange(-rotAccelAmp.x, rotAccelAmp.x);
		ra.y += XMFloatRandomRange(-rotAccelAmp.y, rotAccelAmp.y);
		ra.y += XMFloatRandomRange(-rotAccelAmp.z, rotAccelAmp.z);

		ComputeParticleSystem::EmitParticleData data;
		//更新タイプ
		data.parameter_.x = param.textureType_;
		data.parameter_.y = param.lifespan_ + XMFloatRandomRange(-lifespanAmp, lifespanAmp);

		//発生位置
		data.position_.x = p.x;
		data.position_.y = p.y;
		data.position_.z = p.z;

		//	発生方向
		data.velocity_.x = v.x;
		data.velocity_.y = v.y;
		data.velocity_.z = v.z;

		//	加速力
		data.acceleration_.x = a.x;
		data.acceleration_.y = a.y;
		data.acceleration_.z = a.z;

		//　スケール
		data.scale_.x = s.x;
		data.scale_.y = s.y;
		data.scale_.z = 0.0f;

		//　スケール速度
		data.scaleVelocity_.x = sv.x;
		data.scaleVelocity_.y = sv.y;
		data.scaleVelocity_.z = 0;

		//　スケール加速度
		data.scaleAcceleration_.x = sv.x;
		data.scaleAcceleration_.y = sv.y;
		data.scaleAcceleration_.z = 0;

		//　回転
		data.rotation_.x = r.x;
		data.rotation_.y = r.y;
		data.rotation_.z = r.z;

		//　回転速度
		data.rotationVelocity_.x = rv.x;
		data.rotationVelocity_.y = rv.y;
		data.rotationVelocity_.z = rv.z;

		//　回転加速度
		data.rotationAcceleration_.x = ra.x;
		data.rotationAcceleration_.y = ra.y;
		data.rotationAcceleration_.z = ra.z;

		//　色
		data.color_.x = param.color_.x + XMFloatRandomRange(-colorAmp.x, colorAmp.x);
		data.color_.y = param.color_.y + XMFloatRandomRange(-colorAmp.y, colorAmp.y);
		data.color_.z = param.color_.z + XMFloatRandomRange(-colorAmp.z, colorAmp.z);
		data.color_.w = param.color_.w + XMFloatRandomRange(-colorAmp.w, colorAmp.w);

		data.color_ = data.color_ * param.intensity_;

		data.sphere_ = param.sphere_;

		ComputeParticleSystem::Instance().Emit(data);
	}
}

void ComputeParticleEmitter::EmitParticle(const std::string& filename)
{
	emitParameter_ = GetJsonEmitParameter(filename);

	EmitParticle(emitParameter_);
}

void ComputeParticleEmitter::EmitParticle()
{
	EmitParticle(emitParameter_);
}

void ComputeParticleEmitter::DrawDebug()
{
	if (ImGui::Begin("ComputeParticleEmitter"))
	{
		if (ImGui::Button("Emit Particle")) EmitParticle(emitParameter_);
		emitParameter_.DrawDebug();

		if (ImGui::TreeNodeEx("========== Reset Emit Parameter ==========", ImGuiTreeNodeFlags_Framed))
		{
			if (ImGui::Button("Reset")) emitParameter_ = {};

			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("========== Load Emit Parameter ==========", ImGuiTreeNodeFlags_Framed))
		{
			static std::string name;

			static char filename[128] = "";
			ImGui::InputText("Asset Name", filename, ARRAYSIZE(filename));
			if (ImGui::Button("Set DebugParameter From SaveData")) emitParameter_ = GetJsonEmitParameter(filename);

			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("========== Save Emit Parameter ==========", ImGuiTreeNodeFlags_Framed))
		{
			static std::string name;

			static char filename[128] = "";
			ImGui::InputText("Asset Name", filename, ARRAYSIZE(filename));
			if (ImGui::Button("Save"))
			{
				// アセット化
				AssetCreation(emitParameter_, filename);

				// 文字列リセット
				memset(filename, 0, sizeof(filename));
			}

			ImGui::TreePop();
		}

		transform_.DrawDebug();

		ImGui::End();
	}
}

void ComputeParticleEmitter::SetEmitParameter(const std::string& filename)
{
	emitParameter_ = GetJsonEmitParameter(filename);
}

ComputeParticleEmitter::EmitParameter ComputeParticleEmitter::GetJsonEmitParameter(const std::string& filename)
{
	std::string filepath = "./Resources/JsonParameters/ParticleEmitParameters/" + filename;
	std::ifstream ifs(filepath);
	nlohmann::json mJson;
	if (ifs.good())
	{
		//ファイル読み込み
		ifs >> mJson;
	}
	else
	{
		_ASSERT_EXPR(false, L"指定のパーティクルアセットが見つかりません");
	}

	//読み込み
	EmitParameter emitParam;
	emitParam.emitNum_ = mJson["emitNum"];
	emitParam.lifespan_ = mJson["lifespan"];
	emitParam.lifespanAmplitude_ = mJson["lifespanAmplitude"];
	emitParam.emitTime_ = mJson["emitTime"];
	emitParam.textureType_ = mJson["texType"];

	emitParam.positionAmplitude_ = { mJson["Position"]["amplitude"][0],mJson["Position"]["amplitude"][1],mJson["Position"]["amplitude"][2] };
	emitParam.velocity_ = { mJson["Position"]["velocity"][0],mJson["Position"]["velocity"][1],mJson["Position"]["velocity"][2] };
	emitParam.velocityAmplitude_ = { mJson["Position"]["vellocityAmplitude"][0],mJson["Position"]["vellocityAmplitude"][1],mJson["Position"]["vellocityAmplitude"][2] };
	emitParam.acceleration_ = { mJson["Position"]["acceleration"][0],mJson["Position"]["acceleration"][1],mJson["Position"]["acceleration"][2] };
	emitParam.accelerationAmplitud_ = { mJson["Position"]["accelerationAmplitude"][0],mJson["Position"]["accelerationAmplitude"][1],mJson["Position"]["accelerationAmplitude"][2] };

	emitParam.scaleInit_ = { mJson["Scale"]["init"][0],mJson["Scale"]["init"][1],mJson["Scale"]["init"][2] };
	emitParam.scaleAmplitude_ = { mJson["Scale"]["amplitude"][0],mJson["Scale"]["amplitude"][1] };
	emitParam.scaleVelocity_ = { mJson["Scale"]["velocity"][0],mJson["Scale"]["velocity"][1] };
	emitParam.scaleVelocityAmplitude_ = { mJson["Scale"]["vellocityAmplitude"][0],mJson["Scale"]["vellocityAmplitude"][1] };
	emitParam.scaleAcceleration_ = { mJson["Scale"]["acceleration"][0],mJson["Scale"]["acceleration"][1] };
	emitParam.scaleAccelerationAmplitud_ = { mJson["Scale"]["accelerationAmplitude"][0],mJson["Scale"]["accelerationAmplitude"][1] };

	emitParam.rotationAmplitude_ = { mJson["Rotation"]["amplitude"][0],mJson["Rotation"]["amplitude"][1],mJson["Rotation"]["amplitude"][2] };
	emitParam.rotationVelocity_ = { mJson["Rotation"]["velocity"][0],mJson["Rotation"]["velocity"][1],mJson["Rotation"]["velocity"][2] };
	emitParam.rotationVelocityAmplitude_ = { mJson["Rotation"]["vellocityAmplitude"][0],mJson["Rotation"]["vellocityAmplitude"][1],mJson["Rotation"]["vellocityAmplitude"][2] };
	emitParam.rotationAcceleration_ = { mJson["Rotation"]["acceleration"][0],mJson["Rotation"]["acceleration"][1],mJson["Rotation"]["acceleration"][2] };
	emitParam.rotationAccelerationAmplitud_ = { mJson["Rotation"]["accelerationAmplitude"][0],mJson["Rotation"]["accelerationAmplitude"][1],mJson["Rotation"]["accelerationAmplitude"][2] };

	emitParam.intensity_ = mJson["brightness"];
	emitParam.color_ = { mJson["color"][0],mJson["color"][1],mJson["color"][2],mJson["color"][3] };
	emitParam.colorAmplitud_ = { mJson["colorAmplitude"][0],mJson["colorAmplitude"][1],mJson["colorAmplitude"][2],mJson["colorAmplitude"][3] };
	
	emitParam.sphere_ = { mJson["sphere"][0],mJson["sphere"][1],mJson["sphere"][2],mJson["sphere"][3] };

	return emitParam;
}

void ComputeParticleEmitter::AssetCreation(const EmitParameter& param, const std::string& filename)
{
	// Jsonファイル作成
	nlohmann::json mJson;

	mJson["emitNum"] = param.emitNum_;
	mJson["lifespan"] = param.lifespan_;
	mJson["lifespanAmplitude"] = param.lifespanAmplitude_;
	mJson["emitTime"] = param.emitTime_;
	mJson["texType"] = param.textureType_;
	mJson["Position"] =
	{
		{"amplitude",			 {param.positionAmplitude_.x,param.positionAmplitude_.y,param.positionAmplitude_.z}},
		{"velocity",			 {param.velocity_.x,param.velocity_.y,param.velocity_.z}},
		{"vellocityAmplitude",  {param.velocityAmplitude_.x,param.velocityAmplitude_.y,param.velocityAmplitude_.z}},
		{"acceleration",		 {param.acceleration_.x,param.acceleration_.y,param.acceleration_.z}},
		{"accelerationAmplitude",{param.accelerationAmplitud_.x,param.accelerationAmplitud_.y,param.accelerationAmplitud_.z}}
	};
	mJson["Scale"] =
	{
		{"init",				{param.scaleInit_.x,param.scaleInit_.y,param.scaleInit_.z}},
		{"amplitude",			 {param.scaleAmplitude_.x,param.scaleAmplitude_.y}},
		{"velocity",			 {param.scaleVelocity_.x,param.scaleVelocity_.y}},
		{"vellocityAmplitude",  {param.scaleVelocityAmplitude_.x,param.scaleVelocityAmplitude_.y}},
		{"acceleration",		 {param.scaleAcceleration_.x,param.scaleAcceleration_.y}},
		{"accelerationAmplitude",{param.scaleAccelerationAmplitud_.x,param.scaleAccelerationAmplitud_.y}}
	};
	mJson["Rotation"] =
	{
		{"amplitude",			 {param.rotationAmplitude_.x,param.rotationAmplitude_.y,param.rotationAmplitude_.z}},
		{"velocity",			 {param.rotationVelocity_.x,param.rotationVelocity_.y,param.rotationVelocity_.z}},
		{"vellocityAmplitude",  {param.rotationVelocityAmplitude_.x,param.rotationVelocityAmplitude_.y,param.rotationVelocityAmplitude_.z}},
		{"acceleration",		 {param.rotationAcceleration_.x,param.rotationAcceleration_.y,param.rotationAcceleration_.z}},
		{"accelerationAmplitude",{param.rotationAccelerationAmplitud_.x,param.rotationAccelerationAmplitud_.y,param.rotationAccelerationAmplitud_.z}}
	};
	mJson["brightness"] = param.intensity_;
	mJson["color"] = { param.color_.x,param.color_.y,param.color_.z,param.color_.w };
	mJson["colorAmplitude"] = { param.colorAmplitud_.x,param.colorAmplitud_.y,param.colorAmplitud_.z,param.colorAmplitud_.w };
	mJson["sphere"] = { param.sphere_.x,param.sphere_.y,param.sphere_.z,param.sphere_.w };

	std::ofstream writingFile;
	std::string filepath = "./Resources/JsonParameters/ParticleEmitParameters/" + filename;
	writingFile.open(filepath, std::ios::out);
	writingFile << mJson.dump() << std::endl;
	writingFile.close();
}