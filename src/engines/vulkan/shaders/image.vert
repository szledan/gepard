# version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 fragTexCoord;

void main()
{
    gl_Position = vec4(position.x, position.y, 1.0, 1.0);
    fragTexCoord = texcoord;
}
