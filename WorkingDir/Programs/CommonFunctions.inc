#include "CommonUniforms.inc"

//layout(origin_upper_left) in vec4 gl_FragCoord;
layout(location=4) in vec2 vPosTest;


vec2 GetScreenPos () {
return vPosTest;
}