#version 130

uniform vec4 DiffuseColor;
uniform vec4 SecondaryColor;
uniform sampler2D DiffuseTexture;
uniform sampler2D AlphaTexture;
uniform bool DoGrayscaleGradient;
uniform bool UseAlphaTexture;

in vec3 fPosition;
in vec4 fColor;
in vec2 fTexCoord;

out vec4 frag_colour;

void main()
{
	vec4 sampleColor = fColor * texture2D(DiffuseTexture, fTexCoord);
	
	if (DoGrayscaleGradient)
	{
		float gray = dot(sampleColor.rgb, vec3(0.299, 0.587, 0.114));
		frag_colour = mix(DiffuseColor.rgba, SecondaryColor.rgba, gray);
		// frag_colour = vec4(fTexCoord.x, fTexCoord.y, 0.0, 1.0);
	}
	else
	{
		frag_colour = DiffuseColor * sampleColor;
	}
	
	if (UseAlphaTexture)
	{
		frag_colour.a *= texture2D(AlphaTexture, fTexCoord).a;
	}
}