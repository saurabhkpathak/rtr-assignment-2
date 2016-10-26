in vec4 MCvertex;
in vec3 MCnormal;
uniform mat4 MVMatrix;
uniform mat4 MVPMatrix;
uniform mat3 NormalMatrix;
uniform mat4 TextureMatrix;
const float Eta = 0.66; // Ratio of indices of refraction
const float FresnelPower = 5.0;
const float F = ((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta) * (1.0+Eta));
out vec3 Reflect;
out vec3 Refract;
out float Ratio;
void main()
{
 vec4 ecPosition = MVMatrix * MCvertex;
 vec3 ecPosition3 = ecPosition.xyz / ecPosition.w;
 vec3 i = normalize(ecPosition3);
 vec3 n = normalize(NormalMatrix * MCnormal);
 Ratio = F + (1.0 - F) * pow((1.0 - dot(-i, n)), FresnelPower);
 Refract = refract(i, n, Eta);
 Refract = vec3(TextureMatrix * vec4(Refract, 1.0));
 Reflect = reflect(i, n);
 Reflect = vec3(TextureMatrix * vec4(Reflect, 1.0));
 gl_Position = MVPMatrix * MCvertex;
}
