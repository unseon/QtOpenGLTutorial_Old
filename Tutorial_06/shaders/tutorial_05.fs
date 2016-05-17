uniform sampler2D texture;
varying mediump vec2 texc;

void main() {
    gl_FragColor = texture2D(texture, texc);
}
