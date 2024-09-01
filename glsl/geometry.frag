#version 450
precision highp float;

<<<<<<< Updated upstream
layout(location = 1) in vec4 i_color;
=======
layout(location = 0) in vec4 i_albedo;
>>>>>>> Stashed changes
layout(location = 0) out vec4 o_albedo;

void main(void)
{
<<<<<<< Updated upstream
    o_albedo = i_color;
}
=======
    o_albedo = i_albedo;
}
>>>>>>> Stashed changes
