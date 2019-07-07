#include "CommonUniforms.inc"

vec2 GetScreenPos () {
#ifdef WITH_OPENGL
	return (gl_FragCoord.xy*vec2(1,-1)) / commonData.screenSize.xx;
#else
	return (gl_FragCoord.xy) / commonData.screenSize.xx;
#endif
}