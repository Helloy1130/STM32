//
// Created by Y on 2020/10/2.
//

#include "ECG_Process.h"
#include "ads1292r.h"

//Packet format
#define  Packet_Header_1      0xAA
#define  Packet_Header_2      0xAA
#define  Packet_Header_3      0xF1
#define  Packet_Header_4      0x08

#define TEMPERATURE         0
#define FILTERORDER         161
/* DC Removal Numerator Coeff*/
#define NRCOEFF     (0.992)
#define WAVE_SIZE   1

//******* ecg filter *********
#define MAX_PEAK_TO_SEARCH      5
#define MAXIMA_SEARCH_WINDOW    25
#define MINIMUM_SKIP_WINDOW     30
#define SAMPLING_RATE           125
#define TWO_SEC_SAMPLES         2 * SAMPLING_RATE
#define TRUE                    1
#define FALSE                   0

uint8_t DataPacketHeader[13];
uint8_t data_len = 7;

//************** ecg *******************
int16_t CoeffBuf_40Hz_LowPass[FILTERORDER] =
{
        -6,     10,     -3,     -9,     11,      0,    -12,     11,      4,
        -16,     10,      9,    -20,      8,     16,    -24,      3,     25,
        -27,     -4,     36,    -28,    -16,     47,    -24,    -32,     57,
        -15,    -51,     64,      0,    -73,     67,     23,    -96,     61,
        53,   -118,     46,     91,   -134,     19,    134,   -141,    -22,
        178,   -136,    -77,    222,   -113,   -146,    259,    -69,   -228,
        283,      0,   -319,    288,     99,   -416,    265,    232,   -515,
        204,    407,   -610,     87,    638,   -697,   -114,    957,   -771,
        -469,   1461,   -827,  -1215,   2547,   -862,  -4013,   9435,  20972,
        9435,  -4013,   -862,   2547,  -1215,   -827,   1461,   -469,   -771,
        957,   -114,   -697,    638,     87,   -610,    407,    204,   -515,
        232,    265,   -416,     99,    288,   -319,      0,    283,   -228,
        -69,    259,   -146,   -113,    222,    -77,   -136,    178,    -22,
        -141,    134,     19,   -134,     91,     46,   -118,     53,     61,
        -96,     23,     67,    -73,      0,     64,    -51,    -15,     57,
        -32,    -24,     47,    -16,    -28,     36,     -4,    -27,     25,
        3,    -24,     16,      8,    -20,      9,     10,    -16,      4,
        11,    -12,      0,     11,     -9,     -3,     10,     -6
};

int16_t ECG_WorkingBuff[2 * FILTERORDER];
unsigned char Start_Sample_Count_Flag = 0;
unsigned char first_peak_detect = FALSE ;
unsigned int sample_index[MAX_PEAK_TO_SEARCH + 2] ;
uint8_t indx = 0;

int cnt = 0;
volatile uint8_t flag = 0;

int QRS_Second_Prev_Sample = 0 ;
int QRS_Prev_Sample = 0 ;
int QRS_Current_Sample = 0 ;
int QRS_Next_Sample = 0 ;
int QRS_Second_Next_Sample = 0 ;

static uint16_t QRS_B4_Buffer_ptr = 0 ;
/*   Variable which holds the threshold value to calculate the maxima */
int16_t QRS_Threshold_Old = 0;
int16_t QRS_Threshold_New = 0;
unsigned int sample_count = 0 ;

/* Variable which will hold the calculated heart rate */
volatile uint16_t QRS_Heart_Rate = 0 ;
int16_t ecg_wave_buff[1], ecg_filterout[1];

volatile uint8_t global_HeartRate = 0;

uint8_t leadoff_deteted = 1;
volatile signed long s32DaqVals[8];

void ECG_FilterProcess(int16_t * WorkingBuff, int16_t * CoeffBuf, int16_t* FilterOut)
{
    int32_t acc = 0;   // accumulator for MACs
    int  k;

    // perform the multiply-accumulate
    for ( k = 0; k < 161; k++ )
    {
        acc += (int16_t)(*CoeffBuf++) * (int32_t)(*WorkingBuff--);
    }
    // saturate the result
    if ( acc > 0x3fffffff )
    {
        acc = 0x3fffffff;
    } else if ( acc < -0x40000000 )
    {
        acc = -0x40000000;
    }
    // convert from Q30 to Q15
//    *FilterOut = (int16_t)(acc >> 15);
    *FilterOut = (int16_t)(acc >> 10);
    //*FilterOut = *WorkingBuff;
}

void ECG_ProcessCurrSample(int16_t *CurrAqsSample, int16_t *FilteredOut)
{
    static uint16_t ECG_bufStart = 0, ECG_bufCur = FILTERORDER - 1, ECGFirstFlag = 1;
    static int16_t ECG_Pvev_DC_Sample, ECG_Pvev_Sample;/* Working Buffer Used for Filtering*/
    //  static short ECG_WorkingBuff[2 * FILTERORDER];
    int16_t *CoeffBuf;
    int16_t temp1, temp2, ECGData;

    /* Count variable*/
    uint16_t Cur_Chan;
    int16_t FiltOut = 0;
    //  short FilterOut[2];
    CoeffBuf = CoeffBuf_40Hz_LowPass;         // Default filter option is 40Hz LowPass

    if  ( ECGFirstFlag )                // First Time initialize static variables.
    {
        for ( Cur_Chan = 0 ; Cur_Chan < FILTERORDER; Cur_Chan++)
        {
            ECG_WorkingBuff[Cur_Chan] = 0;
        }
        ECG_Pvev_DC_Sample = 0;
        ECG_Pvev_Sample = 0;
        ECGFirstFlag = 0;
    }

    temp1 = NRCOEFF * ECG_Pvev_DC_Sample;       //First order IIR
    ECG_Pvev_DC_Sample = (CurrAqsSample[0]  - ECG_Pvev_Sample) + temp1;
    ECG_Pvev_Sample = CurrAqsSample[0];
    temp2 = ECG_Pvev_DC_Sample >> 2;
    ECGData = (int16_t) temp2;

    /* Store the DC removed value in Working buffer in millivolts range*/
    ECG_WorkingBuff[ECG_bufCur] = ECGData;
    ECG_FilterProcess(&ECG_WorkingBuff[ECG_bufCur], CoeffBuf, (int16_t*)&FiltOut);
    /* Store the DC removed value in ECG_WorkingBuff buffer in millivolts range*/
    ECG_WorkingBuff[ECG_bufStart] = ECGData;

    /* Store the filtered out sample to the LeadInfo buffer*/
    FilteredOut[0] = FiltOut ;//(CurrOut);

    ECG_bufCur++;
    ECG_bufStart++;

    if ( ECG_bufStart  == (FILTERORDER - 1))
    {
        ECG_bufStart = 0;
        ECG_bufCur = FILTERORDER - 1;
    }
    return ;
}

static void QRS_check_sample_crossing_threshold( uint16_t scaled_result )
{
    /* array to hold the sample indexes S1,S2,S3 etc */
    static uint16_t s_array_index = 0 ;
    static uint16_t m_array_index = 0 ;

    static unsigned char threshold_crossed = FALSE ;
    static uint16_t maxima_search = 0 ;
    static unsigned char peak_detected = FALSE ;
    static uint16_t skip_window = 0 ;
    static long maxima_sum = 0 ;
    static unsigned int peak = 0;
    static unsigned int sample_sum = 0;
    static unsigned int nopeak = 0;
    uint16_t Max = 0 ;
    uint16_t HRAvg;

    if ( TRUE == threshold_crossed  )
    {
        /*
        Once the sample value crosses the threshold check for the
        maxima value till MAXIMA_SEARCH_WINDOW samples are received
        */
        sample_count ++ ;
        maxima_search ++ ;

        if ( scaled_result > peak )
        {
            peak = scaled_result ;
        }

        if ( maxima_search >= MAXIMA_SEARCH_WINDOW )
        {
            // Store the maxima values for each peak
            maxima_sum += peak ;
            maxima_search = 0 ;

            threshold_crossed = FALSE ;
            peak_detected = TRUE ;
        }
    }
    else if ( TRUE == peak_detected )
    {
        /*
        Once the sample value goes below the threshold
        skip the samples untill the SKIP WINDOW criteria is meet
        */
        sample_count ++ ;
        skip_window ++ ;

        if ( skip_window >= MINIMUM_SKIP_WINDOW )
        {
            skip_window = 0 ;
            peak_detected = FALSE ;
        }

        if ( m_array_index == MAX_PEAK_TO_SEARCH )
        {
            sample_sum = sample_sum / (MAX_PEAK_TO_SEARCH - 1);
            HRAvg =  (uint16_t) sample_sum  ;

            // Compute HR without checking LeadOffStatus
            QRS_Heart_Rate = (uint16_t) 60 *  SAMPLING_RATE;
            QRS_Heart_Rate =  QRS_Heart_Rate / HRAvg ;
            if (QRS_Heart_Rate > 250)
                QRS_Heart_Rate = 250 ;
            //     Serial.println(QRS_Heart_Rate);

            /* Setting the Current HR value in the ECG_Info structure*/
            maxima_sum =  maxima_sum / MAX_PEAK_TO_SEARCH;
            Max = (int16_t) maxima_sum ;
            /*  calculating the new QRS_Threshold based on the maxima obtained in 4 peaks */
            maxima_sum = Max * 7;
            maxima_sum = maxima_sum / 10;
            QRS_Threshold_New = (int16_t)maxima_sum;

            /* Limiting the QRS Threshold to be in the permissible range*/
            if (QRS_Threshold_New > (4 * QRS_Threshold_Old))
            {
                QRS_Threshold_New = QRS_Threshold_Old;
            }

            sample_count = 0 ;
            s_array_index = 0 ;
            m_array_index = 0 ;
            maxima_sum = 0 ;
            sample_index[0] = 0 ;
            sample_index[1] = 0 ;
            sample_index[2] = 0 ;
            sample_index[3] = 0 ;
            Start_Sample_Count_Flag = 0;

            sample_sum = 0;
        }
    }
    else if ( scaled_result > QRS_Threshold_New )
    {
        /*
          If the sample value crosses the threshold then store the sample index
        */
        Start_Sample_Count_Flag = 1;
        sample_count ++ ;
        m_array_index++;
        threshold_crossed = TRUE ;
        peak = scaled_result ;
        nopeak = 0;

        /*  storing sample index*/
        sample_index[ s_array_index ] = sample_count ;
        if ( s_array_index >= 1 )
        {
            sample_sum += sample_index[ s_array_index ] - sample_index[ s_array_index - 1 ] ;
        }
        s_array_index ++ ;
    }
    else if (( scaled_result < QRS_Threshold_New ) && (Start_Sample_Count_Flag == 1))
    {
        sample_count ++ ;
        nopeak++;
        if (nopeak > (3 * SAMPLING_RATE))
        {
            sample_count = 0 ;
            s_array_index = 0 ;
            m_array_index = 0 ;
            maxima_sum = 0 ;
            sample_index[0] = 0 ;
            sample_index[1] = 0 ;
            sample_index[2] = 0 ;
            sample_index[3] = 0 ;
            Start_Sample_Count_Flag = 0;
            peak_detected = FALSE ;
            sample_sum = 0;

            first_peak_detect = FALSE;
            nopeak = 0;

            QRS_Heart_Rate = 0;
        }
    }
    else
    {
        nopeak++;
        if (nopeak > (3 * SAMPLING_RATE))
        {
            /* Reset heart rate computation sate variable in case of no peak found in 3 seconds */
            sample_count = 0 ;
            s_array_index = 0 ;
            m_array_index = 0 ;
            maxima_sum = 0 ;
            sample_index[0] = 0 ;
            sample_index[1] = 0 ;
            sample_index[2] = 0 ;
            sample_index[3] = 0 ;
            Start_Sample_Count_Flag = 0;
            peak_detected = FALSE ;
            sample_sum = 0;
            first_peak_detect = FALSE;
            nopeak = 0;
            QRS_Heart_Rate = 0;
        }
    }

    global_HeartRate = (uint8_t)QRS_Heart_Rate;
    //Serial.println(global_HeartRate);
}

static void QRS_process_buffer( void )
{
    int16_t first_derivative = 0 ;
    int16_t scaled_result = 0 ;

    static int16_t Max = 0 ;

    /* calculating first derivative*/
    first_derivative = QRS_Next_Sample - QRS_Prev_Sample  ;

    /*taking the absolute value*/
    if (first_derivative < 0)
    {
        first_derivative = -(first_derivative);
    }

    scaled_result = first_derivative;

    if ( scaled_result > Max )
    {
        Max = scaled_result ;
    }

    QRS_B4_Buffer_ptr++;
    if (QRS_B4_Buffer_ptr ==  TWO_SEC_SAMPLES)
    {
        QRS_Threshold_Old = ((Max * 7) / 10 ) ;
        QRS_Threshold_New = QRS_Threshold_Old ;
        // if (Max > 70)
        first_peak_detect = TRUE ;
        Max = 0;

        //  ecg_wave_buff[0] = first_derivative;
        QRS_B4_Buffer_ptr = 0;
    }

    if ( TRUE == first_peak_detect )
    {
        QRS_check_sample_crossing_threshold( scaled_result ) ;
    }
}

void QRS_Algorithm_Interface(int16_t CurrSample)
{
    //  static FILE *fp = fopen("ecgData.txt", "w");
    static int16_t prev_data[32] = {0};
    int16_t i;
    long Mac = 0;
    prev_data[0] = CurrSample;

    for ( i = 31; i > 0; i--)
    {
        Mac += prev_data[i];
        prev_data[i] = prev_data[i - 1];
    }
    Mac += CurrSample;
    Mac = Mac >> 2;
    CurrSample = (int16_t) Mac;
    QRS_Second_Prev_Sample = QRS_Prev_Sample ;
    QRS_Prev_Sample = QRS_Current_Sample ;
    QRS_Current_Sample = QRS_Next_Sample ;
    QRS_Next_Sample = QRS_Second_Next_Sample ;
    QRS_Second_Next_Sample = CurrSample ;

    QRS_process_buffer();
}

void ECG_Process(void)
{
    int j = 0;
    unsigned long uecgtemp = 0;
    signed long secgtemp = 0;
    long status_byte=0;
    uint8_t LeadStatus=0;

    for (int i = 3; i < 9; i += 3)         // data outputs is (24 status bits + 24 bits Respiration data +  24 bits ECG data)
    {
        uecgtemp = (unsigned long) (  ((unsigned long)ads1292_data_buf[i + 0] << 16) | ( (unsigned long) ads1292_data_buf[i + 1] << 8) |  (unsigned long) ads1292_data_buf[i + 2]);
        uecgtemp = (unsigned long) (uecgtemp << 8);
        secgtemp = (signed long) (uecgtemp);
        secgtemp = (signed long) (secgtemp >> 8);

        s32DaqVals[j++] = secgtemp;  //s32DaqVals[0] is Resp data and s32DaqVals[1] is ECG data
    }

    status_byte = (long)((long)ads1292_data_buf[2] | ((long) ads1292_data_buf[1]) <<8 | ((long) ads1292_data_buf[0])<<16); // First 3 bytes represents the status
    status_byte  = (status_byte & 0x0f8000) >> 15;  // bit15 gives the lead status
    LeadStatus = (unsigned char ) status_byte ;

    if((LeadStatus & 0x1f) != 0)
        leadoff_deteted  = 1;
    else
        leadoff_deteted  = 0;

    ecg_wave_buff[0] = (int16_t)(s32DaqVals[1] >> 8) ;  // ignore the lower 8 bits out of 24bits

    if(leadoff_deteted == 0)
    {
        ECG_ProcessCurrSample(&ecg_wave_buff[0], &ecg_filterout[0]);   // filter out the line noise @40Hz cutoff 161 order
        QRS_Algorithm_Interface(ecg_filterout[0]);             // calculate
    }
    else
        ecg_filterout[0] = 0;

    DataPacketHeader[0] = Packet_Header_1 ;   // Packet header1 :0xAA
    DataPacketHeader[1] = Packet_Header_2;    // Packet header2 :0xAA
    DataPacketHeader[2] = Packet_Header_3;    // Packet header3 :0xF1
    DataPacketHeader[3] = Packet_Header_4;    // Packet header4 :0x08

    //有符号数16位转有符号数32位传输
    DataPacketHeader[4] = (int32_t)ecg_filterout[0] >> 24;
    DataPacketHeader[5] = (int32_t)ecg_filterout[0] >> 16;
    DataPacketHeader[6] = (int32_t)ecg_filterout[0] >> 8;
    DataPacketHeader[7] = (int32_t)ecg_filterout[0];

    DataPacketHeader[8]  = s32DaqVals[0] >> 24;
    DataPacketHeader[9]  = s32DaqVals[0] >> 16;
    DataPacketHeader[10] = s32DaqVals[0] >> 8;
    DataPacketHeader[11] = s32DaqVals[0];

    if(leadoff_deteted == 1) // lead in not connected
        DataPacketHeader[12] = 0;
    else
        DataPacketHeader[12] = global_HeartRate;
}
