struct Light {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
    vec3 direction;
};

struct Material {
    vec4 emission;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
    float opacity;
};

struct Scene {
    vec4 backgroundColor;
};

varying highp vec4 fragVertex;
varying highp vec3 fragNormal;

uniform mat4 NormalMatrix;

uniform Light light;
uniform Material material;
uniform Scene scene;

void main() {
    float depth = ((fragVertex.z / fragVertex.w) + 1.0) * 0.5;

    //float depth = 0.5;
    gl_FragColor = vec4(depth, depth, depth, 1.0);
    gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
