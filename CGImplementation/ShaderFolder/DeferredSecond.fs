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

struct Light
{
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
    float Radius;
};

const int NR_LIGHTS = 200;
uniform Light lights[NR_LIGHTS];

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
        
		lighting = LMAlbedo * 0.1; // hard-coded ambient component
		
        for(int i = 0; i < NR_LIGHTS; ++i)
        {
            // calculate distance between light source and current fragment
            float dist = length(lights[i].Position - FragPos);
            
            if(dist < lights[i].Radius)
            {
                // diffuse
                vec3 lightDir = (lights[i].Position - FragPos) * (1.0 / dist);
                vec3 diffuse = max(dot(Normal, lightDir), 0.0) * LMAlbedo * lights[i].Color;
                
                vec3 halfwayDir = normalize(lightDir + viewDir);
                float spec = pow(max(dot(Normal, halfwayDir), 0.0), 128.0);
                vec3 specular = spec * LMSpecular * lights[i].Color;

                float attenuation = (1.0) / (1.0 + lights[i].Linear * dist + lights[i].Quadratic * dist * dist);

                diffuse *= attenuation;
                specular *= attenuation;

                lighting += diffuse + specular;
            }    
        }
        lighting += LMemissive;
    }
    else
    {
        // Color Material Calculation
        vec3 CMambient = texture(gBool, TexCoords).rgb;
        vec3 CMdiffuse = texture(gAlbedoSpec, TexCoords).rgb;
        float CMshininess = texture(gAlbedoSpec, TexCoords).a;
        vec3 CMspecular = texture(gEmissive, TexCoords).rgb;

        lighting = CMambient;
        
        for(int i = 0; i < NR_LIGHTS; ++i)
        {
            float dist = length(lights[i].Position - FragPos);
            
            if(dist < lights[i].Radius)
            {
                vec3 lightDir = (lights[i].Position - FragPos) * (1.0 / dist);
                vec3 diffuse = max(dot(Normal, lightDir), 0.0) * CMdiffuse * lights[i].Color;

                vec3 halfwayDir = normalize(lightDir + viewDir);
                float spec = pow(max(dot(Normal, halfwayDir), 0.0), CMshininess * 128); // 128 should be set according to article
                vec3 specular = spec * CMspecular * lights[i].Color;
                
                float attenuation = (1.0) / (1.0 + lights[i].Linear * dist + lights[i].Quadratic * dist * dist);
                
                diffuse *= attenuation;
                specular *= attenuation;

                lighting += diffuse + specular;
            }
        }
    }


    // post=processing for HDR with tone mapping and gamma corection
    const float gamma = 2.2;
    const float exposure = 1.0;

    vec3 hdrColor = lighting;
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
	// FragColor = vec4(lighting, 1.0);
}