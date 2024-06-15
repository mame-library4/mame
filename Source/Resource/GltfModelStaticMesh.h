#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <unordered_map>

#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tinygltf/tiny_gltf.h"

#include "ConstantBuffer.h"
#include "Transform.h"

class GltfModelStaticMesh
{
public:
    GltfModelStaticMesh(const std::string& filename);
    virtual ~GltfModelStaticMesh() = default;

    struct Scene
    {
        std::string         name_;
        std::vector<int>    nodes_;
    };
    struct Node
    {
        std::string name_;
        int         skin_ = -1; // index of skin referenced by this node
        int         mesh_ = -1; // index of mesh referenced by this node

        std::vector<int> children_;

        // Local transforms
        DirectX::XMFLOAT4 rotation_     = { 0, 0, 0, 1 };
        DirectX::XMFLOAT3 scale_        = { 1, 1, 1 };
        DirectX::XMFLOAT3 translation_  = { 0, 0, 0 };

        DirectX::XMFLOAT4X4 globalTransform_ = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    };
    struct BufferView
    {
        DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
        Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
        size_t strideInBytes_   = 0;
        size_t sizeInBytes_     = 0;
        size_t count() const
        {
            return sizeInBytes_ / strideInBytes_;
        }
        BufferView() = default;
    };
    struct Primitive
    {
        int material_;
        std::map<std::string, BufferView> vertexBufferView_;
        BufferView indexBufferView_;
    };
    struct TextureInfo
    {
        int index_ = -1;
        int texcoord_ = 0;
    };
    struct NormalTextureInfo
    {
        int index_ = -1;
        int texcoord_ = 0;
        float scale_ = 1;
    };
    struct OcclusionTextureInfo
    {
        int index_ = -1;
        int texcoord_ = 0;
        float strength_ = 1;
    };
    struct PbrMetallicRoughness
    {
        float baseColorFactor_[4] = { 1, 1, 1, 1 };
        TextureInfo baseColorTexture_;
        float metallicFactor_ = 1;
        float roughnessFactor_ = 1;
        TextureInfo metallicRoughnessTexture_;
    };
    struct Material
    {
        std::string name_;
        struct CBuffer
        {
            float emissiveFactor_[3] = { 0, 0, 0 };
            int alphaMode_ = 0;
            float alphaCutoff_ = 0.5f;
            int doubleSided_ = 0;

            PbrMetallicRoughness pbrMetallicRoughness_;
            NormalTextureInfo normalTexture_;
            OcclusionTextureInfo occlusionTexture_;
            TextureInfo emissiveTexture_;
        };
        CBuffer data_;
    };
    struct TextureData
    {
        std::string name_;
        int source_ = -1;
    };
    struct Image
    {
        std::string name_;
        int width_ = -1;
        int height_ = -1;
        int component_ = -1;
        int bits_ = -1;
        int pixelType_ = -1;
        int bufferView_;
        std::string mimeType_;
        std::string uri_;
        bool asIs_ = false;
    };

    void Render(const float& scaleFactor, ID3D11PixelShader* psShader = nullptr);

    Transform* GetTransform() { return &transform_; }

private:
    void FetchNodes(const tinygltf::Model& gltfModel);
    void FetchMeshes(const tinygltf::Model& gltfModel);
    void FetchTextures(const tinygltf::Model& gltfModel);
    void FetchMaterials(const tinygltf::Model& gltfModel);
    void CumulateTransforms(std::vector<Node>& nodes);
    BufferView MakeBufferView(const tinygltf::Accessor& accessor);

private:
    std::vector<Scene>      scenes_;
    std::vector<Node>       nodes_;
    std::vector<Primitive>  primitives_;
    std::vector<Material>   materials_;
    std::vector<TextureData>textures_;
    std::vector<Image>      images_;
    
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureResourceViews_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> materialResourceView_;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>   inputLayout_;
    struct PrimitiveConstants
    {
        DirectX::XMFLOAT4X4 world_;
        int material_ = -1;
        int hasTangent_ = 0;
        int pad_[2];
    };
    std::unique_ptr<ConstantBuffer<PrimitiveConstants>> primitiveConstantBuffer_;

    std::string filename_;

    Transform transform_ = {};
};

