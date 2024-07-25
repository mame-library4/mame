#pragma once
#include "Stone.h"
#include <vector>
#include <set>


class StoneManager
{
private:
    StoneManager();
    ~StoneManager() {}

    struct InstanceData
    {
        DirectX::XMFLOAT4X4 world_;
    };

public:
    static StoneManager& Instance()
    {
        static StoneManager instance;
        return instance;
    }

    void Finalize();
    void Update(const float& elapsedTime);
    void Render(ID3D11PixelShader* psShader);
    void DrawDebug();

    void Register(Stone* stone);
    void Remove(Stone* stone);
    void Clear();

    const int GetStoneCount() const { return stones_.size(); }
    Stone* GetStone(const int& index) { return stones_.at(index); }

private:
    std::vector<Stone*> stones_;
    std::set<Stone*>    removes_;
    std::set<Stone*>    generates_;   

    int vertexCount_    = 0;
    int instanceCount_  = 0;


private:
    std::string filename_;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>   inputLayout_;

};

