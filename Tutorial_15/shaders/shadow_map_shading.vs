attribute highp vec4 vertices;
attribute highp vec4 normals;

varying highp vec4 fragVertex;
varying highp vec3 fragNormal;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 NormalMatrix;

void main() {
    gl_Position = MVP * vertices;
    fragVertex = MVP * vertices;
}
