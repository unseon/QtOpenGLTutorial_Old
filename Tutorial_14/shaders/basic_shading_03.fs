struct Light {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 position;
    vec4 direction;
};

struct Material {
    vec4 emission;
    vec4 ambient;
//    vec4 diffuse;
    vec4 specular;
    float shininess;
    float opacity;
};

struct Scene {
    vec4 backgroundColor;
};

varying mediump vec2 texc;
varying highp vec4 fragNormal;
varying highp vec4 fragVertex;

uniform sampler2D texture;
uniform vec4 lightDirection;
uniform mat4 NormalMatrix;

uniform Light light;
uniform Material material;
uniform Scene scene;

void main() {
    vec3 L = -light.direction.xyz;
    vec3 E = normalize(-fragVertex).xyz;
    vec3 R = normalize(-reflect(L, fragNormal.xyz)).xyz;

    vec4 textureColor = texture2D(texture, texc);

    vec4 Iamb = 0.5 * (light.ambient + textureColor * length(light.ambient) * 0.333);

    vec4 Idiff = textureColor * max(dot(fragNormal.xyz,L), 0.0);
    Idiff = clamp(Idiff, 0.0, 1.0);

    vec4 Ispec = light.specular
                * pow(max(dot(R,E),0.0), material.shininess);
    Ispec = clamp(Ispec, 0.0, 1.0);

    gl_FragColor = vec4((scene.backgroundColor + Iamb + Idiff + Ispec).xyz, material.opacity);
}
