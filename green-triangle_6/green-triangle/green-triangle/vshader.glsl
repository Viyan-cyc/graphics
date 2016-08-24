#version 330

in vec4 vPosition;
in vec3 vNormal; 
in vec2 vTexCoord;
uniform vec3 theta;
uniform vec3 move;
uniform mat4 model_view,projection;
uniform mat4 model_move;
uniform vec4 r_AmbientProduct, r_DiffuseProduct, r_SpecularProduct;
uniform vec4 g_AmbientProduct, g_DiffuseProduct, g_SpecularProduct;
uniform vec4 b_AmbientProduct, b_DiffuseProduct, b_SpecularProduct;

uniform vec4 r_LightPosition;
uniform vec4 g_LightPosition;
uniform vec4 b_LightPosition;

uniform float Shininess;


out vec4 color;
out vec2 texCoord;

void main()
{
    
 
	gl_Position =projection * model_view * model_move * vPosition * vec4(0.4,0.4,0.4,1.0);
	
	vec3 pos = vec3((model_view * vPosition).xyz);
	vec3 r_L = normalize( r_LightPosition.xyz - pos ); // dirction
    vec3 r_E = normalize( -pos );
    vec3 r_H = normalize( r_L + r_E ); 

    // Transform vertex normal into eye coordinates
    vec3 N = normalize( model_view * vec4(vNormal, 0.0) ).xyz;

    // Compute terms in the illumination equation
    vec4 ambient = r_AmbientProduct + g_AmbientProduct + b_AmbientProduct;

    float r_Kd = max( dot(r_L, N), 0.0 );
    vec4  diffuse = r_Kd * r_DiffuseProduct;

    float r_Ks = pow( max(dot(N, r_H), 0.0), Shininess );
    vec4  specular = r_Ks * r_SpecularProduct;
    
    if( dot(r_L, N) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    vec3 g_L = normalize(g_LightPosition.xyz - pos);
    vec3 g_E = normalize( -pos );
    vec3 g_H = normalize( g_L + g_E ); 

    float g_Kd = max(dot(g_L, N), 0.0);
    diffuse += g_Kd * g_DiffuseProduct;

    float g_Ks = pow(max(dot(N, g_H), 0.0), Shininess);
    if (dot(g_L, N) < 0.0) {
        specular += vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        specular += g_Ks * g_SpecularProduct;
    }

    vec3 b_L = normalize(b_LightPosition.xyz - pos);
    vec3 b_E = normalize(-pos);
    vec3 b_H = normalize(b_L + b_E);

    float b_Kd = max(dot(b_L, N), 0.0);
    diffuse += b_Kd * b_DiffuseProduct;

    float b_Ks = pow(max(dot(N, b_H), 0.0), Shininess);
    if (dot(b_L, N) < 0.0) {
        specular += vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        specular += b_Ks * b_SpecularProduct;
    }

    color = ambient + diffuse + specular;
	
    color.a = 1.0;

	texCoord=vTexCoord;
}
