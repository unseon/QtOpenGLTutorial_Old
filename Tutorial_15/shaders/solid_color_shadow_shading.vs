attribute highp vec4 vertices;
attribute highp vec4 normals;

varying highp vec3 lightVertex;
varying highp vec3 fragVertex;
varying highp vec4 position;
varying highp vec3 fragNormal;

uniform mat4 Model;
uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 NormalMatrix;
uniform mat4 lightMVP;

void main() {
    gl_Position = MVP * vertices;

    position = gl_Position;

    lightVertex = (lightMVP * vertices).xyz;
    fragVertex = (MV * vertices).xyz;

    fragNormal = normalize((NormalMatrix * normals).xyz);
}
