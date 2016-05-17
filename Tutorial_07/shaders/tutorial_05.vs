attribute highp vec4 vertices;
attribute mediump vec2 texCoord;
varying mediump vec2 texc;
uniform mat4 MVP;

void main() {
    gl_Position = MVP * vertices;
    texc = texCoord;
}
