#include "Character.h"
#include "../Scene/GameScene.h"
#include "../Other/MathHelper.h"

// ----- コンストラクタ -----
Character::Character(std::string filename)
    : Object(filename)
{
}

void Character::Update(const float& elapsedTime)
{
    // アニメーション更新
    Object::Update(elapsedTime);

    // 吹っ飛び更新
    UpdateForce(elapsedTime);
}

// ----- ImGui用 -----
void Character::DrawDebug()
{
    ImGui::DragFloat3("LookAtTarget", &lookAtTargetPosition_.x);

    if (ImGui::TreeNode("Move"))
    {
        ImGui::DragFloat("Acceleration", &acceleration_);
        ImGui::DragFloat("Deceleration", &deceleration_);
        ImGui::DragFloat("MaxSpeed", &maxSpeed_);
        ImGui::DragFloat3("Velocity", &velocity_.x);

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
void Character::UpdateCollisions(const float& elapsedTime, const float& scaleFactor)
{
    // くらい判定更新
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));

        data.Update(elapsedTime);
    }
    // 攻撃判定更新
    for (AttackDetectionData& data : attackDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));
    }
    // 押し出し判定更新
    for (CollisionDetectionData& data : collisionDetectionData_)
    {        
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));
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

    blowPower_ -= elapsedTime * 2.0f;
    blowPower_ = std::max(blowPower_, 0.0f); // 0.0f以下にならないように修正

    // 吹っ飛び方向にどれだけ、吹っ飛ばすかを計算する
    DirectX::XMFLOAT3 direction = {};
    direction = XMFloat3Normalize(blowDirection_) * blowPower_;

    // 吹っ飛ばす。
    GetTransform()->AddPosition(direction);
}

void Character::AddForce(const DirectX::XMFLOAT3& direction, const float& power)
{
    // Y方向には吹っ飛ばさない
    blowDirection_ = { direction.x, 0, direction.z };
    blowPower_ = power;
}

void Character::LookAtInitilaize(const std::string& nodeName)
{
    headNodeName_ = nodeName;
    DirectX::XMMATRIX World = GetJointWorldTransform(headNodeName_, 0.01f);
    DirectX::XMMATRIX InverseWorld = DirectX::XMMatrixInverse(nullptr, World);
    DirectX::XMFLOAT4X4 inverseWorld = {};
    DirectX::XMStoreFloat4x4(&inverseWorld, InverseWorld);
    const DirectX::XMVECTOR HeadLocalForward = DirectX::XMVectorSet(
        inverseWorld._31, inverseWorld._32, inverseWorld._33, 0.0f);
    DirectX::XMStoreFloat3(&headLocalForward_, DirectX::XMVector3Normalize(HeadLocalForward));

}

void Character::LookAtUpdate()
{
#if 1
    std::function<void(int, DirectX::XMVECTOR)> UpdateWorldTransforms = [&](int nodeIndex, DirectX::XMVECTOR HeadPosition)
    {
        GltfModel::Node& node = GetNodes()->at(nodeIndex);
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale_.x, node.scale_.y, node.scale_.z);
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation_));
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translation_.x, node.translation_.y, node.translation_.z);
        //DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(HeadPosition);
        DirectX::XMStoreFloat4x4(&node.globalTransform_, S * R * T * DirectX::XMLoadFloat4x4(&node.parentGlobalTransform_));
        //DirectX::XMStoreFloat4x4(&node.globalTransform_, S * R * T);// *DirectX::XMLoadFloat4x4(&parentGlobalTransforms));
        
    
        //for (int childIndex : node.children_)
        //{
        //    DirectX::XMMATRIX Matrix = GetJointGlobalTransform(childIndex, 0.01f);
        //    DirectX::XMVECTOR Position = Matrix.r[3];

        //    UpdateWorldTransforms(childIndex, Position);
        //}

    };
#else
    std::function<void(int)> UpdateWorldTransforms = [&](int nodeIndex)
    {
        GltfModel::Node& node = GetNodes()->at(nodeIndex);
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale_.x, node.scale_.y, node.scale_.z);
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation_));
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translation_.x, node.translation_.y, node.translation_.z);
        DirectX::XMStoreFloat4x4(&node.globalTransform_, S * R * T * DirectX::XMLoadFloat4x4(&node.parentGlobalTransform_));

        for (int childIndex : node.children_)
        {
            UpdateWorldTransforms(childIndex);
        }

    };
#endif

#if 0
    // 頭のローカル前方向ベクトル
    const DirectX::XMVECTOR HeadLocalForward = DirectX::XMLoadFloat3(&headLocalForward_);
    // 頭のワールド座標位置取得
    const DirectX::XMVECTOR HeadWorldPosition = DirectX::XMLoadFloat3(&GetJointPosition(headNodeName_, 0.01f));
    // ターゲットのワールド位置
    const DirectX::XMVECTOR TargetWorldPosition = DirectX::XMLoadFloat3(&lookAtTargetPosition_);
    // 頭のワールド逆行列
    DirectX::XMMATRIX InverseWorld = DirectX::XMMatrixInverse(nullptr, GetJointWorldTransform(headNodeName_, 0.01f));
    // ワールド空間のターゲット方向ベクトルを計算し、ローカル空間に変換
    DirectX::XMVECTOR TargetLocalDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(TargetWorldPosition, HeadWorldPosition));
    TargetLocalDirection = DirectX::XMVector3TransformNormal(TargetLocalDirection, InverseWorld);
    // 回転軸計算
    const DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(HeadLocalForward, TargetLocalDirection));
    // 回転角計算
    const float angle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(HeadLocalForward, TargetLocalDirection)));
    // 回転角が微小な場合は回転しない
    if (fabsf(angle) > 1e-8f)
    {
        // 回転軸と回転角から回転クォータニオンを求める
        const DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationAxis(Axis, angle);

        // 頭のローカル回転値に回転クォータニオンを合成する
        const int nodeIndex = GetNodeIndex(headNodeName_);
        DirectX::XMFLOAT4 headRotate = GetNodes()->at(nodeIndex).rotation_;
        DirectX::XMVECTOR Rotation = DirectX::XMLoadFloat4(&headRotate);
        Rotation = DirectX::XMQuaternionMultiply(Rotation, Q);
        DirectX::XMStoreFloat4(&headRotate, Rotation);

        // 頭ノードとその子ノードのワールド行列更新
        GetNodes()->at(nodeIndex).rotation_ = headRotate;
        DirectX::XMVECTOR HeadLocalPosition = GetJointGlobalTransform(headNodeName_, 0.01f).r[3];
        UpdateWorldTransforms(nodeIndex, HeadLocalPosition);
    }

#else

    // 頭のローカル前方向ベクトル
    DirectX::XMVECTOR HeadLocalForward = DirectX::XMLoadFloat3(&headLocalForward_);

    // 頭のワールド座標位置取得
    DirectX::XMMATRIX HeadGlobalMatrix = GetJointGlobalTransform(headNodeName_);

    DirectX::XMVECTOR HeadPosition = HeadGlobalMatrix.r[3];
    
    DirectX::XMFLOAT3 characterFront = GetTransform()->CalcForward();

    // ターゲットのワールド位置
    DirectX::XMVECTOR TargetPosition = DirectX::XMLoadFloat3(&lookAtTargetPosition_);

    // 頭のワールド逆行列
    DirectX::XMMATRIX World = GetJointWorldTransform(headNodeName_, 0.01f);
    DirectX::XMMATRIX InverseWorld = DirectX::XMMatrixInverse(nullptr, World);
    DirectX::XMFLOAT4X4 inverseWorld = {};
    DirectX::XMStoreFloat4x4(&inverseWorld, InverseWorld);
    TargetPosition = DirectX::XMVector3TransformCoord(TargetPosition, InverseWorld);

    DirectX::XMVECTOR HeadPositionWorld = DirectX::XMVector3TransformCoord(HeadPosition, World);

    DirectX::XMVECTOR characterFrontInverse = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&characterFront), InverseWorld);


    // ワールド空間のターゲット方向ベクトルを計算し、ローカル空間に変換
    //DirectX::XMVECTOR TargetLocalDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(TargetPosition, HeadPosition));
    DirectX::XMVECTOR TargetLocalDirection = TargetPosition;

    DirectX::XMVECTOR TargetLocalDirectionWorld = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract({}, HeadPositionWorld));
    DirectX::XMVECTOR HeadLocalForwardWorld = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(HeadLocalForward, World));
    HeadLocalForward = HeadLocalForwardWorld;
    //TargetLocalDirection = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(TargetLocalDirectionWorld, InverseWorld));
    //TargetLocalDirection = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(TargetLocalDirection, InverseWorld));

    // 回転軸計算
    //DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(HeadLocalForward, TargetLocalDirectionWorld));
    DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(TargetLocalDirection, characterFrontInverse));
    //Axis = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(Axis, InverseWorld));
    // 回転角計算
   // const float angle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(HeadLocalForward, TargetLocalDirectionWorld)));
    characterFrontInverse = DirectX::XMVector3Normalize(characterFrontInverse);
    DirectX::XMVECTOR TargetLocalDirectionNormalize = DirectX::XMVector3Normalize(TargetLocalDirection);
    float angle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(TargetLocalDirectionNormalize, characterFrontInverse)));
    angle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(
        DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&lookAtTargetPosition_), HeadPositionWorld)),
        DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&characterFront)))));
    DirectX::XMVECTOR wAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(
        DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&characterFront)),
        DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&lookAtTargetPosition_), HeadPositionWorld))));
    DirectX::XMVECTOR gAxis = DirectX::XMVector3Normalize(DirectX::XMVector3TransformCoord(wAxis, InverseWorld));
    gAxis = DirectX::XMVector3Normalize(World.r[2]);
    // 回転角が微小な場合は回転しない
    if (fabsf(angle) > 1e-8f)
    {
        const int nodeIndex = GetNodeIndex(headNodeName_);

        // 回転軸と回転角から回転クォータニオンを求める
        const DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationAxis(wAxis, angle);

        // 頭のローカル回転値に回転クォータニオンを合成する
        DirectX::XMFLOAT4 headRotate = GetNodes()->at(nodeIndex).rotation_;
        DirectX::XMVECTOR Rotation = DirectX::XMLoadFloat4(&headRotate);

        DirectX::XMMATRIX lMat = DirectX::XMMatrixRotationQuaternion(Rotation);
        DirectX::XMVECTOR wMatF = DirectX::XMVector3TransformCoord(lMat.r[2], World);
        DirectX::XMVECTOR wMatF2 = World.r[2];
        Rotation = DirectX::XMQuaternionMultiply(Rotation, Q);
        DirectX::XMStoreFloat4(&headRotate, Rotation);

        // 頭ノードとその子ノードのワールド行列更新
    //    GetNodes()->at(nodeIndex).rotation_ = headRotate;

    
        //UpdateWorldTransforms(nodeIndex);
    //    UpdateWorldTransforms(nodeIndex, HeadPosition);
    }
#endif
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