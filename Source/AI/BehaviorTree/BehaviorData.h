#pragma once
#include <vector>
#include <stack>
#include <map>
#include "BehaviorTree.h"

class NodeBase;

// �܂������Ă邩��
class BehaviorData
{
public:
    BehaviorData() { Initialize(); }

    void PushSequenceNode(NodeBase* node) { sequenceStack_.push(node); }
    NodeBase* PopSequenceNode();

    int GetSequenceStep(std::string name);
    void SetSequenceStep(std::string name, int step);

    void Initialize();

private:
    std::stack<NodeBase*> sequenceStack_;               // ���s���钆�ԃm�[�h���X�^�b�N
    std::map<std::string, int> runSequenceStepMap_;     // ���s���̒��ԃm�[�h�̃X�e�b�v���L�^
};


