#include "Character.h"
#include "../Scene/GameScene.h"
#include "MathHelper.h"

// ----- コンストラクタ -----
Character::Character(const std::string& filename, const float& scaleFactor)
    : Object(filename, scaleFactor)
{
}

// ----- 更新 -----
void Character::Update(const float& elapsedTime)
{
    // アニメーション更新
    Object::Update(elapsedTime);

    // 吹っ飛び更新
    UpdateForce(elapsedTime);

    // LookAt更新
    LookAtUpdate();
}

// ----- ImGui用 -----
void Character::DrawDebug()
{
    if (ImGui::TreeNode("HP"))
    {
        ImGui::DragFloat("Health", &health_);
        if (ImGui::Button("Add")) health_ += 10.0f;
        if (ImGui::Button("Reduce")) health_ -= 10.0f;

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("LookAt"))
    {
        ImGui::Checkbox("USE", &isLookAt_);
        ImGui::DragFloat3("GlobalForward", &headGlobalForward_.x);
        ImGui::DragFloat3("TargetPosition", &lookAtTargetPosition_.x);
        ImGui::DragInt("JointIndex", &headJointIndex_);
        ImGui::TreePop();
    }


    if (ImGui::TreeNode("Move"))
    {
        ImGui::DragFloat("Acceleration", &acceleration_);
        ImGui::DragFloat("Deceleration", &deceleration_);
        ImGui::DragFloat("MaxSpeed", &maxSpeed_);
        ImGui::DragFloat3("Velocity", &velocity_.x);

        float speed = XMFloat3Length(velocity_);
        ImGui::DragFloat("Speed", &speed);

        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Rotate"))
    {
        ImGui::DragFloat("Speed", &rotateSpeed_);

        ImGui::TreePop();
    }


    ImGui::DragFloat("CollisionRadius", &collisionRadius_);

    if (ImGui::TreeNode("Collision"))
    {

        if (ImGui::TreeNode("DamageDetection"))
        {
            for (DamageDetectionData& data : damageDetectionData_)
            {
                data.DrawDebug();
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("AttackDetection"))
        {
            for (AttackDetectionData& data : attackDetectionData_)
            {
                //if (data.GetIsActive() == false) continue;
                data.DrawDebug();
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("CollisionDetection"))
        {
            for (CollisionDetectionData& data : collisionDetectionData_)
            {
                data.DrawDebug();
            }
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}

// ----- Collision更新 -----
void Character::UpdateCollisions(const float& elapsedTime)
{
    // くらい判定更新
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition()));

        data.Update(elapsedTime);
    }
    // 攻撃判定更新
    for (AttackDetectionData& data : attackDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition()));
    }
    // 押し出し判定更新
    for (CollisionDetectionData& data : collisionDetectionData_)
    {        
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition()));
    }
}

// ----- ステージ外に出ないようにする -----
void Character::CollisionCharacterVsStage()
{
    const DirectX::XMFLOAT3 characterPos = GetTransform()->GetPosition();
    const DirectX::XMFLOAT3 stagePos = GameScene::stageCenter_;
    DirectX::XMFLOAT3 vec = characterPos - stagePos;
    const float length = XMFloat3Length(vec);
    const float radius = GameScene::stageRadius_ - collisionRadius_;
    if (length > radius)
    {
        vec = XMFloat3Normalize(vec);
        GetTransform()->SetPosition(GameScene::stageCenter_ + vec * radius);
    }
}

DirectX::XMFLOAT3 Character::SetTargetPosition()
{
    // ステージの中で移動先を決める
    DirectX::XMFLOAT3 stagePos = GameScene::stageCenter_;
    int length = rand() % static_cast<int>(GameScene::stageRadius_);
    DirectX::XMFLOAT3 direction = { static_cast<float>(rand() % 21 - 10), 0, static_cast<float>(rand() % 21 - 10) };
    DirectX::XMFLOAT3 vec = stagePos + XMFloat3Normalize(direction) * length;
    return vec;
}

void Character::UpdateForce(const float& elapsedTime)
{
    // パワーが無いときは処理しない
    if (blowPower_ <= 0) return;

    blowPower_ -= decelerationForce_ * elapsedTime;
    blowPower_ = std::max(blowPower_, 0.0f); // 0.0f以下にならないように修正

    // 吹っ飛び方向にどれだけ、吹っ飛ばすかを計算する
    DirectX::XMFLOAT3 direction = {};
    direction = XMFloat3Normalize(blowDirection_) * blowPower_ * elapsedTime;

    // 吹っ飛ばす。
    GetTransform()->AddPosition(direction);
}

void Character::AddForce(const DirectX::XMFLOAT3& direction, const float& power, const float& decelerationForce)
{
    // Y方向には吹っ飛ばさない
    //blowDirection_ = { direction.x, 0, direction.z };
    blowDirection_ = direction;
    blowPower_ = power;
    decelerationForce_ = decelerationForce;
}

// ----- LookAt初期化 -----
void Character::LookAtInitilaize(const std::string& nodeName)
{
    // 頭のジョイント番号を保存
    headJointIndex_ = GetNodeIndex(nodeName);

    GltfModel::Node node = GetNodes()->at(headJointIndex_);

    // 頭のグローバルの前方向を保存
    headGlobalForward_ = { node.globalTransform_._31, node.globalTransform_._32, node.globalTransform_._33 };
}

// ----- LookAt更新 -----
void Character::LookAtUpdate()
{
    // LookAt処理をするか
    if (isLookAt_ == false) return;

    // ワールド行列
    DirectX::XMMATRIX World = GetTransform()->CalcWorldMatrix(GetScaleFactor());

    // ワールドの逆行列
    DirectX::XMMATRIX InverseWorld = DirectX::XMMatrixInverse(NULL, World);

    // 頭のノード
    GltfModel::Node& node = GetNodes()->at(headJointIndex_);

    // 頭のグローバル位置
    DirectX::XMFLOAT3 headGlobalPosition = { node.globalTransform_._41, node.globalTransform_._42, node.globalTransform_._43 };

    // ターゲットのワールド位置
    DirectX::XMFLOAT3 targetWorldPosition = lookAtTargetPosition_;
    // ターゲットのグローバル位置
    DirectX::XMFLOAT3 targetGlobalPosition = {};
    DirectX::XMStoreFloat3(&targetGlobalPosition, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&targetWorldPosition), InverseWorld));

    // 頭からターゲットへの　グローバルのベクトルを作る
    DirectX::XMFLOAT3 headToTargetGlobal = targetGlobalPosition - headGlobalPosition;

    // 頭のグローバル逆行列
    DirectX::XMMATRIX InverseGlobalTransform = DirectX::XMMatrixInverse(NULL, DirectX::XMLoadFloat4x4(&node.globalTransform_));

    // グローバルのものをローカル空間に落とし込む
    DirectX::XMVECTOR headToTargetLocal = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&headToTargetGlobal), InverseGlobalTransform);
    DirectX::XMVECTOR headLocalForward = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&headGlobalForward_), InverseGlobalTransform);

    // 回転軸を求める
    DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(headLocalForward, headToTargetLocal);

    // 回転角を求める
    float angle = DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenVectors(headLocalForward, headToTargetLocal));

    // 回転行列を求める
    DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(DirectX::XMVector3Normalize(Axis), angle);

    // 頭のノードのグローバルトランスフォームを更新する
    DirectX::XMStoreFloat4x4(&node.globalTransform_, Rotation * DirectX::XMLoadFloat4x4(&node.globalTransform_));

    // 頭の子ノードを再帰的に更新する
    std::function<void(int, int)> UpdateGlobalTransforms = [&](int parentIndex, int nodeIndex)
    {
        GltfModel::Node& node = GetNodes()->at(nodeIndex);
        if (parentIndex > -1)
        {
            DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale_.x, node.scale_.y, node.scale_.z);
            DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotation_.x, node.rotation_.y, node.rotation_.z, node.rotation_.w));
            DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translation_.x, node.translation_.y, node.translation_.z);
            DirectX::XMStoreFloat4x4(&node.globalTransform_, S * R * T * DirectX::XMLoadFloat4x4(&GetNodes()->at(parentIndex).globalTransform_));
        }
        for (int childIndex : node.children_)
        {
            UpdateGlobalTransforms(nodeIndex, childIndex);
        }
    };
    UpdateGlobalTransforms(-1, headJointIndex_);
}

// ---------- くらい判定 ----------
#pragma region くらい判定
// ----- 登録 -----
void Character::RegisterDamageDetectionData(const DamageDetectionData& data)
{
    damageDetectionData_.emplace_back(data);
}

// ----- データ取得 ( 名前検索 ) -----
DamageDetectionData& Character::GetDamageDetectionData(const std::string& name)
{
    // 名前でデータを探す
    for (DamageDetectionData& data : damageDetectionData_)
    {
        if (data.GetName() != name) continue;

        return data;
    }

    // 見つからなかった
    return DamageDetectionData();
}

// ----- データ取得 ( 登録番号 ) -----
DamageDetectionData& Character::GetDamageDetectionData(const int& index)
{
    return damageDetectionData_.at(index);
}


#pragma endregion くらい判定

// ---------- 攻撃判定 ----------
#pragma region 攻撃判定
// ----- 登録 -----
void Character::RegisterAttackDetectionData(const AttackDetectionData& data)
{
    attackDetectionData_.emplace_back(data);
}

// ----- データ取得 ( 名前検索 ) -----
AttackDetectionData& Character::GetAttackDetectionData(const std::string& name)
{
    // 名前でデータを探す
    for (AttackDetectionData& data : attackDetectionData_)
    {
        if (data.GetName() != name) continue;

        return data;
    }

    // 見つからなかった
    return AttackDetectionData();
}

// ----- データ取得 ( 登録番号 ) -----
AttackDetectionData& Character::GetAttackDetectionData(const int& index)
{
    return attackDetectionData_.at(index);
}
#pragma endregion 攻撃判定

// ---------- 押し出し判定 ----------
#pragma region 押し出し判定
void Character::RegisterCollisionDetectionData(const CollisionDetectionData& data)
{
    collisionDetectionData_.emplace_back(data);
}

// ----- データ取得 ( 名前検索 ) -----
CollisionDetectionData& Character::GetCollisionDetectionData(const std::string& name)
{
    // 名前でデータを探す
    for (CollisionDetectionData& data : collisionDetectionData_)
    {
        if (data.GetName() != name) continue;

        return data;
    }

    // 見つからなかった
    return CollisionDetectionData();
}

// ----- データ取得 ( 登録番号 ) -----
CollisionDetectionData& Character::GetCollisionDetectionData(const int& index)
{
    return collisionDetectionData_.at(index);
}
#pragma endregion 押し出し判定