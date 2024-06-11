#include "Particle.hlsli"

RWStructuredBuffer<Particle> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;
    
    Particle p = particleBuffer[id];
    
    // �A�j���[�V��������
    p.animationTime += animationSpeed * deltaTime;
    int frame = (int) p.animationTime; // �����؂�̂�
    float animationFrame = (float) frame;
        
    const float totalXFrame = size.x / texSize.x;
    const float totalYFrame = size.y / texSize.y;    
    if (animationLoopFlag)
    {
        if (animationFrame > (int) (totalXFrame + 0.5f) - 1)
        {
            // �������̍s�����݂��Ȃ�
            if (p.texPos.y >= texSize.y * ((int) (totalYFrame + 0.5f) - 1) - 0.0001f)
            {
                // �U�o���ɖ߂�
                p.animationTime = 0.0f;
                p.texPos.x = 0;
                p.texPos.y = 0;
                animationFrame = 0.0f;
            }
            else
            {
                p.animationTime = 0.0f;
                p.texPos.y += texSize.y;
                animationFrame = 0.0f;
            }
        }
    }    
    p.texPos.x = texSize.x * animationFrame;
    
    
    p.position = p.velocity * p.age * 1.0f;
    
    particleBuffer[id] = p;
}