# version 450

layout(location = 0) in vec2 fragTexCoord;
layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(location = 0) out vec4 uFragColor;

void main()
{
    uFragColor = texture(texSampler, fragTexCoord);
}
