attribute highp vec4 vertices;
attribute mediump vec2 texCoord;
attribute highp vec4 normals;
attribute highp vec4 tangents;
attribute highp vec4 bitangents;

varying mediump vec2 texc;
varying highp vec3 fragVertex;
varying highp vec3 fragNormal;
varying highp vec3 fragTangent;
varying highp vec3 fragBitangent;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 NormalMatrix;

void main() {
    gl_Position = MVP * vertices;

    texc = texCoord;
    fragVertex = (MV * vertices).xyz;
    fragNormal = (NormalMatrix * normals).xyz;
    fragTangent = (NormalMatrix * tangents).xyz;
    fragBitangent = (NormalMatrix * bitangents).xyz;
}
