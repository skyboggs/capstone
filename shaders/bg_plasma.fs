#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform float time;
uniform vec2 resolution;

void main()
{
    vec2 uv = fragTexCoord;
    // aspect-correct coordinates centered at (0,0)
    vec2 p = (uv - 0.5) * vec2(resolution.x / resolution.y, 1.0);

    float t = time * 0.28;

    // -- layered interference waves -------------------------------------------
    float v = 0.0;
    v += sin(p.x * 4.2  + t * 1.1);
    v += sin(p.y * 3.7  + t * 0.9);
    v += sin((p.x + p.y) * 3.1 + t * 0.7);
    v += sin(length(p) * 6.5 - t * 1.8);
    v += sin(p.x * 2.0 - p.y * 1.5 + t * 1.3);
    v += sin(length(p - vec2(0.3 * sin(t), 0.3 * cos(t * 0.7))) * 7.0 - t);

    v = v / 6.0; // normalize to ~[-1, 1]

    // -- map to vaporwave palette: deep purple -> UV magenta -> neon pink -----
    float n = v * 0.5 + 0.5; // remap to [0,1]

    vec3 col;
    if (n < 0.33)
    {
        col = mix(vec3(0.04, 0.01, 0.12), vec3(0.35, 0.0, 0.65), n * 3.0);
    }
    else if (n < 0.66)
    {
        col = mix(vec3(0.35, 0.0, 0.65), vec3(0.75, 0.0, 0.95), (n - 0.33) * 3.0);
    }
    else
    {
        col = mix(vec3(0.75, 0.0, 0.95), vec3(1.0, 0.2, 0.65), (n - 0.66) * 3.0);
    }

    // keep it atmospheric and dark — not overwhelming behind the tablet
    col *= 0.38;

    // -- subtle radial vignette ------------------------------------------------
    float vig = 1.0 - smoothstep(0.4, 0.85, length(p * vec2(0.9, 1.1)));
    col *= vig;

    finalColor = vec4(col, 1.0);
}
