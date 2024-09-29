#version 450
precision highp float;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec4 i_color;
layout(location = 2) in uint i_normalFlag;

layout(location = 0) out vec4 o_albedo;
layout(location = 1) out vec3 o_normal;

layout(binding = 0) uniform GlobalUniform
{
    mat4 view;
    mat4 proj;
    mat4 inv_view_proj;
    float farZ;
}
globalUniform;

layout(binding = 1) uniform ObjectUniform
{
    mat4 model;
}
objectUniform;

void main(void)
{
    vec4 worldPosition = objectUniform.model * vec4(i_position, 1);
    vec4 viewPosition = globalUniform.view * worldPosition;
    gl_Position = globalUniform.proj * viewPosition;
    o_albedo = i_color;
    vec3 normals[6] = {
        {-1,0,0},
        {1,0,0},
        {0,-1,0},
        {0,1,0},
        {0,0,-1},
        {0,0,1},
    };
    // vec3 normals[27] = {
    //     {-1,-1,-1},
    //     {0,-1,-1},
    //     {1,-1,-1},

    //     {-1,0,-1},
    //     {0,0,-1},
    //     {1,0,-1},

    //     {-1,1,-1},
    //     {0,1,-1},
    //     {1,1,-1},

    //     {-1,-1,0},
    //     {0,-1,0},
    //     {1,-1,0},

    //     {-1,0,0},
    //     {0,0,0},
    //     {1,0,0},

    //     {-1,1,0},
    //     {0,1,0},
    //     {1,1,0},

    //     {-1,-1,1},
    //     {0,-1,1},
    //     {1,-1,1},

    //     {-1,0,1},
    //     {0,0,1},
    //     {1,0,1},

    //     {-1,1,1},
    //     {0,1,1},
    //     {1,1,1},
    // };

    float minDotValue = 1;
    for(int i = 0; i < 6; i++) 
    {
        if((i_normalFlag & (1 << i)) != 0){
            vec4 n = globalUniform.view * vec4(normals[i], 1);
            float dotValue = dot(normalize(viewPosition.xyz), normalize(n.xyz));
            if(dotValue < minDotValue) {
                minDotValue = dotValue;
                o_normal = normalize(n.xyz);
            }
        }
    }
    gl_PointSize = (globalUniform.farZ / -viewPosition.z) * 1.5;
}