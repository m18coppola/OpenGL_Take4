#version 430

in vec2 tc;
in vec4 varyingColor;
out vec4 color;
layout (binding = 0) uniform sampler2D s;
void main()
{
    color = vec4(0.0, 0.0, 1.0, 1.0);
    //color = varyingColor;
    //color = texture(s, tc);
}


