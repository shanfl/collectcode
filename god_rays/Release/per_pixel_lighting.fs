#version 120

varying vec3 LD, Normal, LDR;

void main()
{
	vec3 LDN = normalize(LD);
	vec3 NormalN = normalize(Normal);
	vec3 LDRN = normalize(LDR);
	float NdotLD = max(dot(NormalN, LDN), 0.0);
	float EVdotLDR = pow(max(dot(LDN, LDRN), 0.0), 32.0);
	gl_FragColor = vec4(gl_Color.rgb * NdotLD + vec3(EVdotLDR), 1.0);
}
