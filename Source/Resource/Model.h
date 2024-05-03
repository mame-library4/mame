#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include <vector>
#include <string>

#include <fbxsdk.h>

#include <unordered_map>

#include "../Graphics/ConstantBuffer.h"

// cereal
#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>

// DirectXMath構造体用のserializeテンプレート
namespace DirectX
{
    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT2& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y)
        );
    }

    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT3& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z)
        );
    }

    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT4X4& m)
    {
        archive(
            cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
            cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
            cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
            cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
            cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
            cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
            cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
            cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
        );
    }
}

// スケルトン
struct Skeleton
{
    // ボーン
    struct Bone
    {
        uint64_t uniqueId{ 0 };     // ID
        std::string name;           // ボーンの名前
        int64_t parentIndex{ -1 };  // 親ボーンのIndex
        int64_t nodeIndex{ 0 };     // シーンノードのIndex

        // モデル (メッシュ) 空間からボーン (ノード) シーンへの変換に使用される。
        DirectX::XMFLOAT4X4 offsetTransform{
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
        };

        bool IsOrphan()const { return parentIndex < 0; };

        // serialize
        template<class T>
        void serialize(T& archive)
        {
            archive(uniqueId, name, parentIndex, nodeIndex, offsetTransform);
        }
    };
    std::vector<Bone>bones;
    int64_t indexof(uint64_t uniqueId)const
    {
        int64_t index{ 0 };
        for (const Bone& bone : bones)
        {
            if (bone.uniqueId == uniqueId)
            {
                return index;
            }
            ++index;
        }
        return -1;
    }

    // serialize
    template<class T>
    void serialize(T& archive)
    {
        archive(bones);
    }
};

// アニメーション
struct Animation
{
    std::string name;
    float samplingRate{ 0 };

    struct KeyFrame
    {
        struct Node
        {
            // ノードのローカル空間からシーンのグローバル空間に変換するために使用される。
            DirectX::XMFLOAT4X4 globalTransform{
                1,0,0,0,
                0,1,0,0,
                0,0,1,0,
                0,0,0,1
            };

            // ノードの変換データには、親に対する移動、回転、スケーリング ベクトルが含まれる。
            DirectX::XMFLOAT3 scaling{ 1,1,1 };
            DirectX::XMFLOAT4 rotation{ 0,0,0,1 };  // Rotation quaternion
            DirectX::XMFLOAT3 translation{ 0,0,0 };

            // serialize
            template<class T>
            void serialize(T& archive)
            {
                archive(globalTransform, scaling, rotation, translation);
            }
        };
        std::vector<Node> nodes;

        // serialize
        template<class T>
        void serialize(T& archive)
        {
            archive(nodes);
        }
    };
    std::vector<KeyFrame> sequence;

    // serialize
    template<class T>
    void serialize(T& archive)
    {
        archive(name, samplingRate, sequence);
    }
};

// シーン
struct Scene
{
    struct Node
    {
        uint64_t uniqueId{ 0 };
        std::string name;
        FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
        int64_t parentIndex{ -1 };

        // serialize
        template<class T>
        void serialize(T& archive)
        {
            archive(uniqueId, name, attribute, parentIndex);
        }
    };
    std::vector<Node> nodes;

    int64_t IndexOf(uint64_t uniqueId)const
    {
        int64_t index{ 0 };
        for (const Node& node : nodes)
        {
            if (node.uniqueId == uniqueId)
            {
                return index;
            }
            ++index;
        }
        return -1;
    }

    // serialize
    template<class T>
    void serialize(T& archive)
    {
        archive(nodes);
    }
};

class Model
{
public:
    // 頂点情報
    static const int MAX_BONE_INFLUENCES{ 4 };
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT4 tangent;
        DirectX::XMFLOAT2 texcoord;
        float boneWeights[MAX_BONE_INFLUENCES]{ 1,0,0,0 };
        uint32_t boneIndices[MAX_BONE_INFLUENCES]{};

        // serialize
        template<class T>
        void serialize(T& archive)
        {
            archive(position, normal, tangent, texcoord, boneWeights, boneIndices);
        }
    };

    // 定数バッファー
    static const int MAX_BONES{ 256 };
    struct Constants
    {
        DirectX::XMFLOAT4X4 world;
        DirectX::XMFLOAT4 materialColor;
        DirectX::XMFLOAT4X4 boneTransforms[MAX_BONES]
        {
            {
                1,0,0,0,
                0,1,0,0,
                0,0,1,0,
                0,0,0,1
            }
        };

        // EMISSIVE
        DirectX::XMFLOAT4 emissiveColor{ 0.0f, 0.0f, 0.0f, 1.0f };
        float emissiveIntensity = 3.0f;
        float emissiveOptions = 0.0f;  // emissiveTextureUVScrollSpeed
        DirectX::XMFLOAT2 emissiveScrollDirection{ 1.0f,0.0f };

        DirectX::XMFLOAT4 sageColor{ 1.0f,1.0f,1.0f,1.0f };
    };

    // メッシュ
    struct Mesh
    {
        uint64_t uniqueId{ 0 };
        std::string name;
        // シーンのノード配列を参照するIndex
        int64_t nodeIndex{ 0 };

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // サブセット
        struct Subset
        {
            uint64_t materialUniqueId{ 0 };
            std::string materialName;

            uint32_t startIndexLocation{ 0 };
            uint32_t indexCount{ 0 };

            // serialize
            template<class T>
            void serialize(T& archive)
            {
                archive(materialUniqueId, materialName, startIndexLocation, indexCount);
            }
        };
        std::vector<Subset> subsets;

        // シーン内の位置・姿勢・スケール情報
        DirectX::XMFLOAT4X4 defaultGlobalTransform
        {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
        };

        // スケルトン
        Skeleton bindPose;

        // バウンディングボックス
        DirectX::XMFLOAT3 boundingBox[2]
        {
            { +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX },
            { -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX }
        };

        // serialize
        template<class T>
        void serialize(T& archive)
        {
            archive(uniqueId, name, nodeIndex, subsets, defaultGlobalTransform,
                bindPose, boundingBox, vertices, indices);
        }

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
        friend class Model;
    };
    std::vector<Mesh> meshes;

    // マテリアル
    struct Material
    {
        uint64_t uniqueId{ 0 };
        std::string name;

        DirectX::XMFLOAT4 Ka{ 0.2f, 0.2f, 0.2f, 1.0f };
        DirectX::XMFLOAT4 Kd{ 0.8f, 0.8f, 0.8f, 1.0f };
        DirectX::XMFLOAT4 Ks{ 1.0f, 1.0f, 1.0f, 1.0f };
        // EMISSIVE
        DirectX::XMFLOAT4 Ke{ 1.0f, 1.0f ,1.0f ,1.0f };

        std::string textureFilenames[4];
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[4];

        // serialize
        template<class T>
        void serialize(T& archive)
        {
            archive(uniqueId, name, Ka, Kd, Ks, Ke/* EMISSIVE */ ,textureFilenames);
        }
    };
    std::unordered_map<uint64_t, Material> materials;
    // unordered_mapは、同一キーの要素を複数格納できず、格納準が規定されていないコンテナ

    // アニメーション
    std::vector<Animation> animationClips;

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;   // 頂点シェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;     // ピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;     // 入力レイアウト
    
    std::unique_ptr<ConstantBuffer<Constants>> constant_;


    void CreateComObjects(ID3D11Device* device, const char* fbxFileName);

public:
    Model(ID3D11Device* device, const char* fbxFileName, bool triangulate = false, float samplingRate = 0);
    Model(ID3D11Device* device, const char* fbxFileName, std::vector<std::string>& animationFileNames, bool triangulate = false, float samplingRate = 0);
    virtual ~Model() = default;



    void Render(ID3D11DeviceContext* deviceContext,
        const DirectX::XMFLOAT4X4 world, const DirectX::XMFLOAT4& materialColor,
        const Animation::KeyFrame* keyFrame,
        ID3D11PixelShader* psShader = nullptr);

    void UpdateAnimation(Animation::KeyFrame& keyframe);
    
    bool AppendAnimations(const char* animationFileName, float samplingRate);
    void BlendAnimations(const Animation::KeyFrame* keyFrames[2], float factor, Animation::KeyFrame& keyframe);
    void DrawDebug();


    //// ZELDA
    //DirectX::XMFLOAT3 DetectJointPosition(const char* boneName, const animation::keyframe* keyframe)const
    //{
    //    for (const mesh& mesh : meshes)
    //    {
    //        for (const skeleton::bone& bone : mesh.bind_pose.bones)
    //        {
    //            if (bone.name != boneName) continue;

    //            const animation::keyframe::node& node = keyframe->nodes.at(bone.node_index);
    //            DirectX::XMFLOAT3 jointPosition = { node.global_transform._41,node.global_transform._42,node.global_transform._43 };
    //            return jointPosition;
    //        }
    //    }
    //    _ASSERT_EXPR(FALSE, L"Could not find a joint by 'bone-name'.");
    //    return { 0,0,0 };
    //}

protected:
    Scene sceneView;
    
    void FetchMeshes(FbxScene* fbxScene, std::vector<Mesh>& meshes);
    void FetchMaterials(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials);
    void FetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose);
    void FetchAnimations(FbxScene* fbxScene, std::vector<Animation>& animationClips, float samplingRate/*値が０の場合アニメーションデータのデフォルトのフレームレートでサンプリングされる*/);
    void FetchScene(const char* fbxFileName, bool triangulate, float samplingRate/*値が０の場合アニメーションデータのデフォルトのフレームレートでサンプリングされる*/);

public:
    // BOUNDING_BOX
    DirectX::XMFLOAT3 boundingBox[2]
    {
        { +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX },
        { -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX }
    };


private:
    // BOUNDING_BOX
    void ComputeBoundingBox()
    {
        // Calculate a bounding box surrounding all meshes. In addition, coordinate values of all bounding boxes are converted to global space.
        for (Mesh& mesh : meshes)
        {
            DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform);
            DirectX::XMStoreFloat3(&mesh.boundingBox[0], DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&mesh.boundingBox[0]), M));
            DirectX::XMStoreFloat3(&mesh.boundingBox[1], DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&mesh.boundingBox[1]), M));

            if (mesh.boundingBox[0].x > mesh.boundingBox[1].x) std::swap<float>(mesh.boundingBox[0].x, mesh.boundingBox[1].x);
            if (mesh.boundingBox[0].y > mesh.boundingBox[1].y) std::swap<float>(mesh.boundingBox[0].y, mesh.boundingBox[1].y);
            if (mesh.boundingBox[0].z > mesh.boundingBox[1].z) std::swap<float>(mesh.boundingBox[0].z, mesh.boundingBox[1].z);

            boundingBox[0].x = std::min<float>(mesh.boundingBox[0].x, boundingBox[0].x);
            boundingBox[0].y = std::min<float>(mesh.boundingBox[0].y, boundingBox[0].y);
            boundingBox[0].z = std::min<float>(mesh.boundingBox[0].z, boundingBox[0].z);
            boundingBox[1].x = std::max<float>(mesh.boundingBox[1].x, boundingBox[1].x);
            boundingBox[1].y = std::max<float>(mesh.boundingBox[1].y, boundingBox[1].y);
            boundingBox[1].z = std::max<float>(mesh.boundingBox[1].z, boundingBox[1].z);
        }
    }

private:
    //メッシュ情報をunique_idで検索するためのリスト
    std::unordered_map<uint64_t, FbxNode*> nodeList;

public:
    // JOINT_POSITION
    DirectX::XMFLOAT3 JointPosition(const std::string& meshName, const std::string& boneName, const Animation::KeyFrame* keyFrame, const DirectX::XMFLOAT4X4& transform);
    DirectX::XMFLOAT3 JointPosition(size_t meshIndex, size_t boneIndex, const Animation::KeyFrame* keyframe, const DirectX::XMFLOAT4X4& transform);
};

