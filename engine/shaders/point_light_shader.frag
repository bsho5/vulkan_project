
#version 450                                                                             
                                                                                         
layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;
layout (location = 3) in vec3 normalOut;
layout (location = 4) in vec4 ioEyeSpacePosition;
                                                                        
layout ( location = 0 ) out vec4 color; 

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  vec4 ambientLightColor; // w is intensity
  vec3 lightPosition;
  mat4 invViewMatrix;
  vec3 cameraPosition;
  vec4 lightColor;
  float t;
} ubo;


layout(push_constant) uniform Push {

  mat4 modelMatrix;
  mat4 normalMatrix;

} push;

struct FogParameters
{
	vec3 color;
	float linearStart;
	float linearEnd;
	float density;
	
	int equation;
	bool isEnabled;
};


float getFogFactor(FogParameters params, float fogCoordinate)
{
	float result = 0.0;
	if(params.equation == 0)
	{
		float fogLength = params.linearEnd - params.linearStart;
		result = (params.linearEnd - fogCoordinate) / fogLength;
	}
	else if(params.equation == 1) {
		result = exp(-params.density * fogCoordinate);
	}
	else if(params.equation == 2) {
		result = exp(-pow(params.density * fogCoordinate, 2.0));
	}
	
	result = 1.0 - clamp(result, 0.0, 1.0);
	return result;
}
                                                                              
void main()                                                                               
{

vec3 lightDir = normalize((ubo.lightPosition) - (fragPosWorld)); // light Direction

// diffuse lighting
float attenuation = 1.0 / dot(lightDir, lightDir); // distance squared
vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * (attenuation);
vec3 diffuseLight =(lightColor * max(dot((fragNormalWorld), (lightDir)), 0));

//ambient lighting
vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;

// specular lighting

vec3 cameraPosWorld = ubo.invViewMatrix[3].xyz;
vec3 viewDir    = normalize(cameraPosWorld- (fragPosWorld));
vec3 halfwayDir = normalize(lightDir + viewDir);
float specularStrength = 4.0;
float shininess = 32.0;
float spec = pow(max(dot(fragNormalWorld, halfwayDir), 0.0), shininess);
vec3 specular = specularStrength * spec * ubo.lightColor.xyz;  



color = vec4((fragColor*(diffuseLight+ambientLight+specular)), 1.0);  
// FogParameters fogParams = FogParameters(vec3(0.05,0.05,0.05),200.0,500.0,0.1,0,true);
// float fogCoordinate = abs(ioEyeSpacePosition.z / ioEyeSpacePosition.w);
// color = mix(color, vec4(fogParams.color, 1.0), getFogFactor(fogParams, fogCoordinate));
                                                      

}