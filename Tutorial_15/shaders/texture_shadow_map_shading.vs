attribute highp vec4 vertices;
attribute mediump vec2 texCoord;
attribute highp vec4 normals;
attribute highp vec4 tangents;
attribute highp vec4 bitangents;

varying mediump vec2 texc;
varying highp vec3 fragVertex;
varying highp vec3 fragNormal;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 NormalMatrix;

varying highp vec4 lightVertex;
uniform mat4 lightMVP;

void main() {
    gl_Position = MVP * vertices;

    texc = texCoord;
    fragVertex = (MV * vertices).xyz;
    fragNormal = normalize((NormalMatrix * normals).xyz);
    lightVertex = lightMVP * vertices;
}
