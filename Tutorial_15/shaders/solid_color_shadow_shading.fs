struct Light {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
    vec3 direction;
    mat4 view;
    mat4 projection;
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

varying highp vec4 lightVertex;
varying highp vec3 fragVertex;
varying highp vec3 fragNormal;
varying highp vec4 position;

uniform mat4 NormalMatrix;

uniform sampler2D shadowmap;
uniform sampler2D texture;

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

    vec4 finalColor = vec4((scene.backgroundColor + Idiff + Ispec).xyz, material.opacity);

    vec2 lightUV = lightVertex.xy;

    lightUV *= vec2(0.5, 0.5);
    lightUV += vec2(0.5, 0.5);

    float depth = ((lightVertex.z / lightVertex.w) + 1.0) * 0.5;

    if (texture2D(shadowmap, lightUV).x < depth - 0.01) {
        gl_FragColor = vec4(Idiff.xyz * 0.3, 1.0);

    } else {
        //gl_FragColor = texture2D(texture, texc) * 0.5 + texture2D(shadowmap, lightUV);
        gl_FragColor = finalColor;
    }
}
