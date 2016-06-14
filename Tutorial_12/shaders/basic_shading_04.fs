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
varying highp vec4 fragTangent;
varying highp vec4 fragBitangent;

uniform sampler2D texture;
uniform sampler2D normalmap;

uniform vec4 lightDirection;
uniform mat4 NormalMatrix;

uniform Light light;
uniform Material material;
uniform Scene scene;


highp mat3 transpose(in highp mat3 inMatrix) {
    highp vec3 i0 = inMatrix[0];
    highp vec3 i1 = inMatrix[1];
    highp vec3 i2 = inMatrix[2];

    highp mat3 outMatrix = mat3(
                 vec3(i0.x, i1.x, i2.x),
                 vec3(i0.y, i1.y, i2.y),
                 vec3(i0.z, i1.z, i2.z)
                 );

    return outMatrix;
}

void main() {
    vec3 L = -light.direction.xyz;
    vec3 E = normalize(-fragVertex).xyz;


    vec3 TangentN = normalize(texture2D( normalmap, texc).rgb * 2.0 - 1.0);

    mat3 TBN = transpose(mat3(
                fragTangent,
                fragBitangent,
                fragNormal
        ));

    vec3 TangentL = normalize(TBN * L);
    vec3 TangentE = normalize(TBN * E);

    float cosTheta = clamp(dot(TangentN, TangentL), 0.0, 1.0);
    float cosTheta1 = clamp(dot(fragNormal.xyz, L), 0.0, 1.0);

    vec3 TangentR = -reflect(TangentL, TangentN);

    float cosAlpha = clamp(dot(TangentE, TangentR), 0.0, 1.0);

    vec4 textureColor = texture2D(texture, texc);

    //vec4 Iamb = 0.5 * (light.ambient + textureColor * length(light.ambient) * 0.333);
    vec4 Iamb = vec4(0.0, 0.0, 0.0, 0.0);

    vec4 Idiff = textureColor * cosTheta;
    //vec4 Idiff = vec4(0.0, 0.0, 1.0, 1.0) * cosTheta;
    //vec4 Idiff = vec4(0.0, 0.0, 1.0, 1.0) * clamp(dot(fragTangent.xyz, vec3(0.0, 0.0, 1.0)), 0.0, 1.0);

    Idiff = clamp(Idiff, 0.0, 1.0);

    vec4 Ispec = light.specular * pow(cosAlpha, 5.0);
    Ispec = clamp(Ispec, 0.0, 1.0);

    gl_FragColor = vec4((scene.backgroundColor + Iamb + Idiff).xyz, material.opacity);
}
