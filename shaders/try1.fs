#version 330

// ── Raylib inputs ─────────────────────────────────────────────────────────────
// fragTexCoord is declared to satisfy raylib's shader linker but we derive
// UV from gl_FragCoord instead — this works correctly with any draw call
// (DrawRectangle, DrawTexture, DrawRectanglePro, etc.)
in vec2  fragTexCoord;
in vec4  fragColor;

out vec4 finalColor;

uniform vec2  resolution; // set via SetShaderValue each frame
uniform float time;       // set via SetShaderValue each frame

// ─────────────────────────────────────────────
//  PALETTE
// ─────────────────────────────────────────────
#define COL_VOID        vec3(0.04, 0.02, 0.07)   // deep blackened void
#define COL_ULTRAVIOLET vec3(0.30, 0.02, 0.55)   // UV purple
#define COL_MAGENTA     vec3(0.85, 0.05, 0.55)   // ultraviolet magenta
#define COL_VIOLET      vec3(0.22, 0.12, 0.40)   // muted violet
#define COL_PINK        vec3(1.00, 0.25, 0.75)   // hot pink glow
#define COL_CYAN_GHOST  vec3(0.05, 0.80, 0.90)   // corrupted cyan artefact
#define COL_AMBER_ERR   vec3(1.00, 0.55, 0.02)   // error amber

// ─────────────────────────────────────────────
//  MATH HELPERS
// ─────────────────────────────────────────────
float hash(vec2 p) {
    p  = fract(p * vec2(127.1, 311.7));
    p += dot(p, p + 19.31);
    return fract(p.x * p.y);
}

float hash1(float n) { return fract(sin(n) * 43758.5453); }

float vnoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i),             hash(i + vec2(1.0, 0.0)), u.x),
               mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), u.x), u.y);
}

float fbm(vec2 p) {
    float v = 0.0, a = 0.5;
    for (int i = 0; i < 5; i++) {
        v += a * vnoise(p);
        p  = p * 2.1 + vec2(1.3, 0.7);
        a *= 0.5;
    }
    return v;
}

// ─────────────────────────────────────────────
//  CRT BARREL DISTORTION
// ─────────────────────────────────────────────
vec2 barrel(vec2 uv) {
    vec2  c  = uv - 0.5;
    float r2 = dot(c, c);
    c *= 1.0 + 0.12 * r2 + 0.05 * r2 * r2;
    return c + 0.5;
}

// ─────────────────────────────────────────────
//  VIGNETTE
// ─────────────────────────────────────────────
float vignette(vec2 uv) {
    vec2 d = abs(uv - 0.5) * 2.0;
    return pow(1.0 - dot(d, d) * 0.55, 1.8);
}

// ─────────────────────────────────────────────
//  SCANLINES
// ─────────────────────────────────────────────
float scanline(vec2 uv) {
    float line = mod(uv.y * resolution.y, 2.0);
    float base = smoothstep(0.0, 0.5, line) * (1.0 - smoothstep(1.5, 2.0, line));
    float roll = 0.5 + 0.5 * sin(uv.y * 180.0 - time * 1.5);
    return mix(0.80, 1.0, base) * (0.97 + 0.03 * roll);
}

// ─────────────────────────────────────────────
//  GLITCH — per-row horizontal tear
// ─────────────────────────────────────────────
float glitchRow(float y, float t) {
    float sliceSize = 0.03 + 0.05 * hash1(floor(t * 8.0));
    float row       = floor(y / sliceSize);
    float rnd       = hash1(row + floor(t * 6.0) * 531.7);
    return (rnd > 0.75) ? (rnd - 0.75) * 0.25 : 0.0;
}

// ─────────────────────────────────────────────
//  GLITCH — block corruption
// ─────────────────────────────────────────────
vec2 blockGlitch(vec2 uv, float t) {
    float bt   = floor(t * 3.0);
    float bRnd = hash1(bt * 13.37);
    if (bRnd < 0.35) return uv;

    float bx = hash1(bt * 7.11);
    float by = hash1(bt * 3.91);
    float bw = 0.05 + 0.25 * hash1(bt * 19.2);
    float bh = 0.02 + 0.08 * hash1(bt * 5.33);

    if (uv.x > bx && uv.x < bx + bw && uv.y > by && uv.y < by + bh) {
        float shift = (hash1(bt * 2.71 + floor(uv.y * 120.0)) - 0.5) * 0.06;
        uv.x = fract(uv.x + shift);
    }
    return uv;
}

// ─────────────────────────────────────────────
//  GLITCH — CRT sync error bars
// ─────────────────────────────────────────────
float syncError(float y, float t) {
    float band = fract(y * 12.0 - t * 0.7);
    float rnd  = hash1(floor(t * 4.0) + floor(y * 12.0) * 17.3);
    return (rnd > 0.88 && band < 0.04) ? 1.0 : 0.0;
}

// ─────────────────────────────────────────────
//  NEON PERSPECTIVE GRID
// ─────────────────────────────────────────────
float neonGrid(vec2 uv, float t) {
    float horizon = 0.48;
    if (uv.y > horizon) return 0.0;

    float depth = horizon - uv.y;
    float persp = 1.0 / (depth + 0.05);

    vec2 grid;
    grid.x = fract((uv.x - 0.5) * persp * 3.0 + t * 0.4);
    grid.y = fract(persp * 1.5 - t * 0.6);

    float lx    = min(grid.x, 1.0 - grid.x);
    float ly    = min(grid.y, 1.0 - grid.y);
    float lineW = 0.04 * min(1.0, persp * 0.04);

    float g = smoothstep(lineW, 0.0, lx) + smoothstep(lineW, 0.0, ly);
    return clamp(g, 0.0, 1.0) * smoothstep(0.0, 0.08, depth);
}

// ─────────────────────────────────────────────
//  HOLOGRAPHIC SWEEP BAND
// ─────────────────────────────────────────────
float holoBand(vec2 uv, float t) {
    float y = fract(uv.y - t * 0.18);
    return smoothstep(0.0, 0.03, y) * smoothstep(0.12, 0.09, y) * 0.35;
}

// ─────────────────────────────────────────────
//  FILM GRAIN / STATIC
// ─────────────────────────────────────────────
float grain(vec2 uv, float t) {
    vec2 p = uv * resolution + t * 300.0;
    return (hash(floor(p)) - 0.5) * 0.06;
}

// ─────────────────────────────────────────────
//  NEBULA BACKGROUND
// ─────────────────────────────────────────────
vec3 nebula(vec2 uv, float t) {
    vec2  q  = uv + vec2(t * 0.015, t * 0.008);
    float f1 = fbm(q * 2.5);
    float f2 = fbm(q * 4.0 + vec2(f1 * 1.5, 0.8));
    float f3 = fbm(q * 1.2 - vec2(0.4, f2));

    vec3 col = COL_VOID;
    col = mix(col, COL_ULTRAVIOLET, smoothstep(0.35, 0.70, f1) * 0.7);
    col = mix(col, COL_VIOLET,      smoothstep(0.50, 0.75, f2) * 0.5);
    col = mix(col, COL_MAGENTA,     smoothstep(0.65, 0.85, f3) * 0.45);
    return col;
}

// ─────────────────────────────────────────────
//  DIGITAL RAIN
// ─────────────────────────────────────────────
float digitalRain(vec2 uv, float t) {
    float cols  = 60.0;
    float col   = floor(uv.x * cols);
    float speed = 0.3 + hash1(col) * 0.7;
    float head  = fract(t * speed + hash1(col + 100.0));
    float trail = smoothstep(head + 0.25, head, fract(uv.y))
                * smoothstep(head - 0.02, head, fract(uv.y));
    return smoothstep(0.0, 0.25, trail)
         * (0.08 + 0.12 * hash(vec2(col, floor(uv.y * 80.0) + t * 5.0)));
}

// ─────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────
void main() {
    // Derive UV from gl_FragCoord — works with any raylib draw call.
    // gl_FragCoord origin is bottom-left; flip Y so (0,0) = top-left.
    vec2 uv = vec2(gl_FragCoord.x / resolution.x,
                   1.0 - gl_FragCoord.y / resolution.y);
    float t  = time;

    // ── CRT barrel distortion
    vec2 bUV = barrel(uv);
    if (bUV.x < 0.0 || bUV.x > 1.0 || bUV.y < 0.0 || bUV.y > 1.0) {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // ── Glitch UV distortion
    vec2  gUV      = blockGlitch(bUV, t);
    float rowShift = glitchRow(gUV.y, t);
    gUV.x = fract(gUV.x + rowShift);

    // ── Chromatic aberration (burst-driven)
    float glitchPhase = 0.5 + 0.5 * sin(t * 7.3);
    float burst       = hash1(floor(t * 3.0) * 5.51) > 0.80 ? 1.0 : 0.0;
    float caAmt       = 0.003 + 0.012 * glitchPhase * burst + rowShift * 0.5;

    vec3 colR = nebula(gUV + vec2( caAmt, 0.0), t);
    vec3 colG = nebula(gUV,                      t);
    vec3 colB = nebula(gUV + vec2(-caAmt, 0.0), t);
    vec3 col  = vec3(colR.r, colG.g, colB.b);

    // ── Neon perspective grid
    float grid    = neonGrid(gUV, t);
    vec3  gridCol = mix(COL_MAGENTA, COL_PINK, 0.5 + 0.5 * sin(t + gUV.x * 6.2));
    col = mix(col, gridCol, grid * 0.85);

    // ── Digital rain
    float rain = digitalRain(gUV, t);
    col = mix(col, COL_MAGENTA * 2.0, rain * 0.5);
    col = mix(col, COL_CYAN_GHOST,    rain * 0.2);

    // ── Holographic sweep band
    col += holoBand(gUV, t) * COL_PINK * 0.6;

    // ── CRT sync error bars
    col = mix(col, vec3(1.0, 0.1, 0.6), syncError(gUV.y,        t      ) * 0.9);
    col = mix(col, COL_AMBER_ERR,        syncError(gUV.y + 0.5,  t * 1.3) * 0.5);

    // ── Scanlines
    col *= scanline(gUV);

    // ── Film grain
    col += grain(gUV, t);

    // ── Vignette
    col *= vignette(bUV);

    // ── Phosphor bloom (bright areas bleed pink)
    float lum = dot(col, vec3(0.2126, 0.7152, 0.0722));
    col += COL_PINK * pow(max(lum - 0.45, 0.0), 2.0) * 1.8;

    // ── Tone map + gamma (CRT phosphor response)
    col = col / (col + 0.7);
    col = pow(max(col, 0.0), vec3(0.88));

    // ── Rare hard-glitch invert flash
    if (hash1(floor(t * 12.0) * 91.3) > 0.97) col = 1.0 - col;

    finalColor = vec4(col, 1.0);
}
