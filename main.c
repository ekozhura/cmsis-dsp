#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "arm_math.h"                   // ARM::CMSIS:DSP

#define SIG_LENGTH 288
#define NUM_TAPS 29
#define BLOCK_SIZE 32

uint32_t numBlocks = SIG_LENGTH / BLOCK_SIZE;

extern void SystemClock_Config(void);
extern float32_t inputSignal_f32_1kHz_15kHz[SIG_LENGTH];

volatile float32_t inputSample, outputSample;

void plot_input_signal(void);
void plot_output_signal(void);
void plot_both_signal(void);

float32_t outputSignal_f32[SIG_LENGTH];
static float32_t firState32[BLOCK_SIZE + NUM_TAPS - 1];
	
	
const float32_t firCoeffs32[NUM_TAPS] = {
  -0.0018225230f, -0.0015879294f, +0.0000000000f, +0.0036977508f, +0.0080754303f, +0.0085302217f, -0.0000000000f, -0.0173976984f,
  -0.0341458607f, -0.0333591565f, +0.0000000000f, +0.0676308395f, +0.1522061835f, +0.2229246956f, +0.2504960933f, +0.2229246956f,
  +0.1522061835f, +0.0676308395f, +0.0000000000f, -0.0333591565f, -0.0341458607f, -0.0173976984f, -0.0000000000f, +0.0085302217f,
  +0.0080754303f, +0.0036977508f, +0.0000000000f, -0.0015879294f, -0.0018225230f
};

int main() 
{
	int i;
	arm_fir_instance_f32 _1kHz_15kHz_sig;
	
	HAL_Init();
	SystemClock_Config();
	
	arm_fir_init_f32(
		&_1kHz_15kHz_sig, 
		NUM_TAPS, 
		(float32_t *)&firCoeffs32[0], 
		(float32_t *)&firState32[0], 
		BLOCK_SIZE
	);
	
	for(i=0;i<numBlocks;i++) 
	{
		arm_fir_f32(
			&_1kHz_15kHz_sig,
			&inputSignal_f32_1kHz_15kHz[0] + (i * BLOCK_SIZE),
			&outputSignal_f32[0] + (i * BLOCK_SIZE),
			BLOCK_SIZE
		);
	}
	
	plot_both_signal();
	
	while(1)
	{
	}
}

void plot_input_signal(void)
{
	int i,j;
	for(i=0;i<SIG_LENGTH;i++) {
		inputSample = inputSignal_f32_1kHz_15kHz[i];
		for(j=0;j<3000;j++) {
			__nop();
		}
		if(i == SIG_LENGTH - 1) {
			i = 0;
		}
	}
}

void plot_output_signal(void)
{
	int i,j;
	for(i=0;i<SIG_LENGTH;i++) {
		outputSample = outputSignal_f32[i];
		for(j=0;j<3000;j++) {
			__nop();
		}
		if(i == SIG_LENGTH - 1) {
			i = 0;
		}
	}
}

void plot_both_signal(void)
{
	int i,j;
	for(i=0;i<SIG_LENGTH;i++) {
		inputSample = inputSignal_f32_1kHz_15kHz[i];
		outputSample = outputSignal_f32[i];
		for(j=0;j<5000;j++) {
			__nop();
		}
		if(i == SIG_LENGTH - 1) {
			i = 0;
		}
	}
}

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}
