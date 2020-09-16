#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX

#include "lowpass.h"
#include "sine_gen.h"

#define SAMPLING_FREQ 1000
#define SIGNAL_FREQ 10
#define NOISE_FREQ 50

extern void SystemClock_Config(void);

osThreadId sineId, noiseId, disturbedId, filteredId,syncId;
uint32_t freq;
sine_generator_q15_t signal_set;
sine_generator_q15_t noise_set;
q15_t disturbed, filtered, noise, sine;

void sine_thread(void const *args);
void noise_thread(void const *args);
void disturbed_thread(void const *args);
void filtered_thread(void const *args);
void sync_thread(void const *args);

osThreadDef(sine_thread, osPriorityNormal, 1, 0);
osThreadDef(noise_thread, osPriorityNormal, 1, 0);
osThreadDef(disturbed_thread, osPriorityNormal, 1, 0);
osThreadDef(filtered_thread, osPriorityNormal, 1, 0);
osThreadDef(sync_thread, osPriorityNormal, 1, 0);

uint32_t t = 1;

void sine_thread(void const *args) 
{
	while(1)
	{
		osSignalWait(0x0001, 0);
		osDelay(t);
		sine = sine_calc_sample_q15(&signal_set) / 2;
		osSignalSet(noiseId, 0x0001);
	}
}

void noise_thread(void const *args) 
{
	while(1)
	{
		osSignalWait(0x0001, 0);
		osDelay(t);
		noise = sine_calc_sample_q15(&noise_set) / 6;
		osSignalSet(disturbedId, 0x0001);
	}
}

void disturbed_thread(void const *args) 
{
	while(1)
	{
		osSignalWait(0x0001, 0);
		osDelay(t);
		disturbed = sine + noise;
		osSignalSet(filteredId, 0x0001);
	}
}

void filtered_thread(void const *args) 
{
	while(1)
	{
		osSignalWait(0x0001, 0);
		osDelay(t);
		filtered = lowpass_filter(&disturbed);
		osSignalSet(syncId, 0x0001);
	}
}

void sync_thread(void const *args) 
{
	while(1)
	{
		osDelay(t);
		osSignalWait(0x0001, osWaitForever);
		osSignalSet(sineId, 0x0001);
	}
}

int main()
{
	
	HAL_Init();
	SystemClock_Config();
	
	freq = HAL_RCC_GetHCLKFreq();
	
	sine_generator_init_q15(&signal_set, SIGNAL_FREQ, SAMPLING_FREQ);
	sine_generator_init_q15(&noise_set, NOISE_FREQ, SAMPLING_FREQ);
	lowpass_filter_init();
	
	sineId = osThreadCreate(osThread(sine_thread), NULL);
	noiseId = osThreadCreate(osThread(noise_thread), NULL);
	disturbedId = osThreadCreate(osThread(disturbed_thread), NULL);
	filteredId = osThreadCreate(osThread(filtered_thread), NULL);
	syncId = osThreadCreate(osThread(sync_thread), NULL);

}
/*
void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}
*/
