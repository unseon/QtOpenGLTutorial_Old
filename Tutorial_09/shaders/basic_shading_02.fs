struct Light {
    vec4 position;
    vec4 direction;
    float intensity;
};

varying mediump vec2 texc;
varying highp vec4 fragNormal;

uniform sampler2D texture;
uniform Light light;
uniform vec3 lightDirection;

void main() {
    vec3 camDir = normalize(vec3(1.0, -1.0, -1.0));

    float cosTheta = clamp(dot(-lightDirection, fragNormal.xyz), 0.0, 1.0);
    gl_FragColor = vec4(texture2D(texture, texc).xyz * cosTheta, 1);
}
