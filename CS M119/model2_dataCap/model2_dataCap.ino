#include <arduinoFFT.h>
#include <PDM.h>

#define ADC_BUF_SIZE  16384 // ~1 second
#define SCL_INDEX     0x00
#define SCL_TIME      0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT      0x03

#define K ADC_BUF_SIZE/2+1

// buffer to read samples into, each sample is 16-bits
unsigned short ADC_buff[ADC_BUF_SIZE];
// Real and Imaginary vectors
double vReal[ADC_BUF_SIZE];
double vImag[ADC_BUF_SIZE];
// Mel-Frequency Cepstrum Coefficients
float MFCC[13];

// FFT Object
arduinoFFT FFT = arduinoFFT(); 

void setup() {
  Serial.begin(9600);
  while (!Serial);
  PDM.setBufferSize(ADC_BUF_SIZE);
  // configure the data receive callback
  PDM.onReceive(onPDMdata);
  if (!PDM.begin(1, 16000)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }
}

void loop() {
  Serial.println("a");
  delay(500);
}

void onPDMdata() {
  // query the number of bytes available
  int bytesAvailable = PDM.available();

  // read into the sample buffer
  PDM.read(ADC_buff, bytesAvailable);
  
  // get MFCCs
  get_MFCC();
}

void get_MFCC() {
  /*
   *  STEP 1: Window filtering
   */
  // Subtract the average value
  float avg_val;
  for(int i=0; i<ADC_BUF_SIZE; i++)
  {
    avg_val += ADC_buff[i];
  }
  avg_val /= ADC_BUF_SIZE;
  // Window filtering
  float window[ADC_BUF_SIZE];
  for(int i=0; i<ADC_BUF_SIZE; i++)
  {
    window[i] = 0.54 - 0.46 * cos(2*PI*i / (ADC_BUF_SIZE-1));
  }
  /*
   * STEP 2: Periodogram computation
   */
  // Convert to complex array
  for(int i = 0; i< ADC_BUF_SIZE; i++)
  {
    vReal[i] = ((float)ADC_buff[i] - avg_val) * window[i];
    vImag[i] = 0;
  }
  // Perform FFT
  FFT.Compute(vReal, vImag, ADC_BUF_SIZE, FFT_FORWARD);
  // K points of the magnitude squared
  float mag_sq_input[ADC_BUF_SIZE] = {0};
  for(int i=0; i<ADC_BUF_SIZE; i++)
  {
    mag_sq_input[i] = powf(vReal[i],2)+powf(vImag[i],2);
  }
  float K_input[K];
  for(int i=0; i<K; i++)
  {
    K_input[i] = mag_sq_input[i];
  }
  /*
   *  STEP 3: Filter bank construction
   */
  // Mel freq based on phi_0 and phi_(M+1)
  float f_mel[28] = {0};
  f_mel[0]  = 2595 * log10f(1+250/700.f);
  f_mel[27] = 2595 * log10f(1+8e3/700.f);
  float f_mel_div = (f_mel[27]-f_mel[0])/27;
  // M frequencies and inverse mapping
  float f_phi[28] = {0};
  for(int i=0; i<28; i++)
  {
    f_mel[i] = f_mel[0] + f_mel_div * i;
    f_phi[i] = 700 * (powf(10, f_mel[i]/2595) - 1);
  }
  // Lambda calculation
  float lambda[28] = {0};
  for(int i=0; i<28; i++)
  {
    lambda[i] = (int)(513 * f_phi[i] / 8e3);
  }
  // Construction of the filter bank
  float filter[26][K] = {0};
  for(int filter_index=0; filter_index<26; filter_index++) // iteration per filter
  {
    float f_min_index = lambda[filter_index];
    float f_mid_index = lambda[filter_index+1];
    float f_max_index = lambda[filter_index+2];
    for(int f_PSD_index=0; f_PSD_index<K; f_PSD_index++) // iteration per PSD frequency
    {
      // f < lambda_(m-1) OR f > lambda_(m+1)
      if(f_PSD_index < f_min_index || f_PSD_index > f_max_index)
      {
        filter[filter_index][f_PSD_index] = 0;
      }
      // 0 to 1 linear interpolation lambda_(m-1) <= f <= lambda_(m)
      else if(f_PSD_index >= f_min_index && f_PSD_index <= f_mid_index)
      {
        filter[filter_index][f_PSD_index] = (f_PSD_index - f_min_index ) / (f_mid_index - f_min_index);
      }
      // 1 to 0 linear interpolation lambda_(m) <= f <= lambda_(m+1)
      else if(f_PSD_index >= f_mid_index && f_PSD_index <= f_max_index)
      {
        filter[filter_index][f_PSD_index] = 1 - (f_PSD_index - f_mid_index ) / (f_max_index - f_mid_index);
      }
    }
  }
  /*
   *  STEP 4 & 5: Filter bank application & Logarithmic energy output
   */
  float K_energy[26] = {0};
  for(int i=0; i<26; i++) // filter
  {
    for(int j=0; j<K; j++) // K elements
    {
      K_energy[i] += K_input[j] * filter[i][j];
    }
    K_energy[i] = log10f(K_energy[i]);
  }
  /*
   *  STEP 6: MFCC values
   */
  for(int i=0; i<13; i++) // MFCC index
  {
    for(int j=0; j<26; j++) // Energy element
    {
      MFCC[i] += K_energy[j] * cos((i+1) * ((j+1) - 0.5) * M_PI / 26);
    }
  }
  for (int i=0; i<13; i++) {
    Serial.print(MFCC[i]);
    if (i < 12) Serial.print(", ");
  }
}
