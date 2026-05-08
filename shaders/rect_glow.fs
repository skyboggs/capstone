#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform vec2  resolution;
uniform float time;
uniform vec4  rectBounds;   // x, y, width, height in screen pixels (top-left origin)
uniform vec3  glowColor;
uniform float glowRadius;   // falloff distance in pixels

void main()
{
    vec2 p       = fragTexCoord * resolution;
    vec2 center  = rectBounds.xy + rectBounds.zw * 0.5;
    vec2 halfExt = rectBounds.zw * 0.5;

    // box SDF: negative inside, positive outside, 0 on border
    vec2  d   = abs(p - center) - halfExt;
    float sdf = length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);

    // exponential falloff from border in both directions
    float glow  = exp(-abs(sdf) / max(glowRadius, 1.0));

    // slow breathing pulse
    float pulse = 0.75 + 0.25 * sin(time * 2.5);
    glow *= pulse;

    // pure additive RGB — blend mode handles compositing
    finalColor = vec4(glowColor * glow, 1.0);
}
