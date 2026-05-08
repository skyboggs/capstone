#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform float time;
uniform vec2 resolution;

void main()
{
    vec2 uv = fragTexCoord; // (0,0) = top-left, (1,1) = bottom-right

    float horizon = 0.50;

    if (uv.y < horizon)
    {
        // -- sky ---------------------------------------------------------------
        float t = uv.y / horizon; // 0 at top, 1 at horizon
        vec3 col = mix(vec3(0.08, 0.02, 0.18), vec3(0.04, 0.01, 0.10), t);

        // twinkling stars
        vec2 sid = floor(uv * vec2(150.0, 75.0));
        float rng = fract(sin(dot(sid, vec2(127.1, 311.7))) * 43758.5453);
        if (rng > 0.965)
        {
            float twinkle = 0.6 + 0.4 * sin(time * (2.0 + rng * 6.0) + rng * 6.28);
            col += vec3(0.85, 0.55, 1.0) * (rng - 0.965) * 28.0 * twinkle;
        }

        // horizon glow bleeding up into sky
        float hg = exp(-(horizon - uv.y) * 22.0);
        col += vec3(0.55, 0.0, 0.85) * hg * 0.7;

        finalColor = vec4(col, 1.0);
        return;
    }

    // -- perspective grid floor ------------------------------------------------
    float d = uv.y - horizon;          // 0 at horizon -> 0.5 at bottom
    float z = (horizon * 0.5) / max(d, 0.0001); // world depth

    float worldX = (uv.x - 0.5) * z * 2.5;
    float worldZ = z - time * 2.5;

    // cell edges: lines where fract is near 0
    vec2 cell = fract(vec2(worldX, worldZ));
    float lineX = 1.0 - smoothstep(0.0, 0.04, min(cell.x, 1.0 - cell.x));
    float lineZ = 1.0 - smoothstep(0.0, 0.04, min(cell.y, 1.0 - cell.y));
    float line  = max(lineX, lineZ);

    // fade out near horizon and near bottom
    float depthFade = clamp(d * 14.0, 0.0, 1.0) * clamp(1.0 - d * 2.5, 0.0, 1.0);
    line *= depthFade;

    // magenta at center x, cyan at edges
    vec3 gridCol = mix(vec3(0.85, 0.0, 1.0), vec3(0.0, 0.88, 1.0), abs(uv.x - 0.5) * 2.0);
    vec3 bgCol   = vec3(0.04, 0.01, 0.08);

    vec3 col = mix(bgCol, gridCol, line);

    // horizon glow bleeding down into floor
    float hg = exp(-d * 22.0);
    col += vec3(0.6, 0.0, 0.9) * hg * 0.55;

    finalColor = vec4(col, 1.0);
}
