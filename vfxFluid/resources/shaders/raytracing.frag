#version 450

layout (binding = 0) uniform sampler3D densityImage;
layout (binding = 1) uniform sampler3D opacityImage;
layout (binding = 2) uniform sampler3D obstacleImage;
layout (binding = 3) uniform sampler3D temperatureImage;
layout (binding = 4) uniform sampler2D depthImage;
layout (binding = 5) uniform sampler3D densityNoBlurImage;

// Light properties
uniform vec3 lightColor;
uniform float lightIntensity;
uniform float lightAbsorbtion;

// Tracing
uniform float jitter;
uniform int samples;
uniform float stepSize;
uniform vec2 framebufferSize;
uniform mat4 invModelViewProjMatrix;

// Features
/* Debug mode rendering flag
 * 0 - supress debug mode
 * 1 - render front faces
 * 2 - render back faces
 * 3 - render view vectors (direction texture)
 */
uniform int domainDebugMode = 3;	
uniform int enableShadows;
uniform int enableRadiance;
uniform int enableScattering;

// Render properties
uniform float radianceColorFallOff;
uniform float densityCoefficient;

out vec4 outputColor;

const float PI = 3.1415926535897932384626433832795;
const vec3 OBSTACLE_COLOR = vec3(1.0);
const vec3 AMBIENT_LIGHT = vec3(0.15, 0.15, 0.20);

// Structures
struct Ray
{
	vec3 origin;
	vec3 direction;
};

struct AABB
{
	vec3 min;
	vec3 max;
};

// Helpers
bool IntersectBox(Ray r, AABB box, out vec2 tOut)
{
    vec3 invR = 1.0 / r.direction;
    vec3 tbot = invR * (box.min - r.origin);
    vec3 ttop = invR * (box.max - r.origin);
    vec3 tmin = min(ttop, tbot);
    vec3 tmax = max(ttop, tbot);
    vec2 t = max(tmin.xx, tmin.yz);
    tOut.x = max(t.x, t.y);
    t = min(tmax.xx, tmax.yz);
    tOut.y = min(t.x, t.y);

	if(tOut.x < 0) tOut.x = 0;

	return tOut.x <= tOut.y;
}

// Generates rancom float number from given 3D coordinate
highp float generateNumber(vec3 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co, vec3(a,b,c));
    highp float sn= mod(dt, 3.14);
    return fract(sin(sn) * c);
}

vec4 debugDomain(vec3 front, vec3 back)
{
	if(domainDebugMode == 1)
		return vec4(front, 1.0);
	else if (domainDebugMode == 2)
		return vec4(back, 1.0);
	if(domainDebugMode == 3)
	{
		vec3 direction = normalize(back - front);
		return vec4(direction, length(back - front));
	}

	return vec4(1.0);
}

vec3 computeLighting(vec3 position, float alpha)
{
	return lightColor * ((enableShadows == 1) ? texture(opacityImage, position).x : (1.0 * lightIntensity)) * alpha * stepSize;
}

bool isTextureCoordinateValid(vec3 tc)
{
	if (tc.x < 0 || tc.y < 0 || tc.z < 0 ) return false;
	if (tc.x > 1 || tc.y > 1 || tc.z > 1 ) return false;

	return true;
}

void main()
{
	vec4 eyeDirection;
	eyeDirection.xy = 2.0 * gl_FragCoord.xy / framebufferSize - 1.0;
	eyeDirection.z = - 1 / tan(PI * 30.0 / 180.0);
	eyeDirection.x *= framebufferSize.x / framebufferSize.y;
	
	vec3 direction = normalize((invModelViewProjMatrix * eyeDirection).xyz);
	vec3 eye = (invModelViewProjMatrix * vec4(0,0,0,1)).xyz;
	
	Ray cameraRay = Ray(eye, direction);
	AABB box = AABB(vec3(0), vec3(1));
	
	vec2 t;
	if(!IntersectBox(cameraRay, box, t)) discard;
	
	vec3 start = eye + direction * t.x;
	vec3 end = eye + direction * t.y;

	if(domainDebugMode > 0 && domainDebugMode <= 3)
	{
		outputColor = debugDomain(start, end);
	}
	else
	{
		vec3 textureCoordinate = start + mix(-jitter / 2.0, jitter / 2.0, generateNumber(start)) * stepSize * direction;

		float T = 1.0;
		vec3 T0 = AMBIENT_LIGHT;

		vec3 rayDirection = normalize(end - start);
		float remainingRayDistance = distance(end, start);
		vec3 step = stepSize * rayDirection;

		for(int i = 0; i < samples && remainingRayDistance > 0.0; ++i, remainingRayDistance -= stepSize)
		{
			textureCoordinate += step;
			if(!isTextureCoordinateValid(textureCoordinate)) break;

			vec3 lightSample = computeLighting(textureCoordinate, T);

			if (texture(obstacleImage, textureCoordinate).x > 0.2)
			{
				T0 += OBSTACLE_COLOR * lightSample;
				T = 0;
				break;
			}

			vec4 density = texture(densityImage, textureCoordinate) * densityCoefficient;
			float totalDensity = density.x + density.y + density.z;

			if (totalDensity < 0.01) continue;

			float temperature = texture(temperatureImage, textureCoordinate).x;

			if(enableScattering == 1)
			{
				vec3 densityNoBlurSample = texture(densityNoBlurImage, textureCoordinate).xyz * densityCoefficient;
				density.xyz *= densityNoBlurSample;
			}
				
			if(enableRadiance == 1) density.xyz = mix(density.xyz, vec3(0.0) , exp(-temperature*temperature/radianceColorFallOff));

			T0 += lightSample * density.xyz;
			T *= exp(-totalDensity * stepSize * lightAbsorbtion);

			if (T < 0.01) break;

		}

	
		outputColor = vec4(T0, 1 - T);
	}
}
