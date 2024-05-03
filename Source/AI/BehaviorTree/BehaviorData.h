#pragma once
#include <vector>
#include <stack>
#include <map>
#include "BehaviorTree.h"

class NodeBase;

// まちがってるかも
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
    std::stack<NodeBase*> sequenceStack_;               // 実行する中間ノードをスタック
    std::map<std::string, int> runSequenceStepMap_;     // 実行中の中間ノードのステップを記録
};


