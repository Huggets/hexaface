#version 450


layout(push_constant) uniform Push {
    uint windowWidth;
    uint windowHeight;
} push;

void main() {
    const uint halfWindowWidth = push.windowWidth / 2;
    const uint halfWindowHeight = push.windowHeight / 2;

    const vec2 positions[] = {
        vec2( 2.0  / halfWindowWidth, -2.0  / halfWindowHeight),
        vec2( 18.0 / halfWindowWidth, -2.0  / halfWindowHeight),
        vec2( 18.0 / halfWindowWidth,  2.0  / halfWindowHeight),
        vec2( 2.0  / halfWindowWidth,  2.0  / halfWindowHeight),
        vec2( 2.0  / halfWindowWidth,  18.0 / halfWindowHeight),
        vec2(-2.0  / halfWindowWidth,  18.0 / halfWindowHeight),
        vec2(-2.0  / halfWindowWidth,  2.0  / halfWindowHeight),
        vec2(-18.0 / halfWindowWidth,  2.0  / halfWindowHeight),
        vec2(-18.0 / halfWindowWidth, -2.0  / halfWindowHeight),
        vec2(-2.0  / halfWindowWidth, -2.0  / halfWindowHeight),
        vec2(-2.0  / halfWindowWidth, -18.0 / halfWindowHeight),
        vec2( 2.0  / halfWindowWidth, -18.0 / halfWindowHeight)
    };
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}