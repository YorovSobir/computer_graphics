#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D tex;

void main()
{
	const vec2	d1 = vec2 ( 1.0/512.0, 1.0/512.0 );
	const vec2	d2 = vec2 ( 1.0/512.0, -1.0/512.0 );
	const vec2	d3 = vec2 ( -1.0/512.0, 1.0/512.0 );
	const vec2	d4 = vec2 ( -1.0/512.0, -1.0/512.0 );

    vec2 p = TexCoords.st;
	FragColor = (texture(tex, vec2(p + d1)) +
                       texture(tex, vec2(p + d2)) +
					   texture(tex, vec2(p + d3)) +
			           texture(tex, vec2(p + d4))) * 0.25;
}
