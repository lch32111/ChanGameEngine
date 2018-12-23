// shadertype=glsl
// Deferred Second Pass Fragment Shader
// The lighting will be calculated in World Space.

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gEmissive;
uniform sampler2D gBool;

struct DirLight
{
    vec3 Direction;

    // Light Color
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
	float Intensity;

    int ShadowIndex;
};

struct PointLight {
    vec3 Position;
    
    // Attenuation
    float Constant;
    float Linear;
    float Quadratic;
    float Radius;
	
    // Light Color
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
	float Intensity;

    int ShadowIndex;
};

struct SpotLight {
    vec3 Position;
    vec3 Direction;

    // Spot Light CutOff
    float Inner_CutOff;
    float Outer_CutOff;
  
    // Attenuation
    float Constant;
    float Linear;
    float Quadratic;
    float Radius;
	float Intensity;
  
    // Light Color
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;       

    int ShadowIndex;
};

// Limit of the array size for each kind of light
#define NR_DIR_LIGHTS 3
#define NR_DIR_SHADOWS 2

#define NR_POINT_LIGHTS 15
#define NR_POINT_SHADOWS 4

#define NR_SPOT_LIGHTS 10
#define NR_SPOT_SHADOWS 4

#define SHADOW_INDEX_NONE -1

uniform int DIR_USED_NUM;
uniform DirLight dirLights[NR_DIR_LIGHTS];
uniform sampler2D dirShadowMap[NR_DIR_SHADOWS];
uniform mat4 dirLightSpace[NR_DIR_SHADOWS];
uniform float dirBias[NR_DIR_SHADOWS];

uniform int POINT_USED_NUM;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform samplerCube pointShadowMap[NR_POINT_SHADOWS];
uniform float pointFarPlane[NR_POINT_SHADOWS];
uniform float pointBias[NR_POINT_SHADOWS];

uniform int SPOT_USED_NUM;
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform sampler2D spotShadowMap[NR_SPOT_SHADOWS];
uniform mat4 spotLightSpace[NR_SPOT_SHADOWS];
uniform float spotBias[NR_SPOT_SHADOWS];

uniform vec3 cameraPos;

// function prototypes
vec3 CalcLMDirLight(DirLight light, vec3 albedo, float spclr, float shininess, vec3 fragpos, vec3 normal);
vec3 CalcLMPointLight(PointLight light, vec3 albedo, float spclr, float shininess, vec3 fragpos, vec3 normal);
vec3 CalcLMSpotLight(SpotLight light, vec3 albedo, float spclr, float shininess, vec3 fragpos, vec3 normal);
vec3 CalcCMDirLight(DirLight light, vec3 ambnt, vec3 albedo, vec3 spclr, float shininess, vec3 fragpos, vec3 normal);
vec3 CalcCMPointLight(PointLight light, vec3 ambnt, vec3 albedo, vec3 spclr, float shininess, vec3 fragpos, vec3 normal);
vec3 CalcCMSpotLight(SpotLight light, vec3 ambnt, vec3 albedo, vec3 spclr, float shininess, vec3 fragpos, vec3 normal);
float DirShadowCalculation(vec3 normal, vec3 lightDir, vec3 fragpos, int index);
float PointShadowCalculation(vec3 fragpos, vec3 lightpos, int index);
float SpotShadowCalculation(vec3 fragpos, int index);

void main()
{
    // retrieve data from G-buffer
    vec4 MyBool = texture(gBool, TexCoords).rgba;
    if(MyBool.a >= 1.0) discard;
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    
	vec3 lighting = vec3(0.0);
    if(MyBool.a - 0.5 >= 0.0)
    {
        // Light Map Calculation
        vec3 LMAlbedo = vec3(1.0); 
		float LMSpecular = 1.0; 
		vec3 LMemissive = vec3(0.0);
        if(MyBool.r == 1) LMAlbedo = texture(gAlbedoSpec, TexCoords).rgb;
        if(MyBool.g == 1) LMSpecular = texture(gAlbedoSpec, TexCoords).a;
        if(MyBool.b == 1) LMemissive = texture(gEmissive, TexCoords).rgb;
        
		for(int i = 0; i < DIR_USED_NUM; ++i)
            lighting += CalcLMDirLight(dirLights[i], LMAlbedo, LMSpecular, 128, FragPos, Normal);
        
        for(int i = 0; i < POINT_USED_NUM; ++i)
            lighting += CalcLMPointLight(pointLights[i], LMAlbedo, LMSpecular, 128, FragPos, Normal);

        for(int i = 0; i < SPOT_USED_NUM; ++i)
            lighting += CalcLMSpotLight(spotLights[i], LMAlbedo, LMSpecular, 128, FragPos, Normal);

        lighting += LMemissive;
    }
    else
    {
        // Color Material Calculation
        vec3 CMambient = vec3(MyBool.rgb);
        vec3 CMdiffuse = texture(gAlbedoSpec, TexCoords).rgb;
        float CMshininess = texture(gAlbedoSpec, TexCoords).a;
        vec3 CMspecular = texture(gEmissive, TexCoords).rgb;
        
		for(int i = 0; i < DIR_USED_NUM; ++i)
            lighting += CalcCMDirLight(dirLights[i], CMambient, CMdiffuse, CMspecular, CMshininess * 128.0, FragPos, Normal);
        
        for(int i = 0; i < POINT_USED_NUM; ++i)
            lighting += CalcCMPointLight(pointLights[i], CMambient, CMdiffuse, CMspecular, CMshininess * 128.0, FragPos, Normal);

        for(int i = 0; i < SPOT_USED_NUM; ++i)
            lighting += CalcCMSpotLight(spotLights[i], CMambient, CMdiffuse, CMspecular, CMshininess * 128.0, FragPos, Normal);

    }

    // post-processing for HDR with tone mapping and gamma corection
    const float gamma = 2.2;
    const float exposure = 1.0;

    vec3 hdrColor = lighting;
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
	// FragColor = vec4(lighting, 1.0);
}

vec3 CalcLMDirLight(DirLight light, vec3 albedo, float spclr, float shininess, vec3 fragpos, vec3 normal)
{
    // World Space + Blinn-Phong Lighting
    vec3 lightDir = normalize(-light.Direction);
    vec3 viewDir = normalize(cameraPos - fragpos);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);	
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    
    // No attenuation for Directional Light
    // Then Combine the result
    vec3 ambient = light.Ambient * albedo;
    vec3 diffuse = light.Diffuse * diff * albedo;
    vec3 specular = light.Specular * spec * spclr;
	
	// Calculate Shadow
    float shadow = 0.f;
    if(light.ShadowIndex != SHADOW_INDEX_NONE)
	    shadow = DirShadowCalculation(normal, lightDir, fragpos, light.ShadowIndex);

    return (ambient + (1.0 - shadow) * (diffuse + specular)) * light.Intensity;
}

vec3 CalcLMPointLight(PointLight light, vec3 albedo, float spclr, float shininess, vec3 fragpos, vec3 normal)
{
	// World Space + Blinn-Phong Lighting
	
	// Early Exit
	vec3 FragToLight = light.Position - fragpos;
	float SquaredDist = dot(FragToLight, FragToLight);
	if(SquaredDist > light.Radius * light.Radius) return vec3(0);

	// Calculate point light
	float dist = sqrt(SquaredDist);
	vec3 lightDir = FragToLight * (1.0 / dist);

	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	
	// specular shading
	vec3 viewDir = normalize(cameraPos - fragpos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

	// attenuation
	float attenuation = 1.0 / (light.Constant + light.Linear * dist + light.Quadratic * dist * dist);
	
	// combine the result
	vec3 ambient = light.Ambient * albedo;
	vec3 diffuse = light.Diffuse * diff * albedo;
	vec3 specular = light.Specular * spec * spclr;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

    float shadow = 0.0;
    if(light.ShadowIndex != SHADOW_INDEX_NONE)
	    shadow = PointShadowCalculation(fragpos, light.Position, light.ShadowIndex);

    return (ambient + (1.0 - shadow) * (diffuse + specular)) * light.Intensity;
}

vec3 CalcLMSpotLight(SpotLight light, vec3 albedo, float spclr, float shininess, vec3 fragpos, vec3 normal)
{
	// World Space + Blinn-Phong Lighting
	
	// Early Exit
	vec3 FragToLight = light.Position - fragpos;
	float SquaredDist = dot(FragToLight, FragToLight);
	if(SquaredDist > light.Radius * light.Radius) return vec3(0);

	// Calculate Spot Light
	float dist = sqrt(SquaredDist);
	vec3 lightDir = FragToLight * (1.0 / dist);
	
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 viewDir = normalize(cameraPos - fragpos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

	// attenuation
	float attenuation = 1.0 / (light.Constant + light.Linear * dist + light.Quadratic * dist * dist);

	// smooth the spotlight
	float theta = dot(lightDir, normalize(-light.Direction));
	float epsilon = light.Inner_CutOff - light.Outer_CutOff;
	float spot_intensity = clamp((theta - light.Outer_CutOff) / epsilon, 0.0, 1.0);

	// combine the result
	vec3 ambient = light.Ambient * albedo;	
	vec3 diffuse = light.Diffuse * diff * albedo;
	vec3 specular = light.Specular * spec * spclr;

	ambient *= attenuation * spot_intensity;
	diffuse *= attenuation * spot_intensity;
	specular *= attenuation * spot_intensity;

    float shadow = 0.0;
    if(light.ShadowIndex != SHADOW_INDEX_NONE)
        shadow = SpotShadowCalculation(fragpos, light.ShadowIndex);

	return  (ambient + (1.0 - shadow) * (diffuse + specular)) * light.Intensity;
}

vec3 CalcCMDirLight(DirLight light, vec3 ambnt, vec3 albedo, vec3 spclr, float shininess, vec3 fragpos, vec3 normal)
{
    // World Space + Blinn-Phong Lighting
    vec3 lightDir = normalize(-light.Direction);
    vec3 viewDir = normalize(cameraPos - fragpos);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    
    // No attenuation for Directional Light
    // Then Combine the result
    vec3 ambient = light.Ambient * ambnt;
    vec3 diffuse = light.Diffuse * diff * albedo;
    vec3 specular = light.Specular * spec * spclr;

	// Calculate Shadow
    float shadow = 0.f;
    if(light.ShadowIndex != SHADOW_INDEX_NONE)
	    shadow = DirShadowCalculation(normal, lightDir, fragpos, light.ShadowIndex);

    return (ambient + (1.0 - shadow) * (diffuse + specular)) * light.Intensity;
}

vec3 CalcCMPointLight(PointLight light, vec3 ambnt, vec3 albedo, vec3 spclr, float shininess, vec3 fragpos, vec3 normal)
{
	// World Space + Blinn-Phong Lighting
	
	// Early Exit
	vec3 FragToLight = light.Position - fragpos;
	float SquaredDist = dot(FragToLight, FragToLight);
	if(SquaredDist > light.Radius * light.Radius) return vec3(0);

	// Calculate point light
	float dist = sqrt(SquaredDist);
	vec3 lightDir = FragToLight * (1.0 / dist);

	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	
	// specular shading
	vec3 viewDir = normalize(cameraPos - fragpos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

	// attenuation
	float attenuation = 1.0 / (light.Constant + light.Linear * dist + light.Quadratic * dist * dist);
	
	// combine the result
	vec3 ambient = light.Ambient * ambnt;
	vec3 diffuse = light.Diffuse * diff * albedo;
	vec3 specular = light.Specular * spec * spclr;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
    float shadow = 0.0;
    if(light.ShadowIndex != SHADOW_INDEX_NONE)
	    shadow = PointShadowCalculation(fragpos, light.Position, light.ShadowIndex);

    return (ambient + (1.0 - shadow) * (diffuse + specular)) * light.Intensity;
}

vec3 CalcCMSpotLight(SpotLight light, vec3 ambnt, vec3 albedo, vec3 spclr, float shininess, vec3 fragpos, vec3 normal)
{
	// World Space + Blinn-Phong Lighting
	
	// Early Exit
	vec3 FragToLight = light.Position - fragpos;
	float SquaredDist = dot(FragToLight, FragToLight);
	if(SquaredDist > light.Radius * light.Radius) return vec3(0);

	// Calculate Spot Light
	float dist = sqrt(SquaredDist);
	vec3 lightDir = FragToLight * (1.0 / dist);
	
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 viewDir = normalize(cameraPos - fragpos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

	// attenuation
	float attenuation = 1.0 / (light.Constant + light.Linear * dist + light.Quadratic * dist * dist);

	// smooth the spotlight
	float theta = dot(lightDir, normalize(-light.Direction));
	float epsilon = light.Inner_CutOff - light.Outer_CutOff;
	float spot_intensity = clamp((theta - light.Outer_CutOff) / epsilon, 0.0, 1.0);

	// combine the result
	vec3 ambient = light.Ambient * ambnt;
	vec3 diffuse = light.Diffuse * diff * albedo;
	vec3 specular = light.Specular * spec * spclr;

	ambient *= attenuation * spot_intensity;
	diffuse *= attenuation * spot_intensity;
	specular *= attenuation * spot_intensity;

    float shadow = 0.0;
    if(light.ShadowIndex != SHADOW_INDEX_NONE)
        shadow = SpotShadowCalculation(fragpos, light.ShadowIndex);

	return  (ambient + (1.0 - shadow) * (diffuse + specular)) * light.Intensity;
}

float DirShadowCalculation(vec3 normal, vec3 lightDir, vec3 fragpos, int index)
{
	// transform world into lightspace
	vec4 fragPosLightSpace = dirLightSpace[index] * vec4(fragpos, 1.0);
	
	// perform perspective divide (range -1 ~ 1)
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
	// adjust the range from 0 ~ 1
	projCoords = projCoords * 0.5 + 0.5;
	
	float currentDepth = projCoords.z;
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), dirBias[index]);

	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(dirShadowMap[index], 0);

    // Percentage-Closer Filtering
	for(int x = -1; x <=1; ++x)
      for(int y = -1; y <= 1; ++y)
	{
		float pcfDepth = texture(dirShadowMap[index], projCoords.xy + vec2(x,y) * texelSize).r;
		shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
	}
	shadow /= 9.0;

	if(projCoords.z > 1.0) shadow = 0.0;
	return shadow;
}

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float PointShadowCalculation(vec3 fragpos, vec3 lightpos, int index)
{
    // get vector between fragment position and light position
    vec3 fragLight = fragpos - lightpos;
    float currentDepth = length(fragLight);
	float viewDistance = length(cameraPos - fragpos);
	int samples = 20;
	float bias = pointBias[index];
	float diskRadius = (1.0 + viewDistance / pointFarPlane[index]) / 25.0;
	float shadow = 0.0;

	// PCF
	for(int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(pointShadowMap[index], fragLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= pointFarPlane[index]; // Undo Mapping [0;1]
		if(currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	shadow /= float(samples);

    return shadow;
}

float SpotShadowCalculation(vec3 fragpos, int index)
{
    vec4 fragPosLightSpace = spotLightSpace[index] * vec4(fragpos, 1.0);

    // perform perspective divide (range -1 ~ 1)
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
	// adjust the range from 0 ~ 1
	projCoords = projCoords * 0.5 + 0.5;
	
	float currentDepth = projCoords.z;
	float bias = spotBias[index];

	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(spotShadowMap[index], 0);

    // Percentage-Closer Filtering
	for(int x = -1; x <=1; ++x)
      for(int y = -1; y <= 1; ++y)
	{
		float pcfDepth = texture(spotShadowMap[index], projCoords.xy + vec2(x,y) * texelSize).r;
		shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
	}
	shadow /= 9.0;

	if(projCoords.z > 1.0) shadow = 0.0;
	return shadow;
}