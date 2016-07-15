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

varying highp vec3 lightVertex;
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
    vec2 lightUV = lightVertex.xy;
    lightUV += vec2(0.5, 0.5);
    lightUV *= vec2(0.5, 0.5);



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

    gl_FragColor = vec4((scene.backgroundColor + Idiff + Ispec).xyz, material.opacity);

//    if (lightZ > lightVertex.z)
//        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    //gl_FragColor = vec4(lightZ, lightZ, lightZ, 1.0);
    //gl_FragColor = texture2D(texture, position.xy);



    gl_FragColor = vec4(0, lightUV.y, 0, 1);

    //gl_FragColor = texture2D(shadowmap, vec2(fragVertex.z / 100.0, fragVertex.z / 100.0));
}
