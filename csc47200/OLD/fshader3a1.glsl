uniform   sampler2D u_Sampler;
void main() {
        gl_FragColor = texture2D(u_Sampler, gl_TexCoord[0].st);
}
