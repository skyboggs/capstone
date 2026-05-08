#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform float time;
uniform vec2 resolution;

void main()
{
    vec2 uv = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y) / resolution;

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
    float d = uv.y - horizon;
    float z = (horizon * 0.5) / max(d, 0.0001);

    float worldX = (uv.x - 0.5) * z * 2.5;
    float worldZ = z - time * 2.5;

    vec2 cellId = floor(vec2(worldX, worldZ));

    // glitch: per-row horizontal displacement that snaps on/off
    float rowHash  = fract(sin(dot(cellId, vec2(71.3, 157.9)) + floor(time * 7.0)) * 43758.5);
    float rowShift = step(0.88, rowHash) * (fract(rowHash * 37.0) - 0.5) * 0.6;
    worldX += rowShift;

    // cell edges — hard step for a sharp digital look
    vec2 cell  = fract(vec2(worldX, worldZ));
    float lineX = step(min(cell.x, 1.0 - cell.x), 0.035);
    float lineZ = step(min(cell.y, 1.0 - cell.y), 0.035);
    float line  = max(lineX, lineZ);

    // glitch: lines flicker on/off per-cell per-frame burst
    float flickHash = fract(sin(dot(cellId, vec2(23.1, 473.7)) + floor(time * 18.0)) * 37261.9);
    line *= step(0.12, flickHash); // ~12% of cells dark at any moment

    // depth: quantized bands instead of smooth fade — hard pop-in at horizon
    float depthFade = clamp(d * 14.0, 0.0, 1.0) * clamp(1.0 - d * 2.5, 0.0, 1.0);
    float quantized = floor(depthFade * 5.0) / 5.0;
    line *= mix(quantized, depthFade, 0.25);

    // glitch: occasional cell color corruption (cyan -> acid green flash)
    float colorHash = fract(sin(dot(cellId, vec2(127.1, 311.7)) + floor(time * 12.0)) * 43758.5);
    float corrupt   = step(0.94, colorHash);

    // magenta at center x, cyan at edges
    vec3 gridCol = mix(vec3(0.85, 0.0, 1.0), vec3(0.0, 0.88, 1.0), abs(uv.x - 0.5) * 2.0);
    gridCol = mix(gridCol, vec3(0.1, 1.0, 0.4), corrupt);

    // glitch: rare full-cell bright flash
    float flashHash = fract(sin(dot(cellId, vec2(311.7, 74.3)) + floor(time * 5.0)) * 19873.1);
    float flash     = step(0.97, flashHash) * depthFade;

    vec3 bgCol = vec3(0.04, 0.01, 0.08);
    vec3 col   = mix(bgCol, gridCol, line);
    col += vec3(0.9, 0.2, 1.0) * flash * 0.6;

    // horizon glow bleeding down into floor
    float hg = exp(-d * 22.0);
    col += vec3(0.6, 0.0, 0.9) * hg * 0.55;

    finalColor = vec4(col, 1.0);
}
