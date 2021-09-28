#version 330

precision highp float;

uniform vec2 u_resolution;

uniform sampler2D u_cur;

uniform float u_green;
uniform float u_blue;

int isPixelOn(vec2 pixOffset)
{
    return int(texture(u_cur, (gl_FragCoord.xy + pixOffset) / u_resolution).r);
}

void main(){
    vec2 p = gl_FragCoord.xy / u_resolution;

    vec2 rules[9] = vec2[9]
    (
        vec2(0,0), // 0 x = s / y = b
        vec2(0,0), // 1
        vec2(1,0), // 2
        vec2(0,1), // 3
        vec2(0,0), // 4
        vec2(0,0), // 5
        vec2(0,0), // 6
        vec2(0,0), // 7
        vec2(0,0)  // 8
    );

    int sum = 
        isPixelOn( vec2(1,0)  ) + // left
        isPixelOn( vec2(-1,0) ) + // right
        isPixelOn( vec2(0,1)  ) + // top
        isPixelOn( vec2(0,-1) ) + // bottom

        isPixelOn( vec2(1,1)  ) + // top left
        isPixelOn( vec2(1,-1) ) + // bot left
        isPixelOn( vec2(-1,1) ) + // top right
        isPixelOn( vec2(-1,-1));  // bot right

    vec4 image = texture(u_cur, p);

    float color = rules[sum].x * image.r + rules[sum].y;
    
    if (color > 0.)
        image.rgb = vec3(color);
    else
        image.rgb -= vec3(1.,0.03,0.03);
    
    gl_FragColor = image;
}
