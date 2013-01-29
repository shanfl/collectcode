#version 120

uniform sampler2D ColorBuffer;

void main()
{
	int Samples = 128;
	float Intensity = 0.125, Decay = 0.96875;
	vec2 TexCoord = gl_TexCoord[0].st, Direction = vec2(0.5) - TexCoord;
	Direction /= Samples;
	vec3 Color = texture2D(ColorBuffer, TexCoord).rgb;
	
	for(int Sample = 0; Sample < Samples; Sample++)
	{
		Color += texture2D(ColorBuffer, TexCoord).rgb * Intensity;
		Intensity *= Decay;
		TexCoord += Direction;
	}
	
	gl_FragColor = vec4(Color, 1.0);
}
