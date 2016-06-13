struct Light {
    vec4 direction;
};

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float spotExponent;
    float spotCutoff;
    float spotCosCutoff;
};

varying mediump vec2 texc;
varying highp vec4 fragNormal;

uniform sampler2D texture;
uniform Light light;
uniform vec4 lightDirection;
uniform mat4 NormalMatrix;


void main() {
    vec3 camDir = normalize(vec3(1.0, -1.0, -1.0));

    float cosTheta = clamp(dot(-light.direction.xyz, fragNormal.xyz), 0.0, 1.0);
    gl_FragColor = vec4(texture2D(texture, texc).xyz * cosTheta, 1);
}
