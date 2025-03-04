uniform sampler2D texture;
uniform vec2 resolution;

void main()
{
    // Compute texture coordinates from the fragment coordinates.
    vec2 uv = gl_FragCoord.xy / resolution;
    vec4 texColor = texture2D(texture, uv);

    // Use smoothstep to threshold the alpha channel.
    float alpha = smoothstep(0.3, 0.5, texColor.a);
    
    // Output a water-like blue color modulated by the computed alpha.
    gl_FragColor = vec4(0.0, 0.5, 1.0, alpha);
}
