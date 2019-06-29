#version 450						
out vec4 fragColor; 
 
in vec4 vVertColor;	


void main() {
	//fragColor = vec4(1,1,1,1);
	fragColor = vVertColor;
} 
