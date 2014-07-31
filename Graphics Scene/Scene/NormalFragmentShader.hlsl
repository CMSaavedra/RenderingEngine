#version 400

in vec2 TextureCoord;
in vec4 ShadowCoord;
in vec4 Position;
in vec3 Normal;
in mat3 TBNMatrix;

out vec4 FragColor;

uniform vec3 AmbientLight;
uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform vec3 EyeDirection;

uniform float LowThreshold;
uniform float HighThreshold;

uniform sampler2D NoiseTexture;
uniform sampler2D shadowMap;
uniform sampler2D NormalTexture;
uniform sampler2D defaultTexture;

//enable/disable lights
subroutine vec4 LightsOn(vec4 diffuseLight, vec4 specularLight);
subroutine uniform LightsOn lightsOn;

subroutine(LightsOn)
vec4 enableLights(vec4 diffuseLight, vec4 specularLight)
{
	return (vec4(AmbientLight,1) + diffuseLight + specularLight);
}
subroutine(LightsOn)
vec4 disableLights(vec4 diffuseLight, vec4 specularLight)
{
	return vec4(1,1,1,1);
}

void main() 
{	
	vec3 n = TBNMatrix * normalize(texture2D( NormalTexture, vec2(TextureCoord.x, -TextureCoord.y)).rgb * 2.0 - 1.0);
		
	//shadows
	float bias = 0.005;
	float visibility = 1.0;
	if ( texture( shadowMap, (ShadowCoord.xy/ShadowCoord.w)).z  <  (ShadowCoord.z - bias)/ShadowCoord.w)
	{
		visibility = 0.8;
	}

	//Light
	vec3 lightVector = normalize(LightPosition - vec3(Position));

	float diffuseBrightness = dot(n, lightVector);
	vec4 diffuseLight = vec4(diffuseBrightness);
	
	vec3 lightReflectedVector = reflect(-lightVector , n);
	vec3 eyeVector = normalize(EyeDirection - vec3(Position));
	
	float eyeAndReflectedVectorAligmentAmount = dot(lightReflectedVector, eyeVector);
	eyeAndReflectedVectorAligmentAmount = pow(eyeAndReflectedVectorAligmentAmount,10);
	
	vec4 specularLight = vec4(eyeAndReflectedVectorAligmentAmount);	

	specularLight = clamp(specularLight, 0, 1);	 

	//Noise texture
	vec4 noise = texture(NoiseTexture, TextureCoord);
		
	if(noise.a < LowThreshold) discard;
	if(noise.a > HighThreshold) discard;
	
	vec4 dTexture = texture(defaultTexture, TextureCoord);

	FragColor = dTexture * vec4(LightColor,1) * visibility * lightsOn(diffuseLight, specularLight);
}