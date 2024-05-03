#include "BehaviorData.h"
#include "NodeBase.h"

NodeBase* BehaviorData::PopSequenceNode()
{
    // �󂾂�����null
    if (sequenceStack_.empty() != 0)
    {
        return nullptr;
    }

    NodeBase* node = sequenceStack_.top();
    if (node != nullptr)
    {
        // ���o�����f�[�^���폜
        sequenceStack_.pop();
    }

    return node;
}

int BehaviorData::GetSequenceStep(std::string name)
{
    if (runSequenceStepMap_.count(name) == 0)
    {
        runSequenceStepMap_.insert(std::make_pair(name, 0));
    }

    return runSequenceStepMap_.at(name);
}

void BehaviorData::SetSequenceStep(std::string name, int step)
{
    runSequenceStepMap_.at(name) = step;
}

// ----- ������ -----
void BehaviorData::Initialize()
{
    runSequenceStepMap_.clear();
    while (sequenceStack_.size() > 0)
    {
        sequenceStack_.pop();
    }
}
