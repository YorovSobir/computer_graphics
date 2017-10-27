#version 450

uniform sampler1D tex;
uniform vec2 c;
uniform int maxIter;
uniform float radius;
in vec2 pos;
out vec4 color;

void main() {
        vec2 z;
        z.x = (pos.x - 0.5) * 3.0;
        z.y = (pos.y - 0.5) * 3.0;

        int i = 0;
        for(i = 0; i < maxIter; ++i) {
            float x = (z.x * z.x - z.y * z.y) + c.x;
            float y = (z.y * z.x + z.x * z.y) + c.y;

            if( (x * x + y * y) > radius) break;
            z.x = x;
            z.y = y;
        }
        color = texture(tex, (i >= maxIter ? 0.0 : float(i)) / 100.0);
}
