#version 330 core

out vec4 fragColor;
uniform sampler2D leaf;

void main()
{
    fragColor = texture(leaf,gl_PointCoord);
    if(fragColor.a <  0.3f)discard;
    fragColor = vec4(0.6,0.6,0.0,fragColor.a);
}
