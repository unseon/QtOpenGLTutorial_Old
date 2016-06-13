attribute highp vec4 vertices;
attribute mediump vec2 texCoord;
attribute highp vec4 normals;

varying mediump vec2 texc;
varying highp vec4 fragNormal;
varying highp vec4 fragVertex;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 NormalMatrix;

void main() {
    gl_Position = MVP * vertices;
    fragNormal = NormalMatrix * normals;
    texc = texCoord;
    fragVertex = MV * vertices;
}
