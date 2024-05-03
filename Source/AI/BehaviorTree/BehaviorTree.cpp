#include "BehaviorTree.h"
#include "BehaviorData.h"
#include "NodeBase.h"
#include "../../Game/Character/Enemy/Enemy.h"

// ----- デストラクタ -----
BehaviorTree::~BehaviorTree()
{
    NodeAllClear(root_);
}

// ----- 推論 -----
NodeBase* BehaviorTree::ActiveNodeInference(BehaviorData* data)
{
    // データをリセットして開始
    data->Initialize();
    return root_->Inference(data);
}

// ----- シークエンスノードからの推論開始 -----
NodeBase* BehaviorTree::SequenceBack(NodeBase* sequenceNode, BehaviorData* data)
{
    return sequenceNode->Inference(data);
}

// ----- ノード追加 -----
void BehaviorTree::AddNode(std::string parentName, std::string entryName, int priority, SelectRule selectRule, JudgmentBase* judgment, ActionBase* action)
{
    if (parentName != "")
    {
        NodeBase* searchNode = root_->SearchNode(parentName);
        if (searchNode != nullptr)
        {
            NodeBase* sibling = searchNode->GetLastChild();
            NodeBase* addNode = new NodeBase(entryName, searchNode, sibling, priority, selectRule,
                judgment, action, searchNode->GetHirerchyNo() + 1);
            searchNode->AddChild(addNode);
        }
    }
    else
    {
        if (root_ == nullptr)
        {
            root_ = new NodeBase(entryName, nullptr, nullptr, priority, selectRule, judgment, action, 1);
        }
    }
}

// ----- ノード実行 -----
NodeBase* BehaviorTree::Run(NodeBase* actionNode, BehaviorData* data, float elapsedTime)
{
    // ノード実行
    ActionBase::State state = actionNode->Run(elapsedTime);

    // 正常終了
    if (state == ActionBase::State::Complete)
    {
        // シークエンスの途中かを判断
        NodeBase* sequenceNode = data->PopSequenceNode();

        // 途中じゃないなら終了
        if (sequenceNode == nullptr)
        {
            return nullptr;
        }
        // 途中ならそこから始める
        else
        {
            return SequenceBack(sequenceNode, data);
        }
    }
    // 失敗は終了
    else if (state == ActionBase::State::Failed)
    {
        return nullptr;
    }

    // 現状維持
    return actionNode;
}

// ----- 登録されたノードを全て削除する -----
void BehaviorTree::NodeAllClear(NodeBase* delNode)
{
    size_t count = delNode->children_.size();
    if (count > 0)
    {
        for (NodeBase* node : delNode->children_)
        {
            NodeAllClear(node);
        }
        delete delNode;
    }
    else
    {
        delete delNode;
    }
}
