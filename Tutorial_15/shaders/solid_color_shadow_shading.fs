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

varying highp vec3 fragVertex;
varying highp vec3 fragNormal;

uniform mat4 NormalMatrix;

uniform sampler2D shadowmap;

uniform Light light;
uniform Material material;
uniform Scene scene;

void main() {
    vec3 viewL = light.direction;
    vec3 viewE = normalize(-fragVertex);
    vec3 viewN = fragNormal;
    vec3 viewR = reflect(viewL, viewN);

    vec4 diffColor = material.diffuse;
    float cosTheta = clamp(dot(viewN, -viewL), 0.0, 1.0);
    vec4 Idiff = diffColor * cosTheta;
    Idiff = clamp(Idiff, 0.0, 1.0);

    float cosAlpha = clamp(dot(viewE, viewR), 0.0, 1.0);
    vec4 Ispec = light.specular * pow(cosAlpha, 5.0);
    Ispec = clamp(Ispec, 0.0, 1.0);

    //gl_FragColor = vec4((scene.backgroundColor + Idiff + Ispec).xyz, material.opacity);

    gl_FragColor = texture2D(shadowmap, vec2(fragVertex.z / 100.0, fragVertex.z / 100.0));
}
