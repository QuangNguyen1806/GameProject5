#version 330

const float RED_LUM_CONSTANT = 0.2126;
const float GREEN_LUM_CONSTANT = 0.7152;
const float BLUE_LUM_CONSTANT = 0.0722;

uniform sampler2D texture0;
uniform vec2 lightPosition;
uniform float damageIntensity;      // NEW: 0.0 = no damage, 1.0 = full desaturation
uniform float damageFlashTime;      // NEW: For red pulse animation

in vec2 fragTexCoord;
in vec2 fragPosition;

out vec4 finalColor;

// Adjustable attenuation parameters
const float LINEAR_TERM    = 0.00003; // linear term
const float QUADRATIC_TERM = 0.00003; // quadratic term
const float MIN_BRIGHTNESS = 0.05;    // avoid total darkness

float attenuate(float distance, float linearTerm, float quadraticTerm)
{
    float attenuation = 1.0 / (1.0 + 
                               linearTerm * distance + 
                               quadraticTerm * distance * distance);

    return max(attenuation, MIN_BRIGHTNESS);
}

void main()
{
    vec4 color = texture(texture0, fragTexCoord);
    
    // NEW: DAMAGE LOGIC WITH IF-STATEMENT (REQUIREMENT)
    if (damageIntensity > 0.1) {
        // Player took damage - apply desaturation and red tint
        float grayscale = dot(color.rgb, vec3(RED_LUM_CONSTANT, GREEN_LUM_CONSTANT, BLUE_LUM_CONSTANT));
        vec3 desaturated = mix(color.rgb, vec3(grayscale), damageIntensity);
        
        // Add red pulse (oscillates with damageFlashTime)
        float redPulse = sin(damageFlashTime * 10.0) * 0.5 + 0.5;
        vec3 redTinted = mix(desaturated, vec3(1.0, 0.3, 0.3), damageIntensity * redPulse * 0.4);
        
        finalColor = vec4(redTinted, color.a);
    } else {
        // Normal mode - apply original lighting
        float distance = distance(lightPosition, fragPosition);
        float brightness = attenuate(distance, LINEAR_TERM, QUADRATIC_TERM);
        finalColor = vec4(color.rgb * brightness, color.a);
    }
}
