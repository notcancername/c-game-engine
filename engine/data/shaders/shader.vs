#version 330 core
in vec2 position;

in vec2 texcoord;
in float textureID;

out vec2 Texcoord;
out int TextureID;

uniform vec3 movement;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0) + vec4(movement, 0.0);
    Texcoord = texcoord;
    TextureID = int(textureID);
}