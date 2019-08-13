#version 450						

 
in vec4 vVertColor;	

out vec4 fragColor; 

void main() {
	//fragColor = vec4(1,1,1,1);
	fragColor = vVertColor;
} 
