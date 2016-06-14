attribute highp vec4 vertices;
attribute mediump vec2 texCoord;
attribute highp vec4 normals;
attribute highp vec4 tangents;
attribute highp vec4 bitangents;

varying mediump vec2 texc;
varying highp vec4 fragNormal;
varying highp vec4 fragVertex;
varying highp vec4 fragTangent;
varying highp vec4 fragBitangent;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 NormalMatrix;

void main() {
    gl_Position = MVP * vertices;
    fragNormal = NormalMatrix * normals;
    texc = texCoord;
    fragVertex = MV * vertices;
    fragTangent = NormalMatrix * tangents;
    fragBitangent = NormalMatrix * tangents;
}
