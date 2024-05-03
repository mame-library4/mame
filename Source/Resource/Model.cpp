#include "Model.h"

#include "../Other/misc.h"

#include <sstream>
#include <functional>

#include "../Graphics/shader.h"
#include "texture.h"

#include <filesystem>
#include <fstream>


inline DirectX::XMFLOAT4X4 to_xmfloat4x4(const FbxAMatrix& fbxamatrix)
{
    DirectX::XMFLOAT4X4 xmfloat4x4;
    for (int row = 0; row < 4; ++row)
    {
        for (int column = 0; column < 4; ++column)
        {
            xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
        }
    }
    return xmfloat4x4;
}
inline DirectX::XMFLOAT3 to_xmfloat3(const FbxDouble3& fbxdouble3)
{
    DirectX::XMFLOAT3 xmfloat3;
    xmfloat3.x = static_cast<float>(fbxdouble3[0]);
    xmfloat3.y = static_cast<float>(fbxdouble3[1]);
    xmfloat3.z = static_cast<float>(fbxdouble3[2]);
    return xmfloat3;
}
inline DirectX::XMFLOAT4 to_xmfloat4(const FbxDouble4& fbxdouble4)
{
    DirectX::XMFLOAT4 xmfloat4;
    xmfloat4.x = static_cast<float>(fbxdouble4[0]);
    xmfloat4.y = static_cast<float>(fbxdouble4[1]);
    xmfloat4.z = static_cast<float>(fbxdouble4[2]);
    xmfloat4.w = static_cast<float>(fbxdouble4[3]);
    return xmfloat4;
}

// ボーン影響度を表現する構造体
struct boneInfluence
{
    uint32_t boneIndex;
    float boneWeight;
};
using boneInfluencesPerControlPoint = std::vector<boneInfluence>;

// ボーン影響度をFBXデータから取得する関数
void FetchBoneInfluences(const FbxMesh* fbxMesh,
    std::vector<boneInfluencesPerControlPoint>& boneInfluences)
{
    const int controlPointsCount{ fbxMesh->GetControlPointsCount() };
    boneInfluences.resize(controlPointsCount);

    const int skinCount{ fbxMesh->GetDeformerCount(FbxDeformer::eSkin) };
    for (int skinIndex = 0; skinIndex < skinCount; ++skinIndex)
    {
        const FbxSkin* fbxSkin
        { static_cast<FbxSkin*>(fbxMesh->GetDeformer(skinIndex,FbxDeformer::eSkin)) };

        const int clusterCount{ fbxSkin->GetClusterCount() };
        for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
        {
            const FbxCluster* fbxCluster{ fbxSkin->GetCluster(clusterIndex) };

            const int controlPointIndicesCount{ fbxCluster->GetControlPointIndicesCount() };
            for (int controlPointIndicesIndex = 0; controlPointIndicesIndex < controlPointIndicesCount; ++controlPointIndicesIndex)
            {
                int controlPointIndex{ fbxCluster->GetControlPointIndices()[controlPointIndicesIndex] };
                double controlPointWeight
                { fbxCluster->GetControlPointWeights()[controlPointIndicesIndex] };
                boneInfluence& boneInfluence{ boneInfluences.at(controlPointIndex).emplace_back() };
                boneInfluence.boneIndex = static_cast<uint32_t>(clusterIndex);
                boneInfluence.boneWeight = static_cast<float>(controlPointWeight);
            }
        }

        // weightsのxyzw合計1超えていた時のための正規化
        for (auto& influence : boneInfluences)
        {
            float t = 0;
            for (auto& bone : influence)
            {
                t += bone.boneWeight;
            }

            for (auto& bone : influence)
            {
                bone.boneWeight /= t;
            }
        }

    }
}

// リソース生成
void Model::CreateComObjects(ID3D11Device* device, const char* fbxFileName)
{
    HRESULT hr{ S_OK };

    for (Mesh& mesh : meshes)
    {
        D3D11_BUFFER_DESC bufferDesc{};
        D3D11_SUBRESOURCE_DATA subresource_data{};
        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;
        subresource_data.pSysMem = mesh.vertices.data();
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;
        hr = device->CreateBuffer(&bufferDesc, &subresource_data,
            mesh.vertexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subresource_data.pSysMem = mesh.indices.data();
        hr = device->CreateBuffer(&bufferDesc, &subresource_data,
            mesh.indexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


        mesh.vertices.clear();
        mesh.indices.clear();
    }

    for (std::unordered_map<uint64_t, Material>::iterator iterator = materials.begin();
        iterator != materials.end(); ++iterator)
    {
        D3D11_TEXTURE2D_DESC texture2dDesc;

        // Diffuse
        if (iterator->second.textureFilenames[0].size() > 0)
        {
            std::filesystem::path path(fbxFileName);
            path.replace_filename(iterator->second.textureFilenames[0]);
            LoadTextureFromFile(device, path.c_str(), iterator->second.shaderResourceViews[0].GetAddressOf(), &texture2dDesc);
        }
        else
        {
            MakeDummyTexture(device, iterator->second.shaderResourceViews[0].GetAddressOf(), 0xFFFFFFFF, 4);
        }

        // Normal
        if (iterator->second.textureFilenames[1].size() > 0)
        {
            std::filesystem::path path(fbxFileName);
            path.replace_filename(iterator->second.textureFilenames[1]);
            LoadTextureFromFile(device, path.c_str(), iterator->second.shaderResourceViews[1].GetAddressOf(), &texture2dDesc);
        }
        else
        {
            MakeDummyTexture(device, iterator->second.shaderResourceViews[1].GetAddressOf(), 0xFFFFF7F7, 4);
        }

        // EMISSIVE
        if (iterator->second.textureFilenames[2].size() > 0)
        {
            std::filesystem::path path(fbxFileName);
            path.replace_filename(iterator->second.textureFilenames[2]);
            LoadTextureFromFile(device, path.c_str(), iterator->second.shaderResourceViews[2].GetAddressOf(), &texture2dDesc);
        }
        else
        {
            MakeDummyTexture(device, iterator->second.shaderResourceViews[2].GetAddressOf(), 0xFF000000, 4);
        }
    }

#if 1
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT },
        { "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT },
        { "TANGENT" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT },
        { "WEIGHTS" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT },
        { "BONES"   , 0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT },
    };
    CreateVsFromCso(device, "./resources/Shader/FbxModelVS.cso", vertexShader.ReleaseAndGetAddressOf(),
        inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
    CreatePsFromCso(device, "./resources/Shader/CharacterPS.cso", pixelShader.ReleaseAndGetAddressOf());
#else
    // Instancing
    {
        // 頂点シェーダー
        {
            D3D11_INPUT_ELEMENT_DESC d3d_input_element_descs[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
                { "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
                { "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
                { "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
                { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            };
            create_vs_from_cso(device, "./resources/Shader/InstancingVS.cso", vertex_shader.ReleaseAndGetAddressOf(),
                input_layout.ReleaseAndGetAddressOf(), d3d_input_element_descs, ARRAYSIZE(d3d_input_element_descs));
        }
        // ピクセルシェーダー
        {
            create_ps_from_cso(device, "./resources/Shader/InstancingPS.cso", pixel_shader.ReleaseAndGetAddressOf());
        }
    }
#endif

    // 定数バッファー
    constant_ = std::make_unique<ConstantBuffer<Constants>>(device);
}

// コンストラクタ
Model::Model(ID3D11Device* device, const char* fbxFileName, bool triangulate, float samplingRate)
{
    std::filesystem::path cereal_filename(fbxFileName);
    cereal_filename.replace_extension("cereal");
    if (std::filesystem::exists(cereal_filename.c_str()))
    {
        std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(sceneView, meshes, materials, animationClips);
    }
    else
    {
        FetchScene(fbxFileName, triangulate, samplingRate);

        std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        serialization(sceneView, meshes, materials, animationClips);
    }
    CreateComObjects(device, fbxFileName);

    // BOUNDING_BOX
    ComputeBoundingBox();
}

// コンストラクタ ( アニメーションファイルあり )
Model::Model(ID3D11Device* device, const char* fbxFileName, std::vector<std::string>& animationFileNames, bool triangulate, float samplingRate)
{
    std::filesystem::path cereal_filename(fbxFileName);
    cereal_filename.replace_extension("cereal");
    if (std::filesystem::exists(cereal_filename.c_str()))
    {
        std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(sceneView, meshes, materials, animationClips);
    }
    else
    {
        FetchScene(fbxFileName, triangulate, samplingRate);

        for (const std::string animationFileName : animationFileNames)
        {
            AppendAnimations(animationFileName.c_str(), samplingRate);
        }

        std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        serialization(sceneView, meshes, materials, animationClips);
    }
    CreateComObjects(device, fbxFileName);

    // BOUNDING_BOX
    ComputeBoundingBox();
}

// 描画
void Model::Render(ID3D11DeviceContext* deviceContext, const DirectX::XMFLOAT4X4 world, const DirectX::XMFLOAT4& materialColor, const Animation::KeyFrame* keyFrame, ID3D11PixelShader* psShader)
{
    for (const Mesh& mesh : meshes)
    {
        uint32_t stride{ sizeof(Vertex) };
        uint32_t offset{ 0 };
        deviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
        deviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        deviceContext->IASetInputLayout(inputLayout.Get());

        deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
        psShader ? deviceContext->PSSetShader(psShader, nullptr, 0) : deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

        if (keyFrame && keyFrame->nodes.size() > 0)
        {
            // 定数バッファ構造体のworldメンバ変数の計算を変更する
            const Animation::KeyFrame::Node& meshNode{ keyFrame->nodes.at(mesh.nodeIndex) };
            DirectX::XMStoreFloat4x4(&constant_->data.world,
                DirectX::XMLoadFloat4x4(&meshNode.globalTransform) * DirectX::XMLoadFloat4x4(&world));

            const size_t boneCount{ mesh.bindPose.bones.size() };
            _ASSERT_EXPR(boneCount < MAX_BONES, L"The value of the 'bone_count' has exceded MAX_BONES.");

            for (size_t boneIndex = 0; boneIndex < boneCount; ++boneIndex)
            {
                const Skeleton::Bone& bone{ mesh.bindPose.bones.at(boneIndex) };
                const Animation::KeyFrame::Node& boneNode{ keyFrame->nodes.at(bone.nodeIndex) };
                DirectX::XMStoreFloat4x4(&constant_->data.boneTransforms[boneIndex],
                    DirectX::XMLoadFloat4x4(&bone.offsetTransform) *
                    DirectX::XMLoadFloat4x4(&boneNode.globalTransform) *
                    DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform))
                );
            }
        }
        else
        {
            DirectX::XMStoreFloat4x4(&constant_->data.world,
                DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform) * DirectX::XMLoadFloat4x4(&world));
            for (size_t boneIndex = 0; boneIndex < MAX_BONES; ++boneIndex)
            {
                constant_->data.boneTransforms[boneIndex] =
                {
                    1,0,0,0,
                    0,1,0,0,
                    0,0,1,0,
                    0,0,0,1
                };
            }
        }
        // サブセット
        for (const Mesh::Subset& subset : mesh.subsets)
        {
            const Material& material{ materials.at(subset.materialUniqueId) };

            DirectX::XMStoreFloat4(&constant_->data.materialColor,
                DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(&materialColor), DirectX::XMLoadFloat4(&material.Kd)));

            // 定数バッファー更新
            constant_->Activate(deviceContext, 0); 

            // Diffuse
            deviceContext->PSSetShaderResources(0, 1, material.shaderResourceViews[0].GetAddressOf());
            // Normal
            deviceContext->PSSetShaderResources(1, 1, material.shaderResourceViews[1].GetAddressOf());
            // EMISSIVE
            deviceContext->PSSetShaderResources(2, 1, material.shaderResourceViews[2].GetAddressOf());

            deviceContext->DrawIndexed(subset.indexCount, subset.startIndexLocation, 0);
        }
    }
}

// アニメーション更新
void Model::UpdateAnimation(Animation::KeyFrame& keyFrame)
{
    size_t nodeCount{ keyFrame.nodes.size() };
    for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        Animation::KeyFrame::Node& node{ keyFrame.nodes.at(nodeIndex) };
        DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scaling.x,node.scaling.y,node.scaling.z) };
        DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation)) };
        DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation.x,node.translation.y,node.translation.z) };

        int64_t parentIndex{ sceneView.nodes.at(nodeIndex).parentIndex };
        DirectX::XMMATRIX P{ parentIndex < 0 ? DirectX::XMMatrixIdentity() :
        DirectX::XMLoadFloat4x4(&keyFrame.nodes.at(parentIndex).globalTransform) };

        DirectX::XMStoreFloat4x4(&node.globalTransform, S * R * T * P);
    }
}

// アニメーション追加
bool Model::AppendAnimations(const char* animationFileName, float samplingRate)
{
    FbxManager* fbxManager{ FbxManager::Create() };
    FbxScene* fbxScene{ FbxScene::Create(fbxManager,"") };

    FbxImporter* fbx_importer{ FbxImporter::Create(fbxManager,"") };
    bool importStatus{ false };
    importStatus = fbx_importer->Initialize(animationFileName);
    _ASSERT_EXPR_A(importStatus, fbx_importer->GetStatus().GetErrorString());
    importStatus = fbx_importer->Import(fbxScene);
    _ASSERT_EXPR_A(importStatus, fbx_importer->GetStatus().GetErrorString());

    FetchAnimations(fbxScene, animationClips, samplingRate);

    fbxManager->Destroy();

    return true;
}

// アニメーションブレンド
void Model::BlendAnimations(const Animation::KeyFrame* keyFrames[2], float factor, Animation::KeyFrame& keyframe)
{
    _ASSERT_EXPR(keyFrames[0]->nodes.size() == keyFrames[1]->nodes.size(), "The size of the two node arrays must be the same.");

    size_t nodeCount{ keyFrames[0]->nodes.size() };
    keyframe.nodes.resize(nodeCount);
    for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        DirectX::XMVECTOR S[2]{
            DirectX::XMLoadFloat3(&keyFrames[0]->nodes.at(nodeIndex).scaling),
            DirectX::XMLoadFloat3(&keyFrames[1]->nodes.at(nodeIndex).scaling) };
        DirectX::XMStoreFloat3(&keyframe.nodes.at(nodeIndex).scaling, DirectX::XMVectorLerp(S[0], S[1], factor));

        DirectX::XMVECTOR R[2]{
            DirectX::XMLoadFloat4(&keyFrames[0]->nodes.at(nodeIndex).rotation),
            DirectX::XMLoadFloat4(&keyFrames[1]->nodes.at(nodeIndex).rotation) };
        DirectX::XMStoreFloat4(&keyframe.nodes.at(nodeIndex).rotation, DirectX::XMQuaternionSlerp(R[0], R[1], factor));

        DirectX::XMVECTOR T[2]{
            DirectX::XMLoadFloat3(&keyFrames[0]->nodes.at(nodeIndex).translation),
            DirectX::XMLoadFloat3(&keyFrames[1]->nodes.at(nodeIndex).translation) };
        DirectX::XMStoreFloat3(&keyframe.nodes.at(nodeIndex).translation, DirectX::XMVectorLerp(T[0], T[1], factor));
    }
}

// ImGui用
void Model::DrawDebug()
{
    if (ImGui::TreeNode("Emmisive"))
    {
        ImGui::ColorEdit4("color", &constant_->data.emissiveColor.x);
        ImGui::DragFloat("Intencity", &constant_->data.emissiveIntensity);
        ImGui::DragFloat("options", &constant_->data.emissiveOptions);
        ImGui::DragFloat2("scrollDirection", &constant_->data.emissiveScrollDirection.x);

        ImGui::TreePop();
    }

    ImGui::ColorEdit4("materialColor", &constant_->data.sageColor.x);
}

void Model::FetchMeshes(FbxScene* fbxScene, std::vector<Mesh>& meshes)
{
    for (const Scene::Node& node : sceneView.nodes)
    {
        if (node.attribute != FbxNodeAttribute::EType::eMesh)
        {
            continue;
        }

        FbxNode* fbxNode{ nodeList[node.uniqueId] };
        FbxMesh* fbxMesh{ fbxNode->GetMesh() };

        //if (fbx_mesh == nullptr)continue;

        Mesh& mesh{ meshes.emplace_back() };
        mesh.uniqueId = fbxMesh->GetNode()->GetUniqueID();
        mesh.name = fbxMesh->GetNode()->GetName();
        mesh.nodeIndex = sceneView.IndexOf(mesh.uniqueId);

        // シーン内の位置・姿勢・スケール情報
        mesh.defaultGlobalTransform = to_xmfloat4x4(fbxMesh->GetNode()->EvaluateGlobalTransform());

        // ボーン情報
        std::vector<boneInfluencesPerControlPoint>bone_influences;
        FetchBoneInfluences(fbxMesh, bone_influences);
        FetchSkeleton(fbxMesh, mesh.bindPose);


        // サブセット
        std::vector<Mesh::Subset>& subsets{ mesh.subsets };
        const int materialCount{ fbxMesh->GetNode()->GetMaterialCount() };
        subsets.resize(materialCount > 0 ? materialCount : 1);
        for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
        {
            const FbxSurfaceMaterial* fbxMaterial{ fbxMesh->GetNode()->GetMaterial(materialIndex) };
            subsets.at(materialIndex).materialName = fbxMaterial->GetName();
            subsets.at(materialIndex).materialUniqueId = fbxMaterial->GetUniqueID();
        }
        if (materialCount > 0)
        {
            const int polygonCount{ fbxMesh->GetPolygonCount() };
            for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
            {
                const int materialIndex
                { fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) };
                subsets.at(materialIndex).indexCount += 3;
            }
            uint32_t offset{ 0 };
            for (Mesh::Subset& subset : subsets)
            {
                subset.startIndexLocation = offset;
                offset += subset.indexCount;
                // これは次の手順でカウンターとして使用され、ゼロにもどる
                subset.indexCount = 0;
            }
        }

        const int polygonCount{ fbxMesh->GetPolygonCount() };
        mesh.vertices.resize(polygonCount * 3LL);
        mesh.indices.resize(polygonCount * 3LL);

        FbxStringList uvNames;
        fbxMesh->GetUVSetNames(uvNames);
        const FbxVector4* controlPoints{ fbxMesh->GetControlPoints() };
        for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
        {
            const int materialIndex{ materialCount > 0 ?
                fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0 };
            Mesh::Subset& subset{ subsets.at(materialIndex) };
            const uint32_t offset{ subset.startIndexLocation + subset.indexCount };

            for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)
            {
                const int vertexIndex{ polygonIndex * 3 + positionInPolygon };

                Vertex vertex;
                const int polygonVertex{ fbxMesh->GetPolygonVertex(polygonIndex, positionInPolygon) };
                vertex.position.x = static_cast<float>(controlPoints[polygonVertex][0]);
                vertex.position.y = static_cast<float>(controlPoints[polygonVertex][1]);
                vertex.position.z = static_cast<float>(controlPoints[polygonVertex][2]);

                // ボーン情報
                const boneInfluencesPerControlPoint& influencesPerControlPoint
                { bone_influences.at(polygonVertex) };
                for (size_t influenceIndex = 0; influenceIndex < influencesPerControlPoint.size();
                    ++influenceIndex)
                {
                    if (influenceIndex < MAX_BONE_INFLUENCES)
                    {
                        vertex.boneWeights[influenceIndex] =
                            influencesPerControlPoint.at(influenceIndex).boneWeight;
                        vertex.boneIndices[influenceIndex] =
                            influencesPerControlPoint.at(influenceIndex).boneIndex;
                    }
                    else
                    {
                        _ASSERT_EXPR(FALSE, L"BoneWeightOver");
                        // どうする？
                        // ４つの weight の total は必ず 1.0 になってないとだめ
                    }
                }

                if (fbxMesh->GetElementNormalCount() > 0)
                {
                    FbxVector4 normal;
                    fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
                    vertex.normal.x = static_cast<float>(normal[0]);
                    vertex.normal.y = static_cast<float>(normal[1]);
                    vertex.normal.z = static_cast<float>(normal[2]);
                }
                if (fbxMesh->GetElementUVCount() > 0)
                {
                    FbxVector2 uv;
                    bool unmappedUv;
                    fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon,
                        uvNames[0], uv, unmappedUv);
                    vertex.texcoord.x = static_cast<float>(uv[0]);
                    vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
                }
                if (fbxMesh->GenerateTangentsData(0, false))
                {
                    const FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
                    vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[0]);
                    vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[1]);
                    vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[2]);
                    vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[3]);
                }

                mesh.vertices.at(vertexIndex) = std::move(vertex);
                mesh.indices.at(static_cast<size_t>(offset) + positionInPolygon) = vertexIndex;
                subset.indexCount++;
            }
        }


        for (const Vertex& v : mesh.vertices)
        {
            mesh.boundingBox[0].x = std::min<float>(mesh.boundingBox[0].x, v.position.x);
            mesh.boundingBox[0].y = std::min<float>(mesh.boundingBox[0].y, v.position.y);
            mesh.boundingBox[0].z = std::min<float>(mesh.boundingBox[0].z, v.position.z);
            mesh.boundingBox[1].x = std::max<float>(mesh.boundingBox[1].x, v.position.x);
            mesh.boundingBox[1].y = std::max<float>(mesh.boundingBox[1].y, v.position.y);
            mesh.boundingBox[1].z = std::max<float>(mesh.boundingBox[1].z, v.position.z);
        }
    }
}

void Model::FetchMaterials(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials)
{
    const size_t nodeCount{ sceneView.nodes.size() };
    for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        const Scene::Node& node{ sceneView.nodes.at(nodeIndex) };
        const FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };

        const int materialCount{ fbxNode->GetMaterialCount() };
        for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
        {
            const FbxSurfaceMaterial* fbxMaterial{ fbxNode->GetMaterial(materialIndex) };

            Material material;
            material.name = fbxMaterial->GetName();
            material.uniqueId = fbxMaterial->GetUniqueID();
            FbxProperty fbxProperty;
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
            if (fbxProperty.IsValid())
            {
                // プロパティから色やテクスチャファイルパスを取得
                // ディフーズ
                const FbxDouble3 color{ fbxProperty.Get<FbxDouble3>() };
                material.Kd.x = static_cast<float>(color[0]);
                material.Kd.y = static_cast<float>(color[1]);
                material.Kd.z = static_cast<float>(color[2]);
                material.Kd.w = 1.0f;

                const FbxFileTexture* fbx_texture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
                material.textureFilenames[0] =
                    fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
            if (fbxProperty.IsValid())
            {
                // プロパティから色やテクスチャファイルパスを取得
                // アンビエント
                const FbxDouble3 color{ fbxProperty.Get<FbxDouble3>() };
                material.Ka.x = static_cast<float>(color[0]);
                material.Ka.y = static_cast<float>(color[1]);
                material.Ka.z = static_cast<float>(color[2]);
                material.Ka.w = 1.0f;
            }
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
            if (fbxProperty.IsValid())
            {
                // スペキュラ
                const FbxDouble3 color{ fbxProperty.Get<FbxDouble3>() };
                material.Ks.x = static_cast<float>(color[0]);
                material.Ks.y = static_cast<float>(color[1]);
                material.Ks.z = static_cast<float>(color[2]);
                material.Ks.w = 1.0f;
            }
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
            if (fbxProperty.IsValid())
            {
                const FbxFileTexture* file_texture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
                material.textureFilenames[1] = file_texture ? file_texture->GetRelativeFileName() : "";
            }
            // EMISSIVE
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
            if (fbxProperty.IsValid())
            {
                const FbxDouble3 color{ fbxProperty.Get<FbxDouble3>() };
                material.Ke.x = static_cast<float>(color[0]);
                material.Ke.y = static_cast<float>(color[1]);
                material.Ke.z = static_cast<float>(color[2]);
                material.Ke.w = 1.0f;

                const FbxFileTexture* fbxTexture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
                material.textureFilenames[2] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
            }

            materials.emplace(material.uniqueId, std::move(material));
        }
    }

    // ダミーマテリアル
    materials.emplace();
}

void Model::FetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose)
{
    const int deformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
    for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
    {
        FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
        const int clusterCount = skin->GetClusterCount();
        bindPose.bones.resize(clusterCount);
        for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
        {
            FbxCluster* cluster = skin->GetCluster(clusterIndex);

            Skeleton::Bone& bone{ bindPose.bones.at(clusterIndex) };
            bone.name = cluster->GetLink()->GetName();
            bone.uniqueId = cluster->GetLink()->GetUniqueID();
            bone.parentIndex = bindPose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());
            bone.nodeIndex = sceneView.IndexOf(bone.uniqueId);

            // モデル (メッシュ) のローカル空間から,シーンのグローバル空間に変換するために使用されます。
            FbxAMatrix referenceGlobalInitPosition;
            cluster->GetTransformMatrix(referenceGlobalInitPosition);

            // ボーンのローカル空間から シーンのグローバル空間に変換するために使用されます
            FbxAMatrix clusterGlobalInitPosition;
            cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

            // *行列は列 Major スキームを使用して定義されます。 FbxAMatrix が変換を表す場合
            // (移動、回転、スケール)、行列の最後の行は、変換の移動部分を表します。
            // 位置をメッシュ空間からボーン空間に変換する「bone.offset_transform」マトリックスを作成します。
            // この行列はオフセット行列と呼ばれます。
            bone.offsetTransform
                = to_xmfloat4x4(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);
        }
    }
}

void Model::FetchAnimations(FbxScene* fbxScene, std::vector<Animation>& animationClips, float samplingRate)
{
    FbxArray<FbxString*> animationStackNames;
    fbxScene->FillAnimStackNameArray(animationStackNames);
    const int animationStackCount{ animationStackNames.GetCount() };
    for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex)
    {
        Animation& animationClip{ animationClips.emplace_back() };
        animationClip.name = animationStackNames[animationStackIndex]->Buffer();

        FbxAnimStack* animationStack{ fbxScene->FindMember<FbxAnimStack>(animationClip.name.c_str()) };
        fbxScene->SetCurrentAnimationStack(animationStack);

        const FbxTime::EMode timeMode{ fbxScene->GetGlobalSettings().GetTimeMode() };
        FbxTime oneSecond;
        oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
        animationClip.samplingRate = samplingRate > 0 ?
            samplingRate : static_cast<float>(oneSecond.GetFrameRate(timeMode));
        const FbxTime samplingInterval
        { static_cast<FbxLongLong>(oneSecond.Get() / animationClip.samplingRate) };
        const FbxTakeInfo* takeInfo{ fbxScene->GetTakeInfo(animationClip.name.c_str()) };
        const FbxTime startTime{ takeInfo->mLocalTimeSpan.GetStart() };
        const FbxTime stopTime{ takeInfo->mLocalTimeSpan.GetStop() };
        for (FbxTime time = startTime; time < stopTime; time += samplingInterval)
        {
            Animation::KeyFrame& keyFrame{ animationClip.sequence.emplace_back() };

            const size_t nodeCount{ sceneView.nodes.size() };
            keyFrame.nodes.resize(nodeCount);
            for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
                FbxNode* fbxNode{ fbxScene->FindNodeByName(sceneView.nodes.at(nodeIndex).name.c_str()) };
                if (fbxNode)
                {
                    Animation::KeyFrame::Node& node{ keyFrame.nodes.at(nodeIndex) };
                    node.globalTransform = to_xmfloat4x4(fbxNode->EvaluateGlobalTransform(time));

                    const FbxAMatrix& local_transform{ fbxNode->EvaluateLocalTransform(time) };
                    node.scaling = to_xmfloat3(local_transform.GetS());
                    node.translation = to_xmfloat3(local_transform.GetT());
                    node.rotation = to_xmfloat4(local_transform.GetQ());
                }
            }
        }
    }
    for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex)
    {
        delete animationStackNames[animationStackIndex];
    }
}

void Model::FetchScene(const char* fbxFileName, bool triangulate, float samplingRate)
{
    FbxManager* fbxManager{ FbxManager::Create() };
    FbxScene* fbxScene{ FbxScene::Create(fbxManager,"") };
    FbxImporter* fbxImporter{ FbxImporter::Create(fbxManager,"") };
    bool importStatus{ false };
    importStatus = fbxImporter->Initialize(fbxFileName);
    _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
    importStatus = fbxImporter->Import(fbxScene);
    _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

    FbxGeometryConverter fbx_converter(fbxManager);
    if (triangulate)
    {
        fbx_converter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
        fbx_converter.RemoveBadPolygonsFromMeshes(fbxScene);
    }
    // Serialize an entire scene graph into sequence container
    std::function<void(FbxNode*)>traverse{ [&](FbxNode* fbxNode) {
        Scene::Node& node{sceneView.nodes.emplace_back()};
        node.attribute = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
        node.name = fbxNode->GetName();
        node.uniqueId = fbxNode->GetUniqueID();
        node.parentIndex = sceneView.IndexOf(fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0);

        //ノードをuniqueIdをキーとしたリストに格納
        nodeList[node.uniqueId] = fbxNode;

        for (int child_index = 0; child_index < fbxNode->GetChildCount(); ++child_index)
        {
            traverse(fbxNode->GetChild(child_index));
        }
    } };
    traverse(fbxScene->GetRootNode());

    FetchMeshes(fbxScene, meshes);
    FetchMaterials(fbxScene, materials);
    FetchAnimations(fbxScene, animationClips, samplingRate);

    fbxManager->Destroy();
}

// JOINT_POSITION
DirectX::XMFLOAT3 Model::JointPosition(const std::string& meshName, const std::string& boneName, const Animation::KeyFrame* keyFrame, const DirectX::XMFLOAT4X4& transform)
{
    DirectX::XMFLOAT3 position = {}; // world座標

    for (const Mesh& mesh : meshes)
    {
        if (mesh.name == meshName)
        {
            for (const Skeleton::Bone& bone : mesh.bindPose.bones)
            {
                if (bone.name == boneName)
                {
                    const Animation::KeyFrame::Node& node = keyFrame->nodes.at(bone.nodeIndex);
                    DirectX::XMFLOAT4X4 globalTransform = node.globalTransform;
                    position.x = globalTransform._41;
                    position.y = globalTransform._42;
                    position.z = globalTransform._43;
                    DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform) * DirectX::XMLoadFloat4x4(&transform);
                    DirectX::XMStoreFloat3(&position, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&position), M));
                    return position;
                }
            }
        }
    }

    _ASSERT_EXPR(FALSE, "Joint not found");

    return {};
}

// JOINT_POSITION
DirectX::XMFLOAT3 Model::JointPosition(size_t meshIndex, size_t boneIndex, const Animation::KeyFrame* keyframe, const DirectX::XMFLOAT4X4& transform)
{
    DirectX::XMFLOAT3 position = { 0,0,0 };
    const Mesh& mesh = meshes.at(meshIndex);
    const Skeleton::Bone& bone = mesh.bindPose.bones.at(boneIndex);
    const Animation::KeyFrame::Node& boneNode = keyframe->nodes.at(bone.nodeIndex);
    DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&boneNode.globalTransform) * DirectX::XMLoadFloat4x4(&transform);
    DirectX::XMStoreFloat3(&position, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&position), M));
    return position;
}
