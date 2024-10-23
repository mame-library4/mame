#include "GltfModel.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERANL_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT

#include "../../External/tinygltf/tiny_gltf.h"
#include <stack>
#include <filesystem>
#include "Misc.h"
#include "Texture.h"
#include "Graphics.h"

#include "MathHelper.h"

#define USE_SERIALIZE 1

// ----- コンストラクタ -----
GltfModel::GltfModel(const std::string& filename)
    : filename_(filename)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

#if USE_SERIALIZE
    std::filesystem::path cerealFilename(filename);
    cerealFilename.replace_extension("cereal");
    if (std::filesystem::exists(cerealFilename.c_str()))
    {
        std::ifstream ifs(cerealFilename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(scenes_, nodes_, meshes_, materials_, textures_, images_, skins_, animations_);

        HRESULT result;
        D3D11_TEXTURE2D_DESC texture2dDesc;
        for (int i = 0; i < images_.size(); ++i)
        {
            ID3D11ShaderResourceView* shaderResourceView{};
            std::wstring fileName = images_.at(i).filename_;
            result = Texture::Instance().LoadTexture(fileName.c_str(), &shaderResourceView, &texture2dDesc);
            if (result == S_OK)
            {
                textureResourceViews_.emplace_back().Attach(shaderResourceView);
            }
        }

        for (int meshIndex = 0; meshIndex < meshes_.size(); ++meshIndex)
        {
            for (int primitiveIndex = 0; primitiveIndex < meshes_.at(meshIndex).primitives_.size(); ++primitiveIndex)
            {
                const BufferView& indexBufferView = meshes_.at(meshIndex).primitives_.at(primitiveIndex).indexBufferView_;
                D3D11_BUFFER_DESC bufferDesc = {};
                bufferDesc.ByteWidth = static_cast<UINT>(indexBufferView.sizeInBytes_);
                bufferDesc.Usage = D3D11_USAGE_DEFAULT;
                bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                D3D11_SUBRESOURCE_DATA subresourceData = {};
                subresourceData.pSysMem = indexBufferView.verticesBinary_.data();
                result = device->CreateBuffer(&bufferDesc, &subresourceData,
                    meshes_.at(meshIndex).primitives_.at(primitiveIndex).indexBufferView_.buffer_.ReleaseAndGetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

                for (auto& vertexBufferView : meshes_.at(meshIndex).primitives_.at(primitiveIndex).vertexBufferViews_)
                {
                    if (static_cast<UINT>(vertexBufferView.second.sizeInBytes_) == 0)
                    {
                        continue;
                    }
                    bufferDesc.ByteWidth = static_cast<UINT>(vertexBufferView.second.sizeInBytes_);
                    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
                    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                    subresourceData.pSysMem = vertexBufferView.second.verticesBinary_.data();
                    result = device->CreateBuffer(&bufferDesc, &subresourceData,
                        vertexBufferView.second.buffer_.ReleaseAndGetAddressOf());
                    if (FAILED(result))
                    {
                        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

                    }

                }
            }
        }

        std::vector<Material::Cbuffer> materialData;
        for (std::vector<Material>::const_reference material : materials_)
        {
            materialData.emplace_back(material.data_);
        }
        Microsoft::WRL::ComPtr<ID3D11Buffer> materialBuffer;
        D3D11_BUFFER_DESC bufferDesc{};
        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Material::Cbuffer) * materialData.size());
        bufferDesc.StructureByteStride = sizeof(Material::Cbuffer);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        D3D11_SUBRESOURCE_DATA subresourceData{};
        subresourceData.pSysMem = materialData.data();
        result = device->CreateBuffer(&bufferDesc, &subresourceData, materialBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
        shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
        shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        shaderResourceViewDesc.Buffer.NumElements = static_cast<UINT>(materialData.size());
        result = device->CreateShaderResourceView(materialBuffer.Get(),
            &shaderResourceViewDesc, materialResourceView_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
    }
    else
    {
#endif
        tinygltf::Model gltfModel;

        tinygltf::TinyGLTF tinyGltf;
        std::string error, warning;
        bool succeeded{ false };
        if (filename.find(".glb") != std::string::npos)
        {
            succeeded = tinyGltf.LoadBinaryFromFile(&gltfModel, &error, &warning, filename.c_str());
        }
        else if (filename.find(".gltf") != std::string::npos)
        {
            succeeded = tinyGltf.LoadASCIIFromFile(&gltfModel, &error, &warning, filename.c_str());
        }

        _ASSERT_EXPR_A(warning.empty(), warning.c_str());
        _ASSERT_EXPR_A(error.empty(), warning.c_str());
        _ASSERT_EXPR_A(succeeded, L"Failed to load glTF file");
        for (std::vector<tinygltf::Scene>::const_reference gltfScene : gltfModel.scenes)
        {
            Scene& scene{ scenes_.emplace_back() };
            scene.name_ = gltfModel.scenes.at(0).name;
            scene.nodes_ = gltfModel.scenes.at(0).nodes;
        }

        FetchNodes(gltfModel);
        FetchMeshes(device, gltfModel);
        FetchMatetials(device, gltfModel);
        FetchTexture(device, gltfModel);
        FetchAnimation(gltfModel);

#if USE_SERIALIZE
        std::ofstream ofs(cerealFilename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        serialization(scenes_, nodes_, meshes_, materials_, textures_, images_, skins_, animations_);

    }



#endif

    const std::map<std::string, BufferView>& vertexBufferViews{
        meshes_.at(0).primitives_.at(0).vertexBufferViews_ };
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
    {
        { "POSITION", 0, vertexBufferViews.at("POSITION").format_,   0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, vertexBufferViews.at("NORMAL").format_,     1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0, vertexBufferViews.at("TANGENT").format_,    2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, vertexBufferViews.at("TEXCOORD_0").format_, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "JOINTS",   0, vertexBufferViews.at("JOINTS_0").format_,   4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "WEIGHTS",  0, vertexBufferViews.at("WEIGHTS_0").format_,  5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "JOINTS",   1, vertexBufferViews.at("JOINTS_1").format_,   6, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "WEIGHTS",  1, vertexBufferViews.at("WEIGHTS_1").format_,  7, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    Graphics::Instance().CreateVsFromCso("./Resources/Shader/gltfModelVs.cso", vertexShader_.ReleaseAndGetAddressOf(),
        inputLayout_.ReleaseAndGetAddressOf(), inputElementDesc, _countof(inputElementDesc));
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/gltfModelPs.cso", pixelShader_.ReleaseAndGetAddressOf());

    Graphics::Instance().CreateVsFromCso("./Resources/Shader/GltfModelShadowVS.cso", shadowVertexShader_.ReleaseAndGetAddressOf(), NULL, NULL, 0);
    Graphics::Instance().CreateGsFromCso("./Resources/Shader/GltfModelShadowGS.cso", shadowGeometryShader_.ReleaseAndGetAddressOf());


    // 定数バッファ生成
    {
        primitiveConstants_ = std::make_unique<ConstantBuffer<PrimitiveConstants>>();

        jointConstants_ = std::make_unique<ConstantBuffer<JointConstants>>();
    }

    animatedNodes_[0] = nodes_;
    animatedNodes_[1] = nodes_;
    blendedAnimationNodes_ = nodes_;
    upperLowerBodyAnimatedNodes_[0] = nodes_;
    upperLowerBodyAnimatedNodes_[1] = nodes_;

    zeroAnimatedNodes_ = nodes_;
}

// ----- アニメーション更新 -----
void GltfModel::UpdateAnimation(const float& elapsedTime)
{
    // アニメーション再生してなかったら処理しない
    if (!IsPlayAnimation()) return;

    // ブレンドアニメーション再生 ( ブレンドアニメーションの場合はここで終了 )
    if (UpdateBlendAnimation(elapsedTime)) return;

    if (isBlendUpperLowerBodyAnimation_)
    {
        weight_ = upperLowerBodyBlendAnimationSeconds_ / transitionTime_;

        // 下半身アニメーション更新
#if 0
        // アニメーション再生時間経過
        upperLowerBodyAnimationSeconds_ += elapsedTime;
        // アニメーションの最終フレームを取ってくる
        float duration = animations_.at(upperLowerBodyAnimationIndex_).duration_;
        // アニメーションが再生しきっている場合
        if (upperLowerBodyAnimationSeconds_ > duration)
        {
            upperLowerBodyAnimationSeconds_ = 0.0f;
        }
        Animate(upperLowerBodyAnimationIndex_, upperLowerBodyAnimationSeconds_, upperLowerBodyAnimatedNodes_[1]);
        std::vector<Node> lowerNodes = upperLowerBodyAnimatedNodes_[1];
        Animate(animationIndex_, animationSeconds_, lowerNodes);
        // 合体させる
        upperLowerBodyAnimatedNodes_[1] = SetUpperLowerBodyAnimation(upperLowerBodyAnimatedNodes_[1], lowerNodes);
#endif
        const std::vector<Node>* nodes[2] = { &upperLowerBodyAnimatedNodes_[0], &upperLowerBodyAnimatedNodes_[1] };
        BlendAnimations(nodes, weight_, blendedAnimationNodes_);
        nodes_ = blendedAnimationNodes_;

        // アニメーション再生時間更新
        upperLowerBodyBlendAnimationSeconds_ += elapsedTime;

        if (weight_ > 1.0f)
        {
            upperLowerBodyBlendAnimationSeconds_ = 0.0f;
            isBlendUpperLowerBodyAnimation_ = false;
        }

        return;
    }

    // アニメーション再生時間経過
    animationSeconds_ += elapsedTime * animationSpeed_;

    // アニメーションの最終フレームを取ってくる
    float duration = animations_.at(animationIndex_).duration_;

    // アニメーションが再生しきっている場合
    if (animationSeconds_ > duration)
    {
        if (animationLoopFlag_)
        {
            animationSeconds_ = 0.0f;
            isAnimationLooped_ = true;
            return;
        }
        else
        {
            animationEndFlag_ = true;
            return;
        }
    }

    // アニメーション更新
    Animate(animationIndex_, animationSeconds_, nodes_);

    // 上下半身アニメーション更新
    UpdateUpperLowerBodyAnimation(elapsedTime);
}

const bool GltfModel::IsPlayAnimation()
{
    if (isBlendAnimation_) return true;

    return !animationEndFlag_;
}

// ----- ブレンドアニメーション更新 ( 更新していたら true ) -----
bool GltfModel::UpdateBlendAnimation(const float& elapsedTime)
{
    if (isBlendAnimation_ == false) return false;

    weight_ = blendAnimationSeconds_ / transitionTime_;

    const std::vector<Node>* nodes[2] = { &animatedNodes_[0], &animatedNodes_[1] };
    BlendAnimations(nodes, weight_, blendedAnimationNodes_);
    nodes_ = blendedAnimationNodes_;

    // アニメーション再生時間更新
    blendAnimationSeconds_ += elapsedTime * animationSpeed_;

    if (weight_ > 1.0f)
    {
        blendAnimationSeconds_ = 0.0f;
        isBlendAnimation_ = false;
    }

    return true;
}

void GltfModel::BlendAnimations(const std::vector<Node>* nodes[2], float factor, std::vector<Node>& node)
{
    size_t nodeCount = nodes[0]->size();
    node.resize(nodeCount);
    for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        DirectX::XMVECTOR S[2] =
        {
            DirectX::XMLoadFloat3(&nodes[0]->at(nodeIndex).scale_),
            DirectX::XMLoadFloat3(&nodes[1]->at(nodeIndex).scale_)
        };
        DirectX::XMStoreFloat3(&node.at(nodeIndex).scale_, DirectX::XMVectorLerp(S[0], S[1], factor));

        DirectX::XMVECTOR R[2] =
        {
            DirectX::XMLoadFloat4(&nodes[0]->at(nodeIndex).rotation_),
            DirectX::XMLoadFloat4(&nodes[1]->at(nodeIndex).rotation_)
        };
        DirectX::XMStoreFloat4(&node.at(nodeIndex).rotation_, DirectX::XMQuaternionSlerp(R[0], R[1], factor));

        DirectX::XMVECTOR T[2] =
        {
            DirectX::XMLoadFloat3(&nodes[0]->at(nodeIndex).translation_),
            DirectX::XMLoadFloat3(&nodes[1]->at(nodeIndex).translation_),
        };
        DirectX::XMStoreFloat3(&node.at(nodeIndex).translation_, DirectX::XMVectorLerp(T[0], T[1], factor));
    }

    CumulateTransforms(node);
}


// ----- 描画 -----
void GltfModel::Render(const float& scaleFactor, ID3D11PixelShader* psShader)
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    DirectX::XMMATRIX W = transform_.CalcWorldMatrix(scaleFactor);
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, W);

    deviceContext->PSSetShaderResources(0, 1, materialResourceView_.GetAddressOf());
    deviceContext->VSSetShader(vertexShader_.Get(), nullptr, 0);
    psShader ? deviceContext->PSSetShader(psShader, nullptr, 0) : deviceContext->PSSetShader(pixelShader_.Get(), nullptr, 0);
    deviceContext->IASetInputLayout(inputLayout_.Get());
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    std::function<void(int)> traverse{ [&](int nodeIndex)->void {
        const Node& node{nodes_.at(nodeIndex)};
        if (node.mesh_ > -1)
        {
            const Mesh& mesh{ meshes_.at(node.mesh_) };
            for (std::vector<Mesh::Primitive>::const_reference primitive : mesh.primitives_)
            {
                ID3D11Buffer* vertexBuffers[]
                {
                    primitive.vertexBufferViews_.at("POSITION").buffer_.Get(),
                    primitive.vertexBufferViews_.at("NORMAL").buffer_.Get(),
                    primitive.vertexBufferViews_.at("TANGENT").buffer_.Get(),
                    primitive.vertexBufferViews_.at("TEXCOORD_0").buffer_.Get(),
                    primitive.vertexBufferViews_.at("JOINTS_0").buffer_.Get(),
                    primitive.vertexBufferViews_.at("WEIGHTS_0").buffer_.Get(),
                    primitive.vertexBufferViews_.at("JOINTS_1").buffer_.Get(),
                    primitive.vertexBufferViews_.at("WEIGHTS_1").buffer_.Get(),
                };
                UINT strides[]
                {
                    static_cast<UINT>(primitive.vertexBufferViews_.at("POSITION").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("NORMAL").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("TANGENT").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("TEXCOORD_0").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("JOINTS_0").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("WEIGHTS_0").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("JOINTS_1").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("WEIGHTS_1").strideInBytes_),
                };
                UINT offsets[_countof(vertexBuffers)]{ 0 };
                deviceContext->IASetVertexBuffers(0, _countof(vertexBuffers), vertexBuffers, strides, offsets);
                deviceContext->IASetIndexBuffer(primitive.indexBufferView_.buffer_.Get(),
                    primitive.indexBufferView_.format_, 0);

                primitiveConstants_->GetData()->material_ = primitive.material_;
                primitiveConstants_->GetData()->hasTangent_ = primitive.vertexBufferViews_.at("TANGENT").buffer_ != NULL;
                primitiveConstants_->GetData()->skin_ = node.skin_;

                DirectX::XMStoreFloat4x4(&primitiveConstants_->GetData()->world_,
                    DirectX::XMLoadFloat4x4(&node.globalTransform_) * DirectX::XMLoadFloat4x4(&world));                
                primitiveConstants_->Activate(0);

                // texture
                {
                    const Material& material{ materials_.at(primitive.material_) };
                    const int textureIndices[]
                    {
                        material.data_.pbrMetallicRoughness_.baseColorTexture_.index_,
                        material.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.index_,
                        material.data_.normalTexture_.index_,
                        material.data_.emissiveTexture_.index_,
                        material.data_.occlusionTexture_.index_,
                    };
                    ID3D11ShaderResourceView* nullShaderResourceView{};
                    std::vector<ID3D11ShaderResourceView*> shaderResourceViews(_countof(textureIndices));
                    for (int textureIndex = 0; textureIndex < shaderResourceViews.size(); ++textureIndex)
                    {
                        shaderResourceViews.at(textureIndex) = textureIndices[textureIndex] > -1 ?
                            textureResourceViews_.at(textures_.at(textureIndices[textureIndex]).source_).Get() :
                            nullShaderResourceView;
                    }
                    deviceContext->PSSetShaderResources(1, static_cast<UINT>(shaderResourceViews.size()),
                        shaderResourceViews.data());
                }

                // animation
                {
                    if (node.skin_ > -1)
                    {
                        const Skin& skin{ skins_.at(node.skin_) };
                        auto size__ = skin.joints_.size();
                        for (size_t jointIndex = 0; jointIndex < skin.joints_.size(); ++jointIndex)
                        {
                            DirectX::XMStoreFloat4x4(&jointConstants_->GetData()->matrices_[jointIndex],
                                DirectX::XMLoadFloat4x4(&skin.inverseBindMatrices_.at(jointIndex)) *
                                DirectX::XMLoadFloat4x4(&nodes_.at(skin.joints_.at(jointIndex)).globalTransform_) *
                                DirectX::XMMatrixInverse(NULL, DirectX::XMLoadFloat4x4(&node.globalTransform_))
                            );
                        }
                        jointConstants_->Activate(2);
                    }
                }

                deviceContext->DrawIndexed(static_cast<UINT>(primitive.indexBufferView_.count()), 0, 0);
            }
        }
        for (std::vector<int>::value_type childIndex : node.children_)
        {
            traverse(childIndex);
        }
    } };
    for (std::vector<int>::value_type nodeIndex : scenes_.at(0).nodes_)
    {
        traverse(nodeIndex);
    }
}

void GltfModel::Render(const DirectX::XMFLOAT4X4 world, ID3D11PixelShader* psShader)
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();


    deviceContext->PSSetShaderResources(0, 1, materialResourceView_.GetAddressOf());
    deviceContext->VSSetShader(vertexShader_.Get(), nullptr, 0);
    psShader ? deviceContext->PSSetShader(psShader, nullptr, 0) : deviceContext->PSSetShader(pixelShader_.Get(), nullptr, 0);
    deviceContext->IASetInputLayout(inputLayout_.Get());
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    std::function<void(int)> traverse{ [&](int nodeIndex)->void {
        const Node& node{nodes_.at(nodeIndex)};
        if (node.mesh_ > -1)
        {
            const Mesh& mesh{ meshes_.at(node.mesh_) };
            for (std::vector<Mesh::Primitive>::const_reference primitive : mesh.primitives_)
            {
                ID3D11Buffer* vertexBuffers[]
                {
                    primitive.vertexBufferViews_.at("POSITION").buffer_.Get(),
                    primitive.vertexBufferViews_.at("NORMAL").buffer_.Get(),
                    primitive.vertexBufferViews_.at("TANGENT").buffer_.Get(),
                    primitive.vertexBufferViews_.at("TEXCOORD_0").buffer_.Get(),
                    primitive.vertexBufferViews_.at("JOINTS_0").buffer_.Get(),
                    primitive.vertexBufferViews_.at("WEIGHTS_0").buffer_.Get(),
                    primitive.vertexBufferViews_.at("JOINTS_1").buffer_.Get(),
                    primitive.vertexBufferViews_.at("WEIGHTS_1").buffer_.Get(),
                };
                UINT strides[]
                {
                    static_cast<UINT>(primitive.vertexBufferViews_.at("POSITION").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("NORMAL").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("TANGENT").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("TEXCOORD_0").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("JOINTS_0").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("WEIGHTS_0").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("JOINTS_1").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("WEIGHTS_1").strideInBytes_),
                };
                UINT offsets[_countof(vertexBuffers)]{ 0 };
                deviceContext->IASetVertexBuffers(0, _countof(vertexBuffers), vertexBuffers, strides, offsets);
                deviceContext->IASetIndexBuffer(primitive.indexBufferView_.buffer_.Get(),
                    primitive.indexBufferView_.format_, 0);

                primitiveConstants_->GetData()->material_ = primitive.material_;
                primitiveConstants_->GetData()->hasTangent_ = primitive.vertexBufferViews_.at("TANGENT").buffer_ != NULL;
                primitiveConstants_->GetData()->skin_ = node.skin_;

                DirectX::XMStoreFloat4x4(&primitiveConstants_->GetData()->world_,
                    DirectX::XMLoadFloat4x4(&node.globalTransform_) * DirectX::XMLoadFloat4x4(&world));
                primitiveConstants_->Activate(0);

                // texture
                {
                    const Material& material{ materials_.at(primitive.material_) };
                    const int textureIndices[]
                    {
                        material.data_.pbrMetallicRoughness_.baseColorTexture_.index_,
                        material.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.index_,
                        material.data_.normalTexture_.index_,
                        material.data_.emissiveTexture_.index_,
                        material.data_.occlusionTexture_.index_,
                    };
                    ID3D11ShaderResourceView* nullShaderResourceView{};
                    std::vector<ID3D11ShaderResourceView*> shaderResourceViews(_countof(textureIndices));
                    for (int textureIndex = 0; textureIndex < shaderResourceViews.size(); ++textureIndex)
                    {
                        shaderResourceViews.at(textureIndex) = textureIndices[textureIndex] > -1 ?
                            textureResourceViews_.at(textures_.at(textureIndices[textureIndex]).source_).Get() :
                            nullShaderResourceView;
                    }
                    deviceContext->PSSetShaderResources(1, static_cast<UINT>(shaderResourceViews.size()),
                        shaderResourceViews.data());
                }

                // animation
                {
                    if (node.skin_ > -1)
                    {
                        const Skin& skin{ skins_.at(node.skin_) };
                        auto size__ = skin.joints_.size();
                        for (size_t jointIndex = 0; jointIndex < skin.joints_.size(); ++jointIndex)
                        {
                            DirectX::XMStoreFloat4x4(&jointConstants_->GetData()->matrices_[jointIndex],
                                DirectX::XMLoadFloat4x4(&skin.inverseBindMatrices_.at(jointIndex)) *
                                DirectX::XMLoadFloat4x4(&nodes_.at(skin.joints_.at(jointIndex)).globalTransform_) *
                                DirectX::XMMatrixInverse(NULL, DirectX::XMLoadFloat4x4(&node.globalTransform_))
                            );
                        }
                        jointConstants_->Activate(2);
                    }
                }

                deviceContext->DrawIndexed(static_cast<UINT>(primitive.indexBufferView_.count()), 0, 0);
            }
        }
        for (std::vector<int>::value_type childIndex : node.children_)
        {
            traverse(childIndex);
        }
    } };
    for (std::vector<int>::value_type nodeIndex : scenes_.at(0).nodes_)
    {
        traverse(nodeIndex);
    }
}

void GltfModel::CastShadow(const float& scaleFactor)
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    DirectX::XMMATRIX W = transform_.CalcWorldMatrix(scaleFactor);

    deviceContext->VSSetShader(shadowVertexShader_.Get(), NULL, 0);
    deviceContext->GSSetShader(shadowGeometryShader_.Get(), NULL, 0);
    deviceContext->PSSetShader(NULL, NULL, 0);
    deviceContext->IASetInputLayout(inputLayout_.Get());    

    std::function<void(int)> traverse = [&](int nodeIndex)
    {
        const Node& node = nodes_.at(nodeIndex);
        DirectX::XMMATRIX globalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform_);

        if (node.skin_ > -1)
        {
            const Skin& skin = skins_.at(node.skin_);
            _ASSERT_EXPR(skin.joints_.size() <= maxJoints_, L"The size of the joint array is insufficient, please expand it.");
            for (size_t jointIndex = 0; jointIndex < skin.joints_.size(); ++jointIndex)
            {
                DirectX::XMStoreFloat4x4(&jointConstants_->GetData()->matrices_[jointIndex],
                    DirectX::XMLoadFloat4x4(&skin.inverseBindMatrices_.at(jointIndex)) *
                    DirectX::XMLoadFloat4x4(&nodes_.at(skin.joints_.at(jointIndex)).globalTransform_) *
                    DirectX::XMMatrixInverse(NULL, globalTransform)
                );
                jointConstants_->Activate(2);
            }
        }
        if (node.mesh_ > -1)
        {
            const Mesh& mesh = meshes_.at(node.mesh_);

            for (std::vector<Mesh::Primitive>::const_reference primitive : mesh.primitives_)
            {
                ID3D11Buffer* vertexBuffers[]
                {
                    primitive.vertexBufferViews_.at("POSITION").buffer_.Get(),
                    primitive.vertexBufferViews_.at("NORMAL").buffer_.Get(),
                    primitive.vertexBufferViews_.at("TANGENT").buffer_.Get(),
                    primitive.vertexBufferViews_.at("TEXCOORD_0").buffer_.Get(),
                    primitive.vertexBufferViews_.at("JOINTS_0").buffer_.Get(),
                    primitive.vertexBufferViews_.at("WEIGHTS_0").buffer_.Get(),
                    primitive.vertexBufferViews_.at("JOINTS_1").buffer_.Get(),
                    primitive.vertexBufferViews_.at("WEIGHTS_1").buffer_.Get(),
                };
                UINT strides[]
                {
                    static_cast<UINT>(primitive.vertexBufferViews_.at("POSITION").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("NORMAL").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("TANGENT").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("TEXCOORD_0").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("JOINTS_0").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("WEIGHTS_0").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("JOINTS_1").strideInBytes_),
                    static_cast<UINT>(primitive.vertexBufferViews_.at("WEIGHTS_1").strideInBytes_),
                };

                UINT offsets[_countof(vertexBuffers)]{ 0 };
                deviceContext->IASetVertexBuffers(0, _countof(vertexBuffers), vertexBuffers, strides, offsets);
                deviceContext->IASetIndexBuffer(primitive.indexBufferView_.buffer_.Get(),
                    primitive.indexBufferView_.format_, 0);

                DirectX::XMStoreFloat4x4(&primitiveConstants_->GetData()->world_, globalTransform * W);
                primitiveConstants_->GetData()->skin_ = node.skin_;
                primitiveConstants_->GetData()->startInstanceLocation_ = 0;
                primitiveConstants_->Activate(0);

                deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                deviceContext->DrawIndexedInstanced(static_cast<UINT>(primitive.indexBufferView_.count()), 4, 0, 0, 0);
            }
        }
        for (std::vector<int>::value_type childIndex : node.children_)
        {
            traverse(childIndex);
        }
    };
    for (std::vector<int>::value_type nodeIndex : scenes_.at(0).nodes_)
    {
        traverse(nodeIndex);
    }

    deviceContext->VSSetShader(NULL, NULL, 0);
    deviceContext->PSSetShader(NULL, NULL, 0);
    deviceContext->GSSetShader(NULL, NULL, 0);
    deviceContext->IASetInputLayout(NULL);
}

void GltfModel::DrawDebug()
{
    GetTransform()->DrawDebug();
    if (ImGui::TreeNode("Animation"))
    {
        ImGui::DragInt("AnimationIndex", &animationIndex_);
        ImGui::DragFloat("AnimationSeconds", &animationSeconds_);
        ImGui::DragFloat("AnimationSpeed", &animationSpeed_);

        ImGui::NewLine();

        ImGui::DragFloat("Weight", &weight_);
        ImGui::DragFloat("TransitionTime", &transitionTime_);
        ImGui::Checkbox("IsBlendAnimation", &isBlendAnimation_);

        ImGui::Checkbox("AnimationLoopFlag", &animationLoopFlag_);
        ImGui::Checkbox("AnimationEndFlag", &animationEndFlag_);
        ImGui::TreePop();
    }
}

void GltfModel::Animate(size_t animationIndex, float time, std::vector<Node>& animatedNodes)
{
    std::function<size_t(const std::vector<float>&, float, float&)> indexof
    {
        [](const std::vector<float>& timelines, float time, float& interpolationFactor)->size_t
        {
            const size_t keyframeCount{timelines.size()};

            if (time > timelines.at(keyframeCount - 1))
            {
                interpolationFactor = 1.0f;
                return keyframeCount - 2;
            }
            else if (time < timelines.at(0))
            {
                //interpolationFactor = 0.0f;
                interpolationFactor = timelines.at(0);
                return 0;
            }
            size_t keyframeIndex{ 0 };
            for (size_t timeIndex = 1; timeIndex < keyframeCount; ++timeIndex)
            {
                if (time < timelines.at(timeIndex))
                {
                    keyframeIndex = std::max<size_t>(0LL, timeIndex - 1);
                    break;
                }
            }
            interpolationFactor = (time - timelines.at(keyframeIndex + 0)) /
                (timelines.at(keyframeIndex + 1) - timelines.at(keyframeIndex + 0));
            return keyframeIndex;
        }
    };

    if (animations_.size() > 0)
    {
        const Animation& animation{ animations_.at(animationIndex) };
        for (std::vector<Animation::Channel>::const_reference channel : animation.channels_)
        {
            const Animation::Sampler& sampler{ animation.samplers_.at(channel.sampler_) };
            const std::vector<float>& timeline{ animation.timelines_.at(sampler.input_) };
            if (timeline.size() == 0)
            {
                continue;
            }
            float interpolationFactor{};

            size_t keyframeIndex{ indexof(timeline, time, interpolationFactor) };

            if (channel.targetPath_ == "scale")
            {
                const std::vector<DirectX::XMFLOAT3>& scales{ animation.scales_.at(sampler.output_) };
                DirectX::XMStoreFloat3(&animatedNodes.at(channel.targetNode_).scale_,
                    DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&scales.at(keyframeIndex + 0)),
                        DirectX::XMLoadFloat3(&scales.at(keyframeIndex + 1)), interpolationFactor));
            }
            else if (channel.targetPath_ == "rotation")
            {
                const std::vector<DirectX::XMFLOAT4>& rotations{ animation.rotations_.at(sampler.output_) };
                DirectX::XMStoreFloat4(&animatedNodes.at(channel.targetNode_).rotation_,
                    DirectX::XMQuaternionNormalize(DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(&rotations.at(keyframeIndex + 0)),
                        DirectX::XMLoadFloat4(&rotations.at(keyframeIndex + 1)), interpolationFactor)));
            }
            else if (channel.targetPath_ == "translation")
            {
                const std::vector<DirectX::XMFLOAT3>& translations{ animation.translations_.at(sampler.output_) };
                DirectX::XMStoreFloat3(&animatedNodes.at(channel.targetNode_).translation_,
                    DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&translations.at(keyframeIndex + 0)),
                        DirectX::XMLoadFloat3(&translations.at(keyframeIndex + 1)), interpolationFactor));
            }
        }
        CumulateTransforms(animatedNodes);
    }
    else
    {
        animatedNodes = nodes_;
    }
}

void GltfModel::UpdateUpperLowerBodyAnimation(const float& elapsedTime)
{
    if (isUpperLowerBodyAnimation_ == false) return;

    // アニメーション再生時間経過
    upperLowerBodyAnimationSeconds_ += elapsedTime;

    // アニメーションの最終フレームを取ってくる
    float duration = animations_.at(upperLowerBodyAnimationIndex_).duration_;

    // アニメーションが再生しきっている場合
    if (upperLowerBodyAnimationSeconds_ > duration)
    {
        upperLowerBodyAnimationSeconds_ = 0.0f;
    }

    Animate(upperLowerBodyAnimationIndex_, upperLowerBodyAnimationSeconds_, upperLowerBodyAnimatedNodes_[0]);

    // ※プレイヤーのbone構造に対応
    nodes_ = SetUpperLowerBodyAnimation(nodes_, upperLowerBodyAnimatedNodes_[0]);
}

// ----- 下半身のアニメーションを上書きする -----
std::vector<GltfModel::Node> GltfModel::SetUpperLowerBodyAnimation(const std::vector<Node>& currentNodes, const std::vector<Node>& lowerNodes)
{
    std::vector<Node> tmpNodes = currentNodes;

    tmpNodes.at(2) = lowerNodes.at(2);
    tmpNodes.at(3) = lowerNodes.at(3);
    tmpNodes.at(4) = lowerNodes.at(4);
    for (int i = 52; i < 63; ++i)
    {
        tmpNodes.at(i) = lowerNodes.at(i);
    }
    CumulateTransforms(tmpNodes);

    return tmpNodes;
}

// ----- アニメーション再生 -----
void GltfModel::PlayAnimation(const int& index, const bool& loop, const float& speed)
{
    animationIndex_ = index;
    animationSeconds_ = 0.0f;

    animationLoopFlag_ = loop;     // アニメーションループフラグを設定する
    animationEndFlag_ = false;    // 再生終了フラグをリセット
    animationSpeed_ = speed;    // アニメーション再生速度を設定
    isAnimationLooped_ = false;
}

void GltfModel::PlayBlendAnimation(const int& index, const bool& loop, const float& speed, const float& blendAnimationFrame)
{
    // 現在ブレンドアニメーション中の場合
    if (isBlendAnimation_)
    {
        animatedNodes_[0] = nodes_;
    }
    else
    {
        Animate(animationIndex_, animationSeconds_, animatedNodes_[0]);
    }
    Animate(index, blendAnimationFrame, animatedNodes_[1]);

    animationIndex_ = index;

    animationSeconds_ = blendAnimationFrame;
    blendAnimationSeconds_ = 0.0f;
    weight_ = 0.0f;

    animationEndFlag_ = false;
    animationLoopFlag_ = loop;
    animationSpeed_ = speed;

    isBlendAnimation_ = true;
    isAnimationLooped_ = false;
}

// ----- 上下半身分離アニメーションを再生 -----
void GltfModel::PlayUpperLowerBodyAnimation(const int& index, const bool& loop, const float& startFrame)
{
    // 現在再生中のアニメーション
    Animate(animationIndex_, animationSeconds_, upperLowerBodyAnimatedNodes_[0]);

    // -----------------------------------
    //      ブレンド先のアニメーション
    // -----------------------------------
    // 上半身アニメーション
    Animate(index, startFrame, upperLowerBodyAnimatedNodes_[1]);
    // 下半身アニメーション
    std::vector<Node> lowerNodes = upperLowerBodyAnimatedNodes_[1];
    Animate(animationIndex_, animationSeconds_, lowerNodes);
    // 合体させる
    upperLowerBodyAnimatedNodes_[1] = SetUpperLowerBodyAnimation(upperLowerBodyAnimatedNodes_[1], lowerNodes);

    // 各種変数を設定する    
    upperLowerBodyAnimationIndex_   = animationIndex_;  // 下半身アニメーション番号
    animationIndex_                 = index;            // 上半身アニメーション番号
    upperLowerBodyAnimationSeconds_ = animationSeconds_;// 下半身アニメーションタイマー
    animationSeconds_               = startFrame;       // 上半身アニメーションタイマー
    isUpperLowerBodyAnimation_      = true;
    isBlendUpperLowerBodyAnimation_ = true;
    animationLoopFlag_              = loop;
}

// ----- 指定したジョイントの位置を取得 -----
DirectX::XMFLOAT3 GltfModel::GetJointPosition(const size_t& nodeIndex, const float& scaleFactor, const DirectX::XMFLOAT3& offsetPosition)
{
    DirectX::XMFLOAT3 position = offsetPosition;

    const Node& node = nodes_.at(nodeIndex);
    DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&node.globalTransform_) * GetTransform()->CalcWorldMatrix(scaleFactor);
    DirectX::XMStoreFloat3(&position, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&position), M));

    return position;
}

// ----- 指定したジョイントの位置を取得 -----
DirectX::XMFLOAT3 GltfModel::GetJointPosition(const std::string& nodeName, const float& scaleFactor, const DirectX::XMFLOAT3& offsetPosition)
{
    DirectX::XMFLOAT3 position = offsetPosition;

    // ノードを名前検索する
    for (Node& node : nodes_)
    {
        // 名前が一致しなかったら continue
        if (node.name_ != nodeName) continue;

        DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&node.globalTransform_) * GetTransform()->CalcWorldMatrix(scaleFactor);
        DirectX::XMStoreFloat3(&position, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&position), M));

        return position;
    }

    // 見つからなかった。
    return DirectX::XMFLOAT3(0, 0, 0);
}

DirectX::XMFLOAT3 GltfModel::GetJointPosition(const std::string& nodeName, const DirectX::XMFLOAT4X4& world)
{
    DirectX::XMFLOAT3 position = {};

    // ノードを名前検索する
    for (Node& node : nodes_)
    {
        // 名前が一致しなかったら continue
        if (node.name_ != nodeName) continue;

        DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&node.globalTransform_) * DirectX::XMLoadFloat4x4(&world);
        DirectX::XMStoreFloat3(&position, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&position), M));

        return position;
    }

    // 見つからなかった。
    return DirectX::XMFLOAT3(0, 0, 0);
}

DirectX::XMMATRIX GltfModel::GetJointGlobalTransform(const size_t& nodeIndex)
{
    if (nodeIndex < 0 || nodeIndex > nodes_.size()) return DirectX::XMMATRIX();

    const Node& node = nodes_.at(nodeIndex);

    return DirectX::XMLoadFloat4x4(&node.globalTransform_);
}

DirectX::XMMATRIX GltfModel::GetJointGlobalTransform(const std::string& nodeName)
{
    // ノードを名前検索する
    for (Node& node : nodes_)
    {
        // 名前が一致しなかったら continue
        if (node.name_ != nodeName) continue;

        return DirectX::XMLoadFloat4x4(&node.globalTransform_);
    }

    return DirectX::XMMATRIX();
}

DirectX::XMMATRIX GltfModel::GetJointWorldTransform(const std::string& nodeName, const float& scaleFactor)
{
    // ノードを名前検索する
    for (Node& node : nodes_)
    {
        // 名前が一致しなかったら continue
        if (node.name_ != nodeName) continue;

        DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&node.globalTransform_) * GetTransform()->CalcWorldMatrix(scaleFactor);

        return M;
    }

    return DirectX::XMMATRIX();
}

// ----- ノードのインデックスを取得 -----
const int GltfModel::GetNodeIndex(const std::string& nodeName)
{
    for (int nodeIndex = 0; nodeIndex < nodes_.size(); ++nodeIndex)
    {
        if (nodes_.at(nodeIndex).name_ == nodeName)
        {
            return nodeIndex;
        }
    }

    return -1;
}

// ----- RootMotion -----
void GltfModel::RootMotion(const float& scaleFacter)
{
    if (animationEndFlag_) return; // Animationが再生されていない

    // Rootの移動値がないので処理をしない
    if (useRootMotionMovement_ == false) return;

    // 初回時はnodes_を計算しなおしてあげる
    if(isFirstTimeRootMotion_)
    {
        Animate(animationIndex_, animationSeconds_, nodes_);
    }

    Node& node = nodes_.at(rootJointIndex_);

    if(isFirstTimeRootMotion_)
    {
        previousPosition_ = { node.globalTransform_._41, node.globalTransform_._42, node.globalTransform_._43 };
        isFirstTimeRootMotion_ = false;
    }

    DirectX::XMFLOAT3 position = { node.globalTransform_._41, node.globalTransform_._42, node.globalTransform_._43 };
    DirectX::XMFLOAT3 displacement = { position.x - previousPosition_.x, position.y - previousPosition_.y, position.z - previousPosition_.z };

    DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&GetTransform()->GetCoordinateSystemTransforms(Transform::CoordinateSystem::cRightYup)) * DirectX::XMMatrixScaling(scaleFacter, scaleFacter, scaleFacter);
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(GetTransform()->GetScale().x, GetTransform()->GetScale().y, GetTransform()->GetScale().z);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(GetTransform()->GetRotationX(), GetTransform()->GetRotationY(), GetTransform()->GetRotationZ());
    DirectX::XMStoreFloat3(&displacement, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&displacement), C * S * R));

    DirectX::XMFLOAT3 translation = GetTransform()->GetPosition();
    translation = translation + displacement * rootMotionSpeed_;
    GetTransform()->SetPosition(translation);

    node.globalTransform_._41 = zeroAnimatedNodes_.at(rootJointIndex_).globalTransform_._41;
    node.globalTransform_._42 = zeroAnimatedNodes_.at(rootJointIndex_).globalTransform_._42;
    node.globalTransform_._43 = zeroAnimatedNodes_.at(rootJointIndex_).globalTransform_._43;

    std::function<void(int, int)> traverse = [&](int parentIndex, int nodeIndex)
    {
        GltfModel::Node& node = GetNodes()->at(nodeIndex);
        if (parentIndex > -1)
        {
            DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale_.x, node.scale_.y, node.scale_.z);
            DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotation_.x, node.rotation_.y, node.rotation_.z, node.rotation_.w));
            DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translation_.x, node.translation_.y, node.translation_.z);
            DirectX::XMStoreFloat4x4(&node.globalTransform_, S * R * T * DirectX::XMLoadFloat4x4(&GetNodes()->at(parentIndex).globalTransform_));
        }
        for (int childIndex : node.children_)
        {
            traverse(nodeIndex, childIndex);
        }
    };
    traverse(-1, rootJointIndex_);

    previousPosition_ = position;
}

void GltfModel::SetUseRootMotion(const bool& flag)
{
    useRootMotionMovement_ = flag;
    isFirstTimeRootMotion_ = true;
}

// ノードデータ取得
void GltfModel::FetchNodes(const tinygltf::Model& gltfModel)
{
    for (std::vector<tinygltf::Node>::const_reference gltfNode : gltfModel.nodes)
    {
        Node& node{ nodes_.emplace_back() };
        node.name_ = gltfNode.name;
        node.skin_ = gltfNode.skin;
        node.mesh_ = gltfNode.mesh;
        node.children_ = gltfNode.children;

        // rootを見つけたらフラグを立てる
        node.isRootNode_ = false;
        if (gltfNode.name == "root")
        {
            node.isRootNode_ = true;
        }

        if (!gltfNode.matrix.empty())
        {
            DirectX::XMFLOAT4X4 matrix;
            for (size_t row = 0; row < 4; ++row)
            {
                for (size_t column = 0; column < 4; ++column)
                {
                    matrix(row, column) = static_cast<float>(gltfNode.matrix.at(4 * row + column));
                }
            }

            DirectX::XMVECTOR S, T, R;
            bool succeed = DirectX::XMMatrixDecompose(&S, &R, &T, DirectX::XMLoadFloat4x4(&matrix));
            _ASSERT_EXPR(succeed, L"Failed to decompose matrix");

            DirectX::XMStoreFloat3(&node.scale_, S);
            DirectX::XMStoreFloat4(&node.rotation_, R);
            DirectX::XMStoreFloat3(&node.translation_, T);
        }
        else
        {
            if (gltfNode.scale.size() > 0)
            {
                node.scale_.x = static_cast<float>(gltfNode.scale.at(0));
                node.scale_.y = static_cast<float>(gltfNode.scale.at(1));
                node.scale_.z = static_cast<float>(gltfNode.scale.at(2));
            }
            if (gltfNode.translation.size() > 0)
            {
                node.translation_.x = static_cast<float>(gltfNode.translation.at(0));
                node.translation_.y = static_cast<float>(gltfNode.translation.at(1));
                node.translation_.z = static_cast<float>(gltfNode.translation.at(2));
            }
            if (gltfNode.rotation.size() > 0)
            {
                node.rotation_.x = static_cast<float>(gltfNode.rotation.at(0));
                node.rotation_.y = static_cast<float>(gltfNode.rotation.at(1));
                node.rotation_.z = static_cast<float>(gltfNode.rotation.at(2));
                node.rotation_.w = static_cast<float>(gltfNode.rotation.at(3));
            }
        }
    }
    CumulateTransforms(nodes_);
}

// メッシュデータ取得
void GltfModel::FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltfModel)
{
    HRESULT hr;
    for (std::vector<tinygltf::Mesh>::const_reference gltfMesh : gltfModel.meshes)
    {
        Mesh& mesh{ meshes_.emplace_back() };
        mesh.name_ = gltfMesh.name;
        for (std::vector<tinygltf::Primitive>::const_reference gltfPrimitive : gltfMesh.primitives)
        {
            Mesh::Primitive& primitive{ mesh.primitives_.emplace_back() };
            primitive.material_ = gltfPrimitive.material;

            // Create index buffer
            const tinygltf::Accessor& gltfAccessor{ gltfModel.accessors.at(gltfPrimitive.indices) };
            const tinygltf::BufferView& gltfBufferView{ gltfModel.bufferViews.at(gltfAccessor.bufferView) };

            primitive.indexBufferView_ = MakeBufferView(gltfAccessor);
            D3D11_BUFFER_DESC bufferDesc{};
            bufferDesc.ByteWidth = static_cast<UINT>(primitive.indexBufferView_.sizeInBytes_);
            bufferDesc.Usage = D3D11_USAGE_DEFAULT;
            bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            D3D11_SUBRESOURCE_DATA subresourceData{};
            subresourceData.pSysMem = gltfModel.buffers.at(gltfBufferView.buffer).data.data()
                + gltfBufferView.byteOffset + gltfAccessor.byteOffset;

            primitive.indexBufferView_.verticesBinary_.resize(bufferDesc.ByteWidth);
            memcpy(primitive.indexBufferView_.verticesBinary_.data(), subresourceData.pSysMem, bufferDesc.ByteWidth);
            //primitive.indexBufferView_.bufferData_ = subresourceData.pSysMem;

            hr = device->CreateBuffer(&bufferDesc, &subresourceData,
                primitive.indexBufferView_.buffer_.ReleaseAndGetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

            // Create vertex buffers
            for (std::map<std::string, int>::const_reference gltfAttribute : gltfPrimitive.attributes)
            {
#if 0
                const tinygltf::Accessor& gltfAccessor{ gltfModel.accessors.at(gltfAttribute.second) };
                const tinygltf::BufferView& gltfBufferView{ gltfModel.bufferViews.at(gltfAccessor.bufferView) };

                BufferView vertexBufferView{ MakeBufferView(gltfAccessor) };

                D3D11_BUFFER_DESC bufferDesc{};
                bufferDesc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
                bufferDesc.Usage = D3D11_USAGE_DEFAULT;
                bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                D3D11_SUBRESOURCE_DATA subresourceData{};
                subresourceData.pSysMem = gltfModel.buffers.at(gltfBufferView.buffer).data.data()
                    + gltfBufferView.byteOffset + gltfAccessor.byteOffset;
                hr = device->CreateBuffer(&bufferDesc, &subresourceData,
                    vertexBufferView.buffer_.ReleaseAndGetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

                primitive.vertexBufferViews_.emplace(std::make_pair(gltfAttribute.first, vertexBufferView));
#else
                tinygltf::Accessor gltfAccessor{ gltfModel.accessors.at(gltfAttribute.second) };
                const tinygltf::BufferView& gltfBufferView{ gltfModel.bufferViews.at(gltfAccessor.bufferView) };

                const void* buffer = gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset;
                std::vector<USHORT> joints0;
                std::vector<FLOAT> weight0;
                if (gltfAttribute.first == "JOINTS_0")
                {
                    if (gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                    {
                        const BYTE* data = gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset;
                        for (size_t accessorIndex = 0; accessorIndex < gltfAccessor.count * 4; ++accessorIndex)
                        {
                            joints0.emplace_back(static_cast<USHORT>(data[accessorIndex]));
                        }
                        buffer = joints0.data();
                        gltfAccessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
                    }
                }
                else if (gltfAttribute.first == "JOINTS_1")
                {
                    continue;
                }
                else if (gltfAttribute.first == "WEIGHTS_0")
                {
                    if (gltfAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                    {
                        const BYTE* data = gltfModel.buffers.at(gltfBufferView.buffer).data.data() + gltfBufferView.byteOffset + gltfAccessor.byteOffset;
                        for (size_t accessorIndex = 0; accessorIndex < gltfAccessor.count * 4; ++accessorIndex)
                        {
                            weight0.emplace_back(static_cast<FLOAT>(data[accessorIndex]) / 0xFF);
                        }
                        buffer = weight0.data();
                        gltfAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
                    }
                }
                else if (gltfAttribute.first == "WEIGHTS_1")
                {
                    continue;
                }


                BufferView vertexBufferView{ MakeBufferView(gltfAccessor) };

                D3D11_BUFFER_DESC bufferDesc{};
                bufferDesc.ByteWidth = static_cast<UINT>(vertexBufferView.sizeInBytes_);
                bufferDesc.Usage = D3D11_USAGE_DEFAULT;
                bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                D3D11_SUBRESOURCE_DATA subresourceData{};
                subresourceData.pSysMem = buffer;

                //vertexBufferView.bufferData_ = buffer;
                vertexBufferView.verticesBinary_.resize(bufferDesc.ByteWidth);
                memcpy(vertexBufferView.verticesBinary_.data(), subresourceData.pSysMem, bufferDesc.ByteWidth);


                hr = device->CreateBuffer(&bufferDesc, &subresourceData,
                    vertexBufferView.buffer_.ReleaseAndGetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

                primitive.vertexBufferViews_.emplace(std::make_pair(gltfAttribute.first, vertexBufferView));
#endif
            }

            // Add dummy attributes if any are amissing.
            const std::unordered_map<std::string, BufferView> attributes{
                { "TANGENT", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
                { "TEXCOORD_0", { DXGI_FORMAT_R32G32_FLOAT } },
                { "JOINTS_0", { DXGI_FORMAT_R16G16B16A16_UINT } },
                { "WEIGHTS_0", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
                { "JOINTS_1", { DXGI_FORMAT_R16G16B16A16_UINT } },
                { "WEIGHTS_1", { DXGI_FORMAT_R32G32B32A32_FLOAT } },
            };

            for (std::unordered_map<std::string, BufferView>::const_reference attribute : attributes)
            {
                if (primitive.vertexBufferViews_.find(attribute.first) == primitive.vertexBufferViews_.end())
                {
                    primitive.vertexBufferViews_.insert(std::make_pair(attribute.first, attribute.second));
                }
            }
        }
    }
}

// マテリアルの取得
void GltfModel::FetchMatetials(ID3D11Device* device, const tinygltf::Model& gltfModel)
{
    for (std::vector<tinygltf::Material>::const_reference gltfMaterial : gltfModel.materials)
    {
        std::vector<Material>::reference material = materials_.emplace_back();

        material.name_ = gltfMaterial.name;

        material.data_.emissiveFactor_[0] = static_cast<float>(gltfMaterial.emissiveFactor.at(0));
        material.data_.emissiveFactor_[1] = static_cast<float>(gltfMaterial.emissiveFactor.at(1));
        material.data_.emissiveFactor_[2] = static_cast<float>(gltfMaterial.emissiveFactor.at(2));

        material.data_.alphaMode_ = gltfMaterial.alphaMode == "OPAQUE" ?
            0 : gltfMaterial.alphaMode == "MASK" ? 1 : gltfMaterial.alphaMode == "BLEND" ? 2 : 0;
        material.data_.alphaCutoff_ = static_cast<float>(gltfMaterial.alphaCutoff);
        material.data_.doubleSided_ = gltfMaterial.doubleSided ? 1 : 0;

        material.data_.pbrMetallicRoughness_.baseColorFactor_[0] =
            static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor.at(0));
        material.data_.pbrMetallicRoughness_.baseColorFactor_[1] =
            static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor.at(1));
        material.data_.pbrMetallicRoughness_.baseColorFactor_[2] =
            static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor.at(2));
        material.data_.pbrMetallicRoughness_.baseColorFactor_[3] =
            static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor.at(3));
        material.data_.pbrMetallicRoughness_.baseColorTexture_.index_ =
            gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;
        material.data_.pbrMetallicRoughness_.baseColorTexture_.texcoord_ =
            gltfMaterial.pbrMetallicRoughness.baseColorTexture.texCoord;
        material.data_.pbrMetallicRoughness_.metallicFactor_ =
            static_cast<float>(gltfMaterial.pbrMetallicRoughness.metallicFactor);
        material.data_.pbrMetallicRoughness_.roughnessFactor_ =
            static_cast<float>(gltfMaterial.pbrMetallicRoughness.roughnessFactor);
        material.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.index_ =
            gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
        material.data_.pbrMetallicRoughness_.metallicRoughnessTexture_.texcoord_ =
            gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.texCoord;

        material.data_.normalTexture_.index_ = gltfMaterial.normalTexture.index;
        material.data_.normalTexture_.texcoord_ = gltfMaterial.normalTexture.texCoord;
        material.data_.normalTexture_.scale_ = static_cast<float>(gltfMaterial.normalTexture.scale);

        material.data_.occlusionTexture_.index_ = gltfMaterial.occlusionTexture.index;
        material.data_.occlusionTexture_.texcoord_ = gltfMaterial.occlusionTexture.texCoord;
        material.data_.occlusionTexture_.strength_ =
            static_cast<float>(gltfMaterial.occlusionTexture.strength);

        material.data_.emissiveTexture_.index_ = gltfMaterial.emissiveTexture.index;
        material.data_.emissiveTexture_.texcoord_ = gltfMaterial.emissiveTexture.texCoord;
    }

    // Create material data as shader resource view on GPU
    std::vector<Material::Cbuffer> materialData;
    for (std::vector<Material>::const_reference material : materials_)
    {
        materialData.emplace_back(material.data_);
    }

    HRESULT hr;
    Microsoft::WRL::ComPtr<ID3D11Buffer> materialBuffer;
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Material::Cbuffer) * materialData.size());
    bufferDesc.StructureByteStride = sizeof(Material::Cbuffer);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    D3D11_SUBRESOURCE_DATA subresourceData{};
    subresourceData.pSysMem = materialData.data();
    hr = device->CreateBuffer(&bufferDesc, &subresourceData, materialBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
    shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    shaderResourceViewDesc.Buffer.NumElements = static_cast<UINT>(materialData.size());
    hr = device->CreateShaderResourceView(materialBuffer.Get(),
        &shaderResourceViewDesc, materialResourceView_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

// テクスチャの取得
void GltfModel::FetchTexture(ID3D11Device* device, const tinygltf::Model& gltfModel)
{
    HRESULT hr{ S_OK };

    for (const tinygltf::Texture& gltfTexture : gltfModel.textures)
    {
        TextureData& texture{ textures_.emplace_back() };
        texture.name_ = gltfTexture.name;
        texture.source_ = gltfTexture.source;
    }
    for (const tinygltf::Image& gltfImage : gltfModel.images)
    {
        Image& image{ images_.emplace_back() };
        image.name_ = gltfImage.name;
        image.width_ = gltfImage.width;
        image.height_ = gltfImage.height;
        image.component_ = gltfImage.component;
        image.bits_ = gltfImage.bits;
        image.pixelType_ = gltfImage.pixel_type;
        image.bufferView_ = gltfImage.bufferView;
        image.mimeType_ = gltfImage.mimeType;
        image.uri_ = gltfImage.uri;
        image.asIs_ = gltfImage.as_is;

        if (gltfImage.bufferView > -1)
        {
            const tinygltf::BufferView& bufferView{ gltfModel.bufferViews.at(gltfImage.bufferView) };
            const tinygltf::Buffer& buffer{ gltfModel.buffers.at(bufferView.buffer) };
            const byte* data = buffer.data.data() + bufferView.byteOffset;

            ID3D11ShaderResourceView* textureResourceView{};
            hr = Texture::Instance().LoadTexture(data, bufferView.byteLength, &textureResourceView);
            if (hr == S_OK)
            {
                textureResourceViews_.emplace_back().Attach(textureResourceView);
            }
        }
        else
        {
            const std::filesystem::path path(filename_);
            ID3D11ShaderResourceView* shaderResourceView{};
            D3D11_TEXTURE2D_DESC texture2dDesc;
            std::wstring filename
            {
                path.parent_path().concat(L"/").wstring() +
                std::wstring(gltfImage.uri.begin(),gltfImage.uri.end())
            };
            image.filename_ = filename;
            hr = Texture::Instance().LoadTexture(filename.c_str(), &shaderResourceView, &texture2dDesc);
            if (hr == S_OK)
            {
                textureResourceViews_.emplace_back().Attach(shaderResourceView);
            }
        }
    }
}

// アニメーションの取得
void GltfModel::FetchAnimation(const tinygltf::Model& gltfModel)
{
    for (std::vector<tinygltf::Skin>::const_reference transmissionSkin : gltfModel.skins)
    {
        Skin& skin{ skins_.emplace_back() };
        const tinygltf::Accessor& gltfAccessor{ gltfModel.accessors.at(transmissionSkin.inverseBindMatrices) };
        const tinygltf::BufferView& gltfBufferView{ gltfModel.bufferViews.at(gltfAccessor.bufferView) };
        skin.inverseBindMatrices_.resize(gltfAccessor.count);
        std::memcpy(skin.inverseBindMatrices_.data(), gltfModel.buffers.at(gltfBufferView.buffer).data.data() +
            gltfBufferView.byteOffset + gltfAccessor.byteOffset, gltfAccessor.count * sizeof(DirectX::XMFLOAT4X4));
        skin.joints_ = transmissionSkin.joints;
    }

    for (std::vector<tinygltf::Animation>::const_reference gltfAnimation : gltfModel.animations)
    {
        Animation& animation{ animations_.emplace_back() };
        animation.name_ = gltfAnimation.name;
        for (std::vector<tinygltf::AnimationSampler>::const_reference gltfSampler : gltfAnimation.samplers)
        {
            Animation::Sampler& sampler{ animation.samplers_.emplace_back() };
            sampler.input_ = gltfSampler.input;
            sampler.output_ = gltfSampler.output;
            sampler.interpolation_ = gltfSampler.interpolation;

            const tinygltf::Accessor& gltfAccessor{ gltfModel.accessors.at(gltfSampler.input) };
            const tinygltf::BufferView& gltfBufferView{ gltfModel.bufferViews.at(gltfAccessor.bufferView) };
            std::pair<std::unordered_map<int, std::vector<float>>::iterator, bool>& timelines
            {
                animation.timelines_.emplace(gltfSampler.input, gltfAccessor.count)
            };
            if (timelines.second)
            {
                std::memcpy(timelines.first->second.data(), gltfModel.buffers.at(gltfBufferView.buffer).data.data() +
                    gltfBufferView.byteOffset + gltfAccessor.byteOffset, gltfAccessor.count * sizeof(FLOAT));
            }
        }

        for (std::vector<tinygltf::AnimationChannel>::const_reference gltfChannel : gltfAnimation.channels)
        {
            Animation::Channel& channel{ animation.channels_.emplace_back() };
            channel.sampler_ = gltfChannel.sampler;
            channel.targetNode_ = gltfChannel.target_node;
            channel.targetPath_ = gltfChannel.target_path;

            const tinygltf::AnimationSampler& gltfSampler{ gltfAnimation.samplers.at(gltfChannel.sampler) };
            const tinygltf::Accessor& gltfAccessor{ gltfModel.accessors.at(gltfSampler.output) };
            const tinygltf::BufferView& gltfBufferView{ gltfModel.bufferViews.at(gltfAccessor.bufferView) };
            if (gltfChannel.target_path == "scale")
            {
                std::pair<std::unordered_map<int, std::vector<DirectX::XMFLOAT3>>::iterator, bool>& scales
                {
                    animation.scales_.emplace(gltfSampler.output, gltfAccessor.count)
                };
                if (scales.second)
                {
                    std::memcpy(scales.first->second.data(), gltfModel.buffers.at(gltfBufferView.buffer).data.data() +
                        gltfBufferView.byteOffset + gltfAccessor.byteOffset, gltfAccessor.count * sizeof(DirectX::XMFLOAT3));
                }
            }
            else if (gltfChannel.target_path == "rotation")
            {
                std::pair<std::unordered_map<int, std::vector<DirectX::XMFLOAT4>>::iterator, bool>& rotations
                {
                    animation.rotations_.emplace(gltfSampler.output, gltfAccessor.count)
                };
                if (rotations.second)
                {
                    std::memcpy(rotations.first->second.data(), gltfModel.buffers.at(gltfBufferView.buffer).data.data() +
                        gltfBufferView.byteOffset + gltfAccessor.byteOffset, gltfAccessor.count * sizeof(DirectX::XMFLOAT4));
                }
            }
            else if (gltfChannel.target_path == "translation")
            {
                std::pair<std::unordered_map<int, std::vector<DirectX::XMFLOAT3>>::iterator, bool>& translations
                {
                    animation.translations_.emplace(gltfSampler.output,gltfAccessor.count)
                };
                if (translations.second)
                {
                    std::memcpy(translations.first->second.data(), gltfModel.buffers.at(gltfBufferView.buffer).data.data() +
                        gltfBufferView.byteOffset + gltfAccessor.byteOffset, gltfAccessor.count * sizeof(DirectX::XMFLOAT3));
                }
            }
        }
    }
    // 各チャンネルのタイムラインで最長のアニメーション時間を見つける
    for (decltype(animations_)::reference animation : animations_)
    {
        for (decltype(animation.timelines_)::reference timelines : animation.timelines_)
        {
            animation.duration_ = std::max<float>(animation.duration_, timelines.second.back());
        }
    }
}

void GltfModel::CumulateTransforms(std::vector<Node>& nodes)
{
    std::stack<DirectX::XMFLOAT4X4> parentGlobalTransforms;
    std::function<void(int)> traverse{ [&](int nodeIndex)->void
    {
        Node& node = nodes.at(nodeIndex);
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale_.x, node.scale_.y, node.scale_.z);
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotation_.x, node.rotation_.y, node.rotation_.z, node.rotation_.w));
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translation_.x, node.translation_.y, node.translation_.z);
        DirectX::XMStoreFloat4x4(&node.globalTransform_, S * R * T * DirectX::XMLoadFloat4x4(&parentGlobalTransforms.top()));

        // 移動値をなくす
        if (node.isRootNode_ && useRootMotionMovement_ == false)
        {
            node.globalTransform_._41 = 0;
            node.globalTransform_._42 = 0;
            node.globalTransform_._43 = 0;
        }

        for (int childIndex : node.children_)
        {
            parentGlobalTransforms.push(node.globalTransform_);
            traverse(childIndex);
            parentGlobalTransforms.pop();
        }
    } };
    for (std::vector<int>::value_type nodeIndex : scenes_.at(0).nodes_)
    {
        parentGlobalTransforms.push(
            {
                1,0,0,0,
                0,1,0,0,
                0,0,1,0,
                0,0,0,1
            }
        );
        traverse(nodeIndex);
        parentGlobalTransforms.pop();
    }
}

GltfModel::BufferView GltfModel::MakeBufferView(const tinygltf::Accessor& accessor)
{
    BufferView bufferView;
    switch (accessor.type)
    {
    case TINYGLTF_TYPE_SCALAR:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            bufferView.format_ = DXGI_FORMAT_R16_UINT;
            bufferView.strideInBytes_ = sizeof(USHORT);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            bufferView.format_ = DXGI_FORMAT_R32_UINT;
            bufferView.strideInBytes_ = sizeof(UINT);
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }
        break;
    case TINYGLTF_TYPE_VEC2:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            bufferView.format_ = DXGI_FORMAT_R32G32_FLOAT;
            bufferView.strideInBytes_ = sizeof(FLOAT) * 2;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }
        break;
    case TINYGLTF_TYPE_VEC3:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            bufferView.format_ = DXGI_FORMAT_R32G32B32_FLOAT;
            bufferView.strideInBytes_ = sizeof(FLOAT) * 3;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }
        break;
    case TINYGLTF_TYPE_VEC4:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            bufferView.format_ = DXGI_FORMAT_R8G8B8A8_UINT;
            bufferView.strideInBytes_ = sizeof(BYTE) * 4;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            bufferView.format_ = DXGI_FORMAT_R16G16B16A16_UINT;
            bufferView.strideInBytes_ = sizeof(USHORT) * 4;
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            bufferView.format_ = DXGI_FORMAT_R32G32B32A32_UINT;
            bufferView.strideInBytes_ = sizeof(UINT) * 4;
            break;
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            bufferView.format_ = DXGI_FORMAT_R32G32B32A32_FLOAT;
            bufferView.strideInBytes_ = sizeof(FLOAT) * 4;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not suppoeted.");
            break;
        }
        break;
    default:
        _ASSERT_EXPR(FALSE, L"This accessor type is not supported.");
        break;
    }
    bufferView.sizeInBytes_ = static_cast<UINT>(accessor.count * bufferView.strideInBytes_);
    return bufferView;
}