#version 450 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 Tap[4], TapNeg[3];
uniform int height;

void main(void)
{
	vec2 vertTapOffs[7];
	vec2 FragTexCoord = aTexCoords;

	float dy = 1.0 / float(height);
	vertTapOffs[0] = vec2(0.0, 0.0);
	vertTapOffs[1] = vec2(0.0, 1.3366 * dy);
	vertTapOffs[2] = vec2(0.0, 3.4295 * dy);
	vertTapOffs[3] = vec2(0.0, 5.4264 * dy);
	vertTapOffs[4] = vec2(0.0, 7.4359 * dy);
	vertTapOffs[5] = vec2(0.0, 9.4436 * dy);
	vertTapOffs[6] = vec2(0.0, 11.4401 * dy);

	Tap[0] = FragTexCoord;
	Tap[1] = FragTexCoord + vertTapOffs[1];
	Tap[2] = FragTexCoord + vertTapOffs[2];
	Tap[3] = FragTexCoord + vertTapOffs[3];

	TapNeg[0] = FragTexCoord - vertTapOffs[1];
	TapNeg[1] = FragTexCoord - vertTapOffs[2];
	TapNeg[2] = FragTexCoord - vertTapOffs[3];

	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
