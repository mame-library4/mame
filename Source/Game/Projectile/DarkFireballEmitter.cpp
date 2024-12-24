#include "DarkFireballEmitter.h"
#include "DarkFireball.h"
#include "MathHelper.h"
#include "Character/Enemy/EnemyManager.h"

// ----- 初期化 -----
void DarkFireballEmitter::Initilaize(const int& emitNum, const DirectX::XMFLOAT3& staffPosition, const DirectX::XMFLOAT3& ownerForward, const DirectX::XMFLOAT3& ownerRight)
{
    currentEmitCount_ = 0;
    maxEmitNum_ = emitNum;

    emitTimer_ = 0.0f;

    staffPosition_  = staffPosition;
    ownerForward_   = ownerForward;
    ownerRight_     = ownerRight;
}

// ----- 更新 -----
const bool DarkFireballEmitter::Update(const float& elapsedTime)
{
    if (currentEmitCount_ >= maxEmitNum_) return false;

    emitTimer_ -= elapsedTime;

    if (emitTimer_ <= 0.0f)
    {
        DarkFireball* darkFireball = new DarkFireball();

        DirectX::XMFLOAT3 ownerUp = XMFloat3Cross(ownerForward_, ownerRight_);

        DirectX::XMFLOAT3 moveDirectionList[] =
        {
            { ownerForward_ + ownerRight_},      // 斜め右
            { ownerForward_ + ownerRight_ * -1}, // 斜め左
            { ownerForward_ + ownerUp},          // 斜め上
            { ownerForward_ + ownerUp * -1},     // 斜め下
            { ownerForward_ + ownerUp + ownerRight_},           // 斜め上右
            { ownerForward_ + ownerUp + ownerRight_ * -1},      // 斜め上左
            { ownerForward_ + ownerUp * -1 + ownerRight_},      // 斜め下右
            { ownerForward_ + ownerUp * -1 + ownerRight_ * -1}, // 斜め下左
        };

        DirectX::XMFLOAT3 moveDirection = XMFloat3Normalize(moveDirectionList[currentEmitCount_]);

        DirectX::XMFLOAT3 targetPosition = EnemyManager::Instance().GetEnemy(0)->GetJointPosition("Dragon15_spine2");

        darkFireball->Launch(staffPosition_, moveDirection, targetPosition, moveSpeed_, changeTime_);

        emitTimer_ = emitTime_;
        ++currentEmitCount_;
    }

    return true;
}

void DarkFireballEmitter::DrawDebug()
{
    if (ImGui::TreeNodeEx("DarkFireballEmitter", ImGuiTreeNodeFlags_Framed))
    {
        ImGui::DragFloat("MoveSpeed", &moveSpeed_, 0.01f);
        ImGui::DragFloat("ChangeTime", &changeTime_, 0.01f);

        ImGui::DragFloat("EmitTime", &emitTime_, 0.01f);
        ImGui::DragFloat("EmitTimer", &emitTimer_, 0.01f);
        ImGui::DragInt("CurrentEmitCount", &currentEmitCount_);
        ImGui::DragInt("MaxEmitNum", &maxEmitNum_);

        ImGui::TreePop();
    }
}
