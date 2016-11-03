#version 330

in vec3 Normal;
in vec3 Position;
out vec4 color;

uniform vec3 cameraPos;
uniform samplerCube skybox;

const float EtaR = 0.65;
const float EtaG = 0.67;
const float EtaB = 0.69;
const float F = ((1.0-EtaG) * (1.0-EtaG)) / ((1.0+EtaG) * (1.0+EtaG));

void main()
{
	vec3 incident = normalize(Position - cameraPos);
	vec3 N = normalize(Normal);
	vec3 Reflect = reflect(incident,N);
	vec3 RefractR = refract(incident,N, EtaR);
	vec3 RefractG = refract(incident,N, EtaG);
	vec3 RefractB = refract(incident,N, EtaB);

	vec4 refractColor;
	refractColor.r = texture(skybox, RefractR).r;
	refractColor.g = texture(skybox, RefractG).g;
	refractColor.b = texture(skybox, RefractB).b;

	vec4 reflectColor = texture(skybox, Reflect);

	float Ratio = F + (1.0 - F) * pow((1.0 - dot(-incident,N)), 5.0);

	color = mix(refractColor, reflectColor, Ratio);
}