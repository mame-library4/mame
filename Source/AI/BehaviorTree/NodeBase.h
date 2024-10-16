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

    // --- �s���f�[�^�������Ă��邩 ---
    bool HasAction() { return (action_ != nullptr) ? true : false; }

    // --- ���s�۔��� ---
    bool Judgment();

    // --- �D�揇�ʑI�� ---
    NodeBase* SelectPriority(std::vector<NodeBase*>* candidateList);

    // --- �����_���I�� ---
    NodeBase* SelectRandom(std::vector<NodeBase*>* candidateList);

    // --- �V�[�P���X�I�� ---
    NodeBase* SelectSequence(std::vector<NodeBase*>* candidateList, BehaviorData* data);

    // --- �m�[�h���� ---
    NodeBase* SearchNode(std::string searchName);

    // --- �m�[�h���_ ---
    NodeBase* Inference(BehaviorData* data);

    // --- ���s ---
    ActionBase::State Run(const float& elapsedTime);

    void DrawDebug();

public:// --- �擾�E�ݒ� ---
#pragma region [Get, Set] Function
    // --- ���O ---
    std::string GetName() { return name_; }     

    // --- �e�m�[�h ---
    [[nodiscard]] NodeBase* GetParent() { return parent_; }
    void SetParent(NodeBase* parent) { parent_ = parent; }

    // --- �q�m�[�h ---
    [[nodiscard]] NodeBase* GetChild(const int& index);
    [[nodiscard]] NodeBase* GetLastChild();
    [[nodiscard]] NodeBase* GetTopChild();

    // --- �Z��m�[�h ---
    [[nodiscard]] NodeBase* GetSibling() { return sibling_; }
    void SetSibling(NodeBase* sibling) { sibling_ = sibling; }

    // --- �K�w�ԍ� ---
    [[nodiscard]] int GetHirerchyNo() { return hierarchyNo_; }

    // --- �D�揇�� ---
    [[nodiscard]] int GetPriority() { return priority_; }


#pragma endregion [Get, Set] Function

    std::vector<NodeBase*> children_;

protected:
    std::string                 name_;          // ���O
    BehaviorTree::SelectRule    selectRule_;    // �I�����[��
    JudgmentBase*               judgment_;      // ����N���X
    ActionBase*                 action_;        // ���s�N���X
    unsigned int                priority_;      // �D�揇��
    NodeBase*                   parent_;        // �e�m�[�h
    NodeBase*                   sibling_;       // �Z��m�[�h
    int                         hierarchyNo_;   // �K�w�ԍ�
};

