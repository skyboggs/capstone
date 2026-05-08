#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform float time;
uniform vec2 resolution;

void main()
{
    vec2 uv = fragTexCoord; // (0,0) = top-left

    // -- base dark purple background -------------------------------------------
    vec3 col = vec3(0.04, 0.01, 0.09);

    // subtle vertical gradient: top slightly lighter (phosphor bloom from above)
    col = mix(col, vec3(0.07, 0.02, 0.14), 1.0 - uv.y);

    // -- scanlines -------------------------------------------------------------
    // one dark band per 2 screen pixels
    float scanline = sin(uv.y * resolution.y * 3.14159265);
    scanline = pow(abs(scanline), 0.4);          // sharpen the bright bands
    col *= 0.72 + 0.28 * scanline;

    // -- phosphor dot mask (very subtle) ---------------------------------------
    float dotH = sin(uv.x * resolution.x * 3.14159265);
    dotH = pow(abs(dotH), 0.8);
    col *= 0.92 + 0.08 * dotH;

    // -- vignette --------------------------------------------------------------
    vec2 vig = uv * (1.0 - uv);          // peaks at center, 0 at edges
    float vignette = pow(vig.x * vig.y * 16.0, 0.35);
    col *= vignette;

    // -- screen flicker --------------------------------------------------------
    col *= 0.96 + 0.04 * sin(time * 11.3 + 0.5);

    // -- occasional horizontal glitch line -------------------------------------
    float glitchSeed = fract(sin(floor(time * 3.0)) * 4375.85);
    float glitchY    = glitchSeed;
    float glitchBand = smoothstep(0.003, 0.0, abs(uv.y - glitchY));
    // horizontal shift artifact on that band
    float shift = (fract(sin(time * 73.1) * 537.3) - 0.5) * 0.015;
    float glitchLine = step(0.992, fract(sin(dot(vec2(uv.x + shift, floor(uv.y * resolution.y)), vec2(127.1, 311.7))) * 43758.5)) * glitchBand;
    col += vec3(0.55, 0.0, 0.85) * glitchLine * 0.6;

    // wide dim glitch bar that drifts slowly
    float bar = smoothstep(0.01, 0.0, abs(fract(uv.y + time * 0.08) - 0.3));
    col += vec3(0.3, 0.0, 0.45) * bar * 0.07;

    finalColor = vec4(col, 1.0);
}
