uniform samplerCube Cubemap;
in vec3 Reflect;
in vec3 Refract;
in float Ratio;
out vec4 FragColor;
void main()
{
 vec3 refractColor = vec3(texture(Cubemap, Refract));
 vec3 reflectColor = vec3(texture(Cubemap, Reflect));
 vec3 color = mix(refractColor, reflectColor, Ratio);
 FragColor = vec4(color, 1.0);
}

