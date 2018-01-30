#version 450 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform int width;
out vec2 Tap[4], TapNeg[3];

void main(void)
{
	vec2 horzTapOffs[7];
	vec2 FragTexCoord = aTexCoords;

	float dx = 1.0 / float(width);
	horzTapOffs[0] = vec2(0.0, 0.0);
	horzTapOffs[1] = vec2(1.3366 * dx, 0.0);
	horzTapOffs[2] = vec2(3.4295 * dx, 0.0);
	horzTapOffs[3] = vec2(5.4264 * dx, 0.0);
	horzTapOffs[4] = vec2(7.4359 * dx, 0.0);
	horzTapOffs[5] = vec2(9.4436 * dx, 0.0);
	horzTapOffs[6] = vec2(11.4401 * dx, 0.0);

	Tap[0] = FragTexCoord;
	Tap[1] = FragTexCoord + horzTapOffs[1];
	Tap[2] = FragTexCoord + horzTapOffs[2];
	Tap[3] = FragTexCoord + horzTapOffs[3];

	TapNeg[0] = FragTexCoord - horzTapOffs[1];
	TapNeg[1] = FragTexCoord - horzTapOffs[2];
	TapNeg[2] = FragTexCoord - horzTapOffs[3];

	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
