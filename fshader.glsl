#version 430

out vec4 fColor;
in vec4 varyingColor;

void main()
{
    fColor = varyingColor;
}


