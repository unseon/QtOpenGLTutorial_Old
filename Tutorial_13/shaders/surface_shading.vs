attribute highp vec4 vertices;

varying mediump vec2 texc;

void main() {
    gl_Position = vertices;
    texc = (vertices.xy + vec2(1.0, 1.0)) * 0.5;
}
