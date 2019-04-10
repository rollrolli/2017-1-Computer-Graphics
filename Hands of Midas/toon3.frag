// simple toon fragment shader
// www.lighthouse3d.com

uniform vec4 specColor;

varying vec3 normal, lightDir;

void main()
{
	float intensity;
	vec3 n;
	vec4 color;

	n = normalize(normal);
	intensity = max(dot(lightDir,n),0.0); 

	if (intensity > 0.98)
		color = specColor;
	else if (intensity > 0.75)
		color = colors[0];	
	else if (intensity > 0.25)
		color = colors[1];
	else
		color = colors[2];		

	gl_FragColor = color;
}
