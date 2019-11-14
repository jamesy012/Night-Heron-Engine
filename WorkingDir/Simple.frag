#version 450						

 
in vec4 vVertColor;	

out vec4 fragColor; 

void main() {
	vec3 lightColor = vec3(1);
 
vec3 finalColor = vec3(0, 0, 0);
 vec3 fogColor = vec3(0.5);

//distance
float dist = 0;
float fogFactor = 0;

	dist = gl_FragCoord.z / gl_FragCoord.w; // dependence to current camera gl_Position
   // 20 - fog starts; 80 - fog ends
   fogFactor = (80 - dist)/(80 - 20);
   fogFactor = clamp( fogFactor, 0.0, 1.0 );
 
   //if you inverse color in glsl mix function you have to
   //put 1.0 - fogFactor
   finalColor = mix(fogColor, lightColor, fogFactor);

fragColor = vec4(finalColor,1);

   //fragColor = vec4(1,1,1,1);
	//fragColor = vVertColor;
} 
