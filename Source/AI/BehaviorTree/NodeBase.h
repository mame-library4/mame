#pragma once

#include <vector>
#include <string>
#include "BehaviorTree.h"
#include "ActionBase.h"

class JudgmentBase;
class BehaviorData;

class NodeBase
{
public:
    NodeBase(std::string name, NodeBase* parent, NodeBase* sibling, int priority,
        BehaviorTree::SelectRule selectRule, JudgmentBase* judgment, ActionBase* action, int hierarchyNo)
        : name_(name), parent_(parent), sibling_(sibling), priority_(priority),
        selectRule_(selectRule), judgment_(judgment), action_(action), hierarchyNo_(hierarchyNo),
        children_(NULL)
    {
    }
    ~NodeBase();

    void AddChild(NodeBase* child) { children_.push_back(child); }

    // --- 行動データを持っているか ---
    bool HasAction() { return (action_ != nullptr) ? true : false; }

    // --- 実行可否判定 ---
    bool Judgment();

    // --- 優先順位選択 ---
    NodeBase* SelectPriority(std::vector<NodeBase*>* candidateList);

    // --- ランダム選択 ---
    NodeBase* SelectRandom(std::vector<NodeBase*>* candidateList);

    // --- シーケンス選択 ---
    NodeBase* SelectSequence(std::vector<NodeBase*>* candidateList, BehaviorData* data);

    // --- ノード検索 ---
    NodeBase* SearchNode(std::string searchName);

    // --- ノード推論 ---
    NodeBase* Inference(BehaviorData* data);

    // --- 実行 ---
    ActionBase::State Run(const float& elapsedTime);

    void DrawDebug();

public:// --- 取得・設定 ---
#pragma region [Get, Set] Function
    // --- 名前 ---
    std::string GetName() { return name_; }     

    // --- 親ノード ---
    [[nodiscard]] NodeBase* GetParent() { return parent_; }
    void SetParent(NodeBase* parent) { parent_ = parent; }

    // --- 子ノード ---
    [[nodiscard]] NodeBase* GetChild(const int& index);
    [[nodiscard]] NodeBase* GetLastChild();
    [[nodiscard]] NodeBase* GetTopChild();

    // --- 兄弟ノード ---
    [[nodiscard]] NodeBase* GetSibling() { return sibling_; }
    void SetSibling(NodeBase* sibling) { sibling_ = sibling; }

    // --- 階層番号 ---
    [[nodiscard]] int GetHirerchyNo() { return hierarchyNo_; }

    // --- 優先順位 ---
    [[nodiscard]] int GetPriority() { return priority_; }


#pragma endregion [Get, Set] Function

    std::vector<NodeBase*> children_;

protected:
    std::string                 name_;          // 名前
    BehaviorTree::SelectRule    selectRule_;    // 選択ルール
    JudgmentBase*               judgment_;      // 判定クラス
    ActionBase*                 action_;        // 実行クラス
    unsigned int                priority_;      // 優先順位
    NodeBase*                   parent_;        // 親ノード
    NodeBase*                   sibling_;       // 兄弟ノード
    int                         hierarchyNo_;   // 階層番号
};

