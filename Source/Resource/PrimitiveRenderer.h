#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <memory>
#include "ConstantBuffer.h"

class PrimitiveRenderer
{
public:
    PrimitiveRenderer();

    void AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& texcoord);
    void SetTexcoord();
    void Render(ID3D11PixelShader* pixelShader = nullptr);
    void ClearVertices();

private:
    static const UINT vertexCapacity_ = 3 * 1024;

    struct SceneConstants
    {
        DirectX::XMFLOAT4X4 viewProjection_;
        DirectX::XMFLOAT4   color_;
    };
    
    struct Vertex
    {
        DirectX::XMFLOAT3 position_;
        DirectX::XMFLOAT4 color_;
        DirectX::XMFLOAT2 texcoord_;
    };
    std::vector<Vertex> vertices_;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>   inputLayout_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        vertexBuffer_;
    std::unique_ptr<ConstantBuffer<SceneConstants>> constantBuffer_;
};

