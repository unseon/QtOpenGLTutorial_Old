uniform sampler2D texture;
varying mediump vec2 texc;
varying highp vec4 fragNormal;

void main() {
    vec3 camDir = vec3(0.0, 0.0, 1.0);

    float cosTheta = clamp(dot(camDir, fragNormal.xyz), 0.0, 1.0);
    gl_FragColor = vec4(texture2D(texture, texc).xyz * cosTheta, 1);
}
