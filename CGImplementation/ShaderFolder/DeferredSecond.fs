// shadertype=glsl
// Deferred Second Pass Fragment Shader
// The lighting will be calculated in View Space.

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
  
    // Light Color
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;       
};

// Limit of the array size for each kind of light
#define NR_DIR_LIGHTS 10
#define NR_POINT_LIGHTS 30
#define NR_SPOT_LIGHTS 30

uniform int DIR_USED_NUM;
uniform DirLight dirLights[NR_DIR_LIGHTS];

uniform int POINT_USED_NUM;
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform int SPOT_USED_NUM;
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 albedo, float specular, vec3 fragpos, vec3 normal);
vec3 CalcPointLight(PointLight light, vec3 albedo, float specular, vec3 fragpos, vec3 normal);
vec3 CalcSpotLight(SpotLight light, vec3 albedo, float specular, vec3 fragpos, vec3 normal);

void main()
{
    // retrieve data from G-buffer
    vec4 MyBool = texture(gBool, TexCoords);
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    
    vec3 viewDir = normalize(-FragPos); // because this is in viewSpace!
	vec3 lighting = vec3(0.0);
    if(MyBool.a == 1)
    {
        // Light Map Calculation
        vec3 LMAlbedo = vec3(1.0); 
		float LMSpecular = 1.0; 
		vec3 LMemissive = vec3(0.0);
        if(MyBool.r == 1) LMAlbedo = texture(gAlbedoSpec, TexCoords).rgb;
        if(MyBool.g == 1) LMSpecular = texture(gAlbedoSpec, TexCoords).a;
        if(MyBool.b == 1) LMemissive = texture(gEmissive, TexCoords).rgb;
        
		for(int i = 0; i < DIR_USED_NUM; ++i)
            lighting += CalcDirLight(dirLights[i], LMAlbedo, LMSpecular, fragpos, normal);
        
        for(int i = 0; i < POINT_USED_NUM; ++i)
            lighting += CalcPointLight(pointLights[i], LMAlbedo, LMSpecular, fragpos, normal);

        for(int i = 0; i < SPOT_USED_NUM; ++i)
            lighting += CalcSpotLight(spotLights[i], LMAlbedo, LMSpecular, fragpos, normal);

        lighting += LMemissive;
    }
    else
    {
        // Color Material Calculation
        vec3 CMambient = texture(gBool, TexCoords).rgb;
        vec3 CMdiffuse = texture(gAlbedoSpec, TexCoords).rgb;
        float CMshininess = texture(gAlbedoSpec, TexCoords).a;
        vec3 CMspecular = texture(gEmissive, TexCoords).rgb;
        
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

vec3 CalcDirLight(DirLight light, vec3 albedo, float specular, vec3 fragpos, vec3 normal)
{
    // View Space + Blin-Phong Lighting
    vec3 lightDir = normalize(-light.Direction);
    vec3 viewDir = normalize(-fragpos);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
    
    // No attenuation for Directional Light
    // Then Combine the result
    vec3 ambient = light.Ambient * albedo;
    vec3 diffuse = light.Diffuse * diff * albedo;
    vec3 specular = light.Specular * spec * specular;
    return (ambient + diffuse + specular);
}