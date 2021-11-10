#include "aec.h"

#define NLMS_LEN 		1024
#define PROVISION		256
#define THRESHOLD		0.5f
#define ATTENUATION 0.5f
#define TIMEOUT			200

const float stepsize = 0.7f;
int 				hold_timer = 0;

int 	head = PROVISION;
float DC_offset = 0;
float signal_energy = 0;

float x[NLMS_LEN + PROVISION] 		= {0};
float abs_x[NLMS_LEN + PROVISION] = {0};
float weights[NLMS_LEN] 					= {0};
float xf[NLMS_LEN + PROVISION] 		= {0};
float u[14] 											= {0};

// pre-whiten param
const float a0 = 0.105831884f;
const float a1 = -0.105831884;
const float b1 = 0.78833646f;
float y = 0;
float z = 0;
float m = 0;
float n = 0;

// bandpass parameters
const float coef[14] = {
		// Kaiser Window FIR Filter, Filter type: High pass
		// Passband: 300.0 - 4000.0 Hz, Order: 12
		// Transition band: 100.0 Hz, Stopband attenuation: 10.0 dB
		-0.043183226f, -0.046636667f, -0.049576525f, -0.051936015f,
		-0.053661242f, -0.054712527f, 0.82598513f, -0.054712527f,
		-0.053661242f, -0.051936015f, -0.049576525f, -0.046636667f,
		-0.043183226f, 0.0f
};

float prewhiten_x(float in)
{
  // Chebyshev IIR filter, Filter type: HP
  // Passband: 3700 - 4000.0 Hz
  // Passband ripple: 1.5 dB, Order: 1
  float out = a0 * in + a1 * y + b1 * z;
  y = in;
  z = out;
  return out;
}

float prewhiten_e(float in)
{
  // Chebyshev IIR filter, Filter type: HP
  // Passband: 3700 - 4000.0 Hz
  // Passband ripple: 1.5 dB, Order: 1
  float out = a0 * in + a1 * m + b1 * n;
  m = in;
  n = out;
  return out;
}

float bandpass(float in) {
	memmove(u+1, u, 13*sizeof(float));
	u[0] = in;
	float sum0 = 0.0;
	float sum1 = 0.0;

	for (int i = 0; i < 14; i+= 2) {
		sum0 += coef[i] * u[i];
		sum1 += coef[i+1] * u[i+1];
	}
	return sum0+sum1;
}

bool dtd(float input, float output)
{
	abs_x[head] = fabs(input);
	float max = 0;
	float* max_val = &max;
	arm_max_no_idx_f32(abs_x + head, NLMS_LEN, max_val);

	// check dtd condition
	if ( fabs(output) > THRESHOLD * max)
		hold_timer = TIMEOUT;
	if (hold_timer) --hold_timer;

	return !(hold_timer > 0);
}

float nlms(float input, float output, bool update)
{
	x[head] = input;
	xf[head] = prewhiten_x(input);

	// apply echo cancel
	float echo = 0;
	float* echo_ptr = &echo;
	arm_dot_prod_f32(x + head, weights, NLMS_LEN, echo_ptr);
	float error = output - *echo_ptr;
	float ef = prewhiten_x(error);

	// iterate dot product
	signal_energy += (xf[head]*xf[head] - xf[head + NLMS_LEN - 1]*xf[head + NLMS_LEN - 1]);

	// update filter if no dtd
	if (update)
	{
		float n_stepsize = stepsize * ef / ( signal_energy + 1 );
		for (int i = 0; i < NLMS_LEN; i+=2)
		{
			weights[i] = n_stepsize * x[i + head];
			weights[i + 1] = n_stepsize * x[i + head + 1];
		}

		// NLP attenuates residual echo if no near talk
		// works well only if dtd is properly working
		error *= ATTENUATION;
	}

	return error;
}

void aec(int len)
{
	float in = 0;
	float out = 0;

	// compute DC value
	for (int i = 0; i < len; i+=2)
		DC_offset += (float)(ADC_buff[i] + ADC_buff[i + 1]);
	DC_offset /= len;

	for (int j = 0; j < len; j++)
	{
		in = (float)ADC_buff[j] - DC_offset;
		if (j > pts)
			out = (float)ADC_out[j] - gain*DC_offset;
		else
			out = (float)ADC_out[j] - DC_offset;

		bool update = dtd(in, out);
		ADC_out[j] = (uint16_t) bandpass( nlms(in, out, update) + DC_offset );

		// shift buffers back if full
		if (--head < 0)
		{
			head = PROVISION;
			memmove(x + head + 1, x, (NLMS_LEN - 1)*sizeof(float));
			memmove(abs_x + head + 1, abs_x, (NLMS_LEN - 1)*sizeof(float));
			memmove(xf + head + 1, xf, (NLMS_LEN - 1)*sizeof(float));
		}
	}
	// reset parameters
	head = PROVISION;
	DC_offset = 0;
	signal_energy = 0;
	hold_timer = 0;
	//v = 0;
	y = 0;
	z = 0;
	m = 0;
	n = 0;
	memset(u, 0, 14*sizeof(u));
}
