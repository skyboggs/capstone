#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform float time;
uniform vec2 resolution;

void main()
{
    vec2 uv = fragTexCoord; // (0,0) = top-left

    // -- base purple background ------------------------------------------------
    vec3 col = vec3(0.16, 0.05, 0.28);

    // vertical gradient: top lighter (phosphor bloom)
    col = mix(col, vec3(0.30, 0.08, 0.48), 1.0 - uv.y);

    // -- scanlines -------------------------------------------------------------
    float scanline = sin(uv.y * resolution.y * 3.14159265);
    scanline = pow(abs(scanline), 0.35);
    col *= 0.82 + 0.18 * scanline;

    // -- phosphor dot mask -----------------------------------------------------
    float dotH = sin(uv.x * resolution.x * 3.14159265);
    dotH = pow(abs(dotH), 0.8);
    col *= 0.91 + 0.09 * dotH;

    // -- vignette (soft — never blacks out the corners) ------------------------
    vec2 vig = uv * (1.0 - uv);
    float vignette = mix(pow(vig.x * vig.y * 16.0, 0.28), 1.0, 0.30);
    col *= vignette;

    // -- screen flicker --------------------------------------------------------
    col *= 0.97 + 0.03 * sin(time * 11.3 + 0.5);

    // -- occasional horizontal glitch line -------------------------------------
    float glitchSeed = fract(sin(floor(time * 3.0)) * 4375.85);
    float glitchY    = glitchSeed;
    float glitchBand = smoothstep(0.003, 0.0, abs(uv.y - glitchY));
    float shift = (fract(sin(time * 73.1) * 537.3) - 0.5) * 0.015;
    float glitchLine = step(0.992, fract(sin(dot(vec2(uv.x + shift, floor(uv.y * resolution.y)), vec2(127.1, 311.7))) * 43758.5)) * glitchBand;
    col += vec3(0.90, 0.20, 1.00) * glitchLine * 1.4;

    // wide glitch bar that drifts slowly
    float bar = smoothstep(0.01, 0.0, abs(fract(uv.y + time * 0.08) - 0.3));
    col += vec3(0.70, 0.10, 0.90) * bar * 0.22;

    finalColor = vec4(col, 1.0);
}
