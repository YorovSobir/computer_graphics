
#version 450 core
uniform sampler2D tex, texLow;
in vec2 TexCoords;
out vec4 FragColor;

void main (void)
{
	vec4 Fullres = texture(tex, TexCoords);
	vec4 Blurred = texture(texLow, TexCoords);

	// HLSL linear interpolation function
	FragColor = Fullres + Fullres.a * (Blurred - Fullres);
}



/*
#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D tex;
uniform sampler2D texLow;
uniform float radiusScale;

void main (void)
{
	const vec2 poisson1 = vec2 (  0.0,       0.0      );
	const vec2 poisson2 = vec2 (  0.527837, -0.85868  );
	const vec2 poisson3 = vec2 ( -0.040088,  0.536087 );
	const vec2 poisson4 = vec2 ( -0.670445, -0.179949 );
	const vec2 poisson5 = vec2 ( -0.419418, -0.616039 );
	const vec2 poisson6 = vec2 (  0.440453, -0.639399 );
	const vec2 poisson7 = vec2 ( -0.757088,  0.349334 );
	const vec2 poisson8 = vec2 (  0.574619,  0.685879 );

	vec2 p = TexCoords.st;
	vec4 c = texture(tex, p);
	float cd = c.a;
	float discRadius = cd * 10.0 / 512.0;
	float discRadiusLow = discRadius * 0.4 * 4.0;
	vec4 tapLow, tapHigh, tap;
	float blur;

	c = vec4(0.0);

											// sample 1
	tapLow   = texture(texLow, p + poisson1 * discRadiusLow);
	tapHigh  = texture(tex,    p + poisson1 * discRadiusLow);
	blur     = tapHigh.a;
	tap      = mix(tapHigh, tapLow, blur);

											// apply leaking reduction
	tap.a  = (tap.a >= cd ? 1.0 : tap.a);
	c.rgb += tap.rgb * tap.a;
	c.a   += tap.a;

											// sample 2
	tapLow   = texture(texLow, p + poisson2 * discRadiusLow );
	tapHigh  = texture(tex,    p + poisson2 * discRadiusLow );
	blur     = tapHigh.a;
	tap      = mix(tapHigh, tapLow, blur);

											// apply leaking reduction
	tap.a  = ( tap.a >= cd ? 1.0 : tap.a );
	c.rgb += tap.rgb * tap.a;
	c.a   += tap.a;

											// sample 3
	tapLow   = texture(texLow, p + poisson3 * discRadiusLow);
	tapHigh  = texture(tex,    p + poisson3 * discRadiusLow);
	blur     = tapHigh.a;
	tap      = mix(tapHigh, tapLow, blur);

											// apply leaking reduction
	tap.a  = (tap.a >= cd ? 1.0 : tap.a);
	c.rgb += tap.rgb * tap.a;
	c.a   += tap.a;

											// sample 4
	tapLow   = texture(texLow, p + poisson4 * discRadiusLow);
	tapHigh  = texture(tex,    p + poisson4 * discRadiusLow);
	blur     = tapHigh.a;
	tap      = mix(tapHigh, tapLow, blur);

											// apply leaking reduction
	tap.a  = (tap.a >= cd ? 1.0 : tap.a);
	c.rgb += tap.rgb * tap.a;
	c.a   += tap.a;

											// sample 5
	tapLow   = texture(texLow, p + poisson5 * discRadiusLow);
	tapHigh  = texture(tex,    p + poisson5 * discRadiusLow);
	blur     = tapHigh.a;
	tap      = mix(tapHigh, tapLow, blur);

											// apply leaking reduction
	tap.a  = (tap.a >= cd ? 1.0 : tap.a);
	c.rgb += tap.rgb * tap.a;
	c.a   += tap.a;

											// sample 6
	tapLow   = texture(texLow, p + poisson6 * discRadiusLow);
	tapHigh  = texture(tex,    p + poisson6 * discRadiusLow);
	blur     = tapHigh.a;
	tap      = mix(tapHigh, tapLow, blur);

											// apply leaking reduction
	tap.a  = (tap.a >= cd ? 1.0 : tap.a);
	c.rgb += tap.rgb * tap.a;
	c.a   += tap.a;

											// sample 7
	tapLow   = texture(texLow, p + poisson7 * discRadiusLow);
	tapHigh  = texture(tex,    p + poisson7 * discRadiusLow);
	blur     = tapHigh.a;
	tap      = mix(tapHigh, tapLow, blur);

											// apply leaking reduction
	tap.a  = (tap.a >= cd ? 1.0 : tap.a);
	c.rgb += tap.rgb * tap.a;
	c.a   += tap.a;

											// sample 8
	tapLow   = texture(texLow, p + poisson8 * discRadiusLow);
	tapHigh  = texture(tex,    p + poisson8 * discRadiusLow);
	blur     = tapHigh.a;
	tap      = mix(tapHigh, tapLow, blur);

											// apply leaking reduction
	tap.a  = (tap.a >= cd ? 1.0 : tap.a);
	c.rgb += tap.rgb * tap.a;
	c.a   += tap.a;

	FragColor = c / c.a;
}
*/