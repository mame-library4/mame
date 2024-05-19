#include "NodeBase.h"
#include "../../Game/Character/Enemy/EnemyTamamo.h"
#include "BehaviorTree.h"
#include "BehaviorData.h"
#include "ActionBase.h"
#include "JudgmentBase.h"

// ----- �f�X�g���N�^ -----
NodeBase::~NodeBase()
{
    delete judgment_;
    delete action_;
}

// ----- ���� -----
bool NodeBase::Judgment()
{
    if (judgment_ != nullptr)
    {
        return judgment_->Judgment();
    }

    return false;
}

// ----- �D�揇�ʂŃm�[�h���� -----
NodeBase* NodeBase::SelectPriority(std::vector<NodeBase*>* candidateList)
{
    NodeBase* selectNode = nullptr;
    unsigned int priority = UINT_MAX;

    // ��ԗD�揇�ʂ����� ( �l�������� ) �m�[�h��T����selectNode�Ɋi�[
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

// ----- �����_���Ńm�[�h���� -----
NodeBase* NodeBase::SelectRandom(std::vector<NodeBase*>* candidateList)
{
    // list�̃T�C�Y�ŗ������擾����selectNo�Ɋi�[
    const size_t lastNode = (*candidateList).size();
    //const size_t lastNode = (*candidateList).size() - 1;
    const size_t selectNo = static_cast<size_t>(std::rand()) % lastNode;

    // list��selectNo�Ԗڂ̎��Ԃ����^�[��
    return (*candidateList).at(selectNo);
}

// ----- �V�[�P���X�E�V�[�P���V�������[�s���O�Ńm�[�h���� -----
NodeBase* NodeBase::SelectSequence(std::vector<NodeBase*>* candidateList, BehaviorData* data)
{
    int step = 0;

    // �w�肳��Ă��钆�ԃm�[�h�̃V�[�P���X���ǂ��܂Ŏ��s���ꂽ���擾����
    step = data->GetSequenceStep(name_);

    // ���ԃm�[�h�ɓo�^����Ă���m�[�h���ȏ�̏ꍇ
    if (step >= children_.size())
    {
        // ���[����SequentialLooping�̎��͍ŏ�������s���邽�߁Astep�ɂO����
        if (BehaviorTree::SelectRule::SequentialLooping == selectRule_)
        {
            step = 0;
        }
        else if (BehaviorTree::SelectRule::Sequence == selectRule_)
        {
            return nullptr;
        }
    }

    // ���s�\���X�g�ɓo�^����Ă���f�[�^�̐��������[�v���s��
    for (std::vector<NodeBase*>::iterator itr = candidateList->begin(); itr != candidateList->end(); ++itr)
    {
        // �q�m�[�h�����s�\���X�g�Ɋ܂܂�Ă��邩
        NodeBase* childNode = children_.at(step);
        NodeBase* condidateNode = (*itr);
        if (childNode->GetName() == condidateNode->GetName())
        {
            // ���݂̎��s�m�[�h�̕ۑ�
            data->PushSequenceNode(this);

            // ���Ɏ��s���钆�ԃm�[�h�ƃX�e�b�v����ۑ�����
            data->SetSequenceStep(GetName(), step + 1);

            // ���݂̃X�e�b�v�ԍ��̃m�[�h��Ԃ�
            return childNode;
        }
    }

    // �w�肳�ꂽ���ԃm�[�h�Ɏ��s�\�m�[�h���Ȃ��̂�nullptr�����^�[������
    return nullptr;
}

// ----- �m�[�h���� -----
NodeBase* NodeBase::SearchNode(std::string searchName)
{
    // ���O����v
    if (searchName == name_)
    {
        return this;
    }
    else
    {
        // �q�m�[�h�Ō���
        for (std::vector<NodeBase*>::iterator itr = children_.begin(); itr != children_.end(); ++itr)
        {
            NodeBase* ret = (*itr)->SearchNode(searchName);

            if (ret != nullptr) return ret;
        }
    }

    return nullptr;
}

// ----- �m�[�h���_ -----
NodeBase* NodeBase::Inference(BehaviorData* data)
{
    std::vector<NodeBase*> candidateList = {}; // ���s����m�[�h�̌�⃊�X�g
    NodeBase* selectNode = nullptr;            // �I�����ꂽ���s�m�[�h

    // children�̐��������[�v���s��
    for (int childIndex = 0; childIndex < children_.size(); ++childIndex)
    {
        NodeBase* node = children_.at(childIndex);

        // children.at(i)->judgment��nullptr�łȂ����
        if (node->judgment_ != nullptr)
        {
            if (node->judgment_->Judgment())
            {
                candidateList.emplace_back(node);
            }
        }
        else
        {
            // ����N���X���Ȃ���Ζ������ɒǉ�
            candidateList.emplace_back(node);
        }
    }

    // �I�����[���Ńm�[�h����
    switch (selectRule_)
    {
        // �D�揇��
    case BehaviorTree::SelectRule::Priority:
        selectNode = SelectPriority(&candidateList);
        break;
        // �����_��
    case BehaviorTree::SelectRule::Random:
        selectNode = SelectRandom(&candidateList);
        break;
        // �V�[�N�G���X
    case BehaviorTree::SelectRule::Sequence:
    case BehaviorTree::SelectRule::SequentialLooping:
        selectNode = SelectSequence(&candidateList, data);
        break;
    }

    if (selectNode != nullptr)
    {
        // �s��������ΏI��
        if (selectNode->HasAction() == true)
        {
            return selectNode;
        }
        // ���܂����m�[�h�Ő��_�J�n
        else
        {
            selectNode = selectNode->Inference(data);
        }
    }

    return selectNode;
}

// ----- �m�[�h���s -----
ActionBase::State NodeBase::Run(const float& elapsedTime)
{
    // axtion�����邩���f�B����΃����o�֐�Run()���s�������ʂ����^�[��
    if (action_ != nullptr)
    {
        return action_->Run(elapsedTime);
    }

    return ActionBase::State::Failed;
}

// ----- �q�m�[�h�擾 -----
NodeBase* NodeBase::GetChild(const int& index)
{
    if (children_.size() <= index) return nullptr;

    return children_.at(index);
}

// ----- �q�m�[�h�擾 ( ���� ) -----
NodeBase* NodeBase::GetLastChild()
{
    if (children_.size() == 0) return nullptr;

    return children_.at(children_.size() - 1);
}

// ----- �q�m�[�h�擾 ( �擪 ) -----
NodeBase* NodeBase::GetTopChild()
{
    if (children_.size() == 0) return nullptr;

    return children_.at(0);
}
