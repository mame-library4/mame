#include "NodeBase.h"
#include "../../Game/Character/Enemy/EnemyTamamo.h"
#include "BehaviorTree.h"
#include "BehaviorData.h"
#include "ActionBase.h"
#include "JudgmentBase.h"

// ----- デストラクタ -----
NodeBase::~NodeBase()
{
    delete judgment_;
    delete action_;
}

// ----- 判定 -----
bool NodeBase::Judgment()
{
    if (judgment_ != nullptr)
    {
        return judgment_->Judgment();
    }

    return false;
}

// ----- 優先順位でノード検索 -----
NodeBase* NodeBase::SelectPriority(std::vector<NodeBase*>* candidateList)
{
    NodeBase* selectNode = nullptr;
    unsigned int priority = UINT_MAX;

    // 一番優先順位が高い ( 値が小さい ) ノードを探してselectNodeに格納
    for (size_t i = 0; i < (*candidateList).size(); ++i)
    {
        NodeBase* node = (*candidateList).at(i);
        if (node->priority_ < priority)
        {
            priority = node->priority_;
            selectNode = node;
        }
    }

    return selectNode;
}

// ----- ランダムでノード検索 -----
NodeBase* NodeBase::SelectRandom(std::vector<NodeBase*>* candidateList)
{
    // listのサイズで乱数を取得してselectNoに格納
    const size_t lastNode = (*candidateList).size();
    //const size_t lastNode = (*candidateList).size() - 1;
    const size_t selectNo = static_cast<size_t>(std::rand()) % lastNode;

    // listのselectNo番目の実態をリターン
    return (*candidateList).at(selectNo);
}

// ----- シーケンス・シーケンシャルルーピングでノード検索 -----
NodeBase* NodeBase::SelectSequence(std::vector<NodeBase*>* candidateList, BehaviorData* data)
{
    int step = 0;

    // 指定されている中間ノードのシーケンスがどこまで実行されたか取得する
    step = data->GetSequenceStep(name_);

    // 中間ノードに登録されているノード数以上の場合
    if (step >= children_.size())
    {
        // ルールがSequentialLoopingの時は最初から実行するため、stepに０を代入
        if (BehaviorTree::SelectRule::SequentialLooping == selectRule_)
        {
            step = 0;
        }
        else if (BehaviorTree::SelectRule::Sequence == selectRule_)
        {
            return nullptr;
        }
    }

    // 実行可能リストに登録されているデータの数だけループを行う
    for (std::vector<NodeBase*>::iterator itr = candidateList->begin(); itr != candidateList->end(); ++itr)
    {
        // 子ノードが実行可能リストに含まれているか
        NodeBase* childNode = children_.at(step);
        NodeBase* condidateNode = (*itr);
        if (childNode->GetName() == condidateNode->GetName())
        {
            // 現在の実行ノードの保存
            data->PushSequenceNode(this);

            // 次に実行する中間ノードとステップ数を保存する
            data->SetSequenceStep(GetName(), step + 1);

            // 現在のステップ番号のノードを返す
            return childNode;
        }
    }

    // 指定された中間ノードに実行可能ノードがないのでnullptrをリターンする
    return nullptr;
}

// ----- ノード検索 -----
NodeBase* NodeBase::SearchNode(std::string searchName)
{
    // 名前が一致
    if (searchName == name_)
    {
        return this;
    }
    else
    {
        // 子ノードで検索
        for (std::vector<NodeBase*>::iterator itr = children_.begin(); itr != children_.end(); ++itr)
        {
            NodeBase* ret = (*itr)->SearchNode(searchName);

            if (ret != nullptr) return ret;
        }
    }

    return nullptr;
}

// ----- ノード推論 -----
NodeBase* NodeBase::Inference(BehaviorData* data)
{
    std::vector<NodeBase*> candidateList = {}; // 実行するノードの候補リスト
    NodeBase* selectNode = nullptr;            // 選択された実行ノード

    // childrenの数だけループを行う
    for (int childIndex = 0; childIndex < children_.size(); ++childIndex)
    {
        NodeBase* node = children_.at(childIndex);

        // children.at(i)->judgmentがnullptrでなければ
        if (node->judgment_ != nullptr)
        {
            if (node->judgment_->Judgment())
            {
                candidateList.emplace_back(node);
            }
        }
        else
        {
            // 判定クラスがなければ無条件に追加
            candidateList.emplace_back(node);
        }
    }

    // 選択ルールでノード決め
    switch (selectRule_)
    {
        // 優先順位
    case BehaviorTree::SelectRule::Priority:
        selectNode = SelectPriority(&candidateList);
        break;
        // ランダム
    case BehaviorTree::SelectRule::Random:
        selectNode = SelectRandom(&candidateList);
        break;
        // シークエンス
    case BehaviorTree::SelectRule::Sequence:
    case BehaviorTree::SelectRule::SequentialLooping:
        selectNode = SelectSequence(&candidateList, data);
        break;
    }

    if (selectNode != nullptr)
    {
        // 行動があれば終了
        if (selectNode->HasAction() == true)
        {
            return selectNode;
        }
        // 決まったノードで推論開始
        else
        {
            selectNode = selectNode->Inference(data);
        }
    }

    return selectNode;
}

// ----- ノード実行 -----
ActionBase::State NodeBase::Run(const float& elapsedTime)
{
    // axtionがあるか判断。あればメンバ関数Run()実行した結果をリターン
    if (action_ != nullptr)
    {
        return action_->Run(elapsedTime);
    }

    return ActionBase::State::Failed;
}

// ----- 子ノード取得 -----
NodeBase* NodeBase::GetChild(const int& index)
{
    if (children_.size() <= index) return nullptr;

    return children_.at(index);
}

// ----- 子ノード取得 ( 末尾 ) -----
NodeBase* NodeBase::GetLastChild()
{
    if (children_.size() == 0) return nullptr;

    return children_.at(children_.size() - 1);
}

// ----- 子ノード取得 ( 先頭 ) -----
NodeBase* NodeBase::GetTopChild()
{
    if (children_.size() == 0) return nullptr;

    return children_.at(0);
}
