#include "directionallight.h"

DirectionalLight::DirectionalLight()
    : m_shadowMapFrameBuffer(0),
      m_shadowMapTexture(0),
      m_shadowMapDepthBuffer(0)
{

}

void DirectionalLight::prepare()
{
    if (m_shadowMapFrameBuffer == 0 ) {
        //shadow map initialize
        glGenFramebuffers(1, &m_shadowMapFrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFrameBuffer);

        // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
        glGenTextures(1, &m_shadowMapTexture);
        glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_shadowMapTexture, 0);
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers);
    }
}
