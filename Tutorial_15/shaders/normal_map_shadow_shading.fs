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

varying highp vec4 lightVertex;
varying mediump vec2 texc;
varying highp vec3 fragVertex;
varying highp vec3 fragNormal;
varying highp vec3 fragTangent;
varying highp vec3 fragBitangent;

uniform sampler2D texture;
uniform sampler2D normalmap;
uniform sampler2D shadowmap;

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
    vec3 viewL = light.direction;
    vec3 viewE = normalize(-fragVertex);
    vec3 tangentN = normalize(texture2D( normalmap, texc).rgb * 2.0 - 1.0);

    mat3 TBN = transpose(mat3(
                fragTangent,
                fragBitangent,
                fragNormal
                ));

    vec3 tangentL = normalize(TBN * viewL);
    vec3 tangentE = normalize(TBN * viewE);
    vec3 tangentR = reflect(tangentL, tangentN);

    vec4 diffColor = texture2D(texture, texc);
    float cosTheta = clamp(dot(tangentN, -tangentL), 0.0, 1.0);
    vec4 Idiff = diffColor * cosTheta;
    Idiff = clamp(Idiff, 0.0, 1.0);

    float cosAlpha = clamp(dot(tangentE, tangentR), 0.0, 1.0);
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
