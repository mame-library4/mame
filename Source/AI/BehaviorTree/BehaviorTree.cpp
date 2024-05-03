#include "BehaviorTree.h"
#include "BehaviorData.h"
#include "NodeBase.h"
#include "../../Game/Character/Enemy/Enemy.h"

// ----- �f�X�g���N�^ -----
BehaviorTree::~BehaviorTree()
{
    NodeAllClear(root_);
}

// ----- ���_ -----
NodeBase* BehaviorTree::ActiveNodeInference(BehaviorData* data)
{
    // �f�[�^�����Z�b�g���ĊJ�n
    data->Initialize();
    return root_->Inference(data);
}

// ----- �V�[�N�G���X�m�[�h����̐��_�J�n -----
NodeBase* BehaviorTree::SequenceBack(NodeBase* sequenceNode, BehaviorData* data)
{
    return sequenceNode->Inference(data);
}

// ----- �m�[�h�ǉ� -----
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

// ----- �m�[�h���s -----
NodeBase* BehaviorTree::Run(NodeBase* actionNode, BehaviorData* data, float elapsedTime)
{
    // �m�[�h���s
    ActionBase::State state = actionNode->Run(elapsedTime);

    // ����I��
    if (state == ActionBase::State::Complete)
    {
        // �V�[�N�G���X�̓r�����𔻒f
        NodeBase* sequenceNode = data->PopSequenceNode();

        // �r������Ȃ��Ȃ�I��
        if (sequenceNode == nullptr)
        {
            return nullptr;
        }
        // �r���Ȃ炻������n�߂�
        else
        {
            return SequenceBack(sequenceNode, data);
        }
    }
    // ���s�͏I��
    else if (state == ActionBase::State::Failed)
    {
        return nullptr;
    }

    // ����ێ�
    return actionNode;
}

// ----- �o�^���ꂽ�m�[�h��S�č폜���� -----
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
