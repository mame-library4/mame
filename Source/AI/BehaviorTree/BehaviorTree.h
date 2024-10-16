#pragma once
#include <string>

class ActionBase;
class JudgmentBase;
class NodeBase;
class BehaviorData;
class Enemy;

class BehaviorTree
{
public:
    enum class SelectRule
    {
        None,               // 無い端末ノード用
        Priority,           // 優先順位
        Sequence,           // シーケンス
        SequentialLooping,  // シーケンシャルルーピング
        Random,             // ランダム
    };

public:
    BehaviorTree() : root_(nullptr), owner_(nullptr) {}
    BehaviorTree(Enemy* owner) : root_(nullptr), owner_(owner) {}
    ~BehaviorTree();

    NodeBase* ActiveNodeInference(BehaviorData* data);
    NodeBase* SequenceBack(NodeBase* sequenceNode, BehaviorData* data);

    void AddNode(std::string parentName, std::string entryName, int priority, SelectRule selectRule, JudgmentBase* judgment, ActionBase* action);

    NodeBase* Run(NodeBase* actionNode, BehaviorData* data, float elapsedTime);

    void DrawDebug();

private:
    void NodeAllClear(NodeBase* delNode);

private:
    NodeBase* root_;
    Enemy* owner_;
};

