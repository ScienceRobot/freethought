#pragma once
 
#if WIN32
//#include "resource.h"
#endif


typedef struct _RobotStatus RobotStatus;

#define ROBOT_SOUND_OPEN 					0x00000001 //Sound is initialized and open
#define ROBOT_SND_COMPRESS					0x00000002  
#define ROBOT_READ_SND							0x00000004  //Read sound from sound input
#define ROBOT_PLAY_SND							0x00000008  //play sound data in SNDdata buffer
#define ROBOT_PLAY_TRACK_SND				0x00000010  //play sound data on tracks
#define ROBOT_SAROBOT_SND							0x00000020
#define ROBOT_STOPSAROBOT_SND					0x00000040
#define ROBOT_DRAW_SND_TO_TRACK		0x00000080
#define ROBOT_EXPORT_TRACK_SND			0x00000100  //export track data to file
#define ROBOT_USE_SND2							0x00000200
#define ROBOT_SND_PROCESS					0x00000400 //there is sound data to process (to save, play, send, etc)
#define ROBOT_SND_FILEOPEN					0x00000800  
#define ROBOT_SND2_FILEOPEN				0x00001000
#define ROBOT_PROCESS_USB					0x00002000 //process USB events thread is active
#define ROBOT_USB_OPEN							0x00004000
#define ROBOT_READ_USB							0x00008000
#define ROBOT_USB_OPEN_DEVICE			0x00010000  //connected with USB device

//SNDbits (SB) bits
#define ROBOT_SB_READ_SND					0x00000001 //SNDdata needs to be read or is reading
#define ROBOT_SB_PLAY_SND					0x00000002 //SNDdata needs to be played or is playing
#define ROBOT_SB_SAROBOT_SND					0x00000004 //SNDdata needs to be saved or is saving
#define ROBOT_SB_EXPORT_SND				0x00000008 //SNDdata needs to be exported or is exporting
#define ROBOT_SB_DRAW_SND_TO_TRACK	0x00000010 //SNDdata needs to be drawn or is drawing to track
#define ROBOT_SB_USE_SND2					0x00000020 //use SND2 buffer
#define ROBOT_SB_PROCESSED					0x00000040 //SND buffer was processed - do not process again

#define ROBOT_MAX_SND_POINTERS 100 //100 pointers into the SND buffer
#define ROBOT_MAX_SND_BUFFER 960000//48000 x 2 =1sec 25KB=10 seconds of WAV sound data
//#define ROBOT_SOUNDSIZE 100000//65536//16384//8096//65536//9600//131072//65536//16384//8192//96000 //5512  //buffer size of individual reads = 1 second of 48khz 2 channel wav data
#define ROBOT_DEFAULT_SOUND_BUFFER_SIZE 100000  //bytes to read or write from/to sound card

#define ROBOT_SNDTYPE_RAW 1
#define ROBOT_SNDTYPE_WAV 2
#define ROBOT_SNDTYPE_MP3 3





//global structure for VideoEdit Program
struct _RobotStatus
{
unsigned int Bits;  //Program Status bits/flags EButtonHold, etc...
//FTWindow *ButtonWin[4];  //window that has FTControl button 1,2,3 is being held down on is
int TimeSpace;   ///space between time unit lines (tcontrol->TimeScale is time units/pixel, TimeSpace is how many pixels).
long double CursorStartTime;  //currently from btnMarkIn
long double CursorEndTime; 
#if Linux
time_t ttime;  //stores time (for ctime)
#endif
struct tm *ctime;  //current time 10:14:32  should be pointer? 
//ctime->tm_year=100 (2000) ctime->tm_hour=6;  ctime->tm_min=0;  ctime->tm_sec=0;
//int secs; //for now only store seconds for expose
//time_t ttime2;  //stores time (for lastexpose)
//struct tm *lastexpose;  //redraw on expose only after 1 second
char SourceFolder[FTMedStr]; //location of source
char VideoDevice[FTMedStr];
char SpeakerDevice[FTMedStr];
char MicrophoneDevice[FTMedStr];
char RecordingSource[FTMedStr]; //"Audio", or "USB"
char HomeFolder[FTMedStr]; //location of $HOME/.videoedit
char TempFolder[FTMedStr]; //location of $HOME/.videoedit/temp
#if Linux
struct timeval utime;   //current time seconds and microseconds .tv_usec
struct timeval timertime; //last time timer check ran - runs every 1ms
#endif
long long timescale;  //for data FTControls. (in ns)
//Sound variables:
char SoundMethod[FTMedStr]; //Sound method="ALSA", "Pulse"
//Record and Play Data parameters are currently the same (project settings), but may later separate into 2 sets of variables
int SamplesPerSecond; 
int BitsPerSample;
int BytesPerSample;
int BitsPerChannel;
int Channels;
int Signed; //0=no 1=yes, data is signed
int RecordToTrack; //0=no,1=yes
int RecordToFile; //0=no,1=yes
char RecordToFileName[FTMedStr]; //Filename to record to if recording directly to file
char RecordToTempFileName[FTMedStr]; //temporary Filename to record to
char SoundCompression[FTMedStr]; //Sound compression= "No compression", "MP2 compression", "MP3 compression"
int DataFormat; //ROBOT_SNDTYPE_RAW, ROBOT_SNDTYPE_WAV, ROBOT_SNDTYPE_MP3
//Import Data parameters
char ImportSource[FTMedStr]; //ImportSource
int ImportDataFormat; //ROBOT_SNDTYPE_RAW, ROBOT_SNDTYPE_WAV, ROBOT_SNDTYPE_MP3
int ImportSamplesPerSecond; //RecordSamplesPerSecond, ImportSamplesPerSecond,ExportSamplesPerSecond
int ImportBitsPerSample;
int ImportBytesPerSample;
int ImportBitsPerTrack;
int ImportChannels;
int ImportSigned; //0=no 1=yes, data is signed
char ImportFromFileName[FTMedStr]; //Filename to record to if recording directly to file
char ImportFromTempFileName[FTMedStr]; //temporary Filename to record to
int ImportDataToTrack; //0=no 1=yes (no=only import to video window)
int ImportDataToImage; //0=no,1=yes (draws data as image in window-important for SETI work)
int ImportWidth; //width of image
int ImportHeight; //height of image
//Export Data parameters
int ExportSamplesPerSecond; //RecordSamplesPerSecond, ImportSamplesPerSecond,ExportSamplesPerSecond
int ExportBitsPerSample;
int ExportBytesPerSample;
int ExportBitsPerChannel;
int ExportChannels;
int ExportSigned; //0=no 1=yes, data is signed
char ExportToFileName[FTMedStr]; //Filename to export track data to
//char ExportToTempFileName[FTMedStr]; //temporary Filename to record to
int ExportSelected; //0=export all 1=export selected
char ExportSoundCompression[FTMedStr]; //Sound compression= "No compression", "MP2 compression", "MP3 compression"
int ExportDataFormat; //ROBOT_SNDTYPE_RAW, ROBOT_SNDTYPE_WAV, ROBOT_SNDTYPE_MP3
int NumSamples;
int SoundBufferSize; //possibly: RecordSoundBufferSize, PlaySoundBufferSize
int SpeakerFile;
int MicrophoneFile; 
//int PlayRecordedAudio; //0=no, 1=yes - it should not be necessary to play recorded audio while recording - simply set mixer to correct volume
char ALSASpeakerDevice[FTMedStr];
char ALSAMicrophoneDevice[FTMedStr];
FILE *AudioFile,*AudioFile2;
#if Linux
#if USE_SOUND
snd_pcm_t *hplay;//device handle
snd_pcm_t *hrec;//device handle
snd_pcm_hw_params_t *hw_params; //sound device hardware parameters
snd_pcm_stream_t alsa_play; //ALSA play stream
snd_pcm_stream_t alsa_rec; //ALSA record stream
//struct pollfd *ufds;  //for polling writing sound out - otherwise sound is choppy
//int count; //for polling
//PulseAudio
pa_simple *pas_play; //pulseaudio connection to server - play stream
pa_simple *pas_rec; //pulseaudio connection to server - record stream
pa_sample_spec pass; //pulseaudio sound specifications
#endif //USE_SOUND
//USB
struct libusb_device_handle *hUSB; //handle to USB device
struct libusb_transfer *bulk_transfer;  //USB bulk transfer buffer
struct libusb_transfer *ctrl_transfer;  //USB control transfer buffer
#endif
int EndPoint;  //USB EndPoint
int Interface; //USB Interface used
int AltInterface; //USB AltInterface of the Interface used
long long last_update_transferred;//transfered last time
long long submitted_bytes; //submitted for transfer
long long transferred_bytes;  //successfully transfered
int USBCurrentStatus; 
unsigned char *usbdata; //stores usb data
unsigned char *usbctrldata; //stores usb control packet data

#if Linux
pthread_mutex_t ROBOT_ReadSound_Lock; //mutex variable to indicate RecordSound thread is running
pthread_cond_t ROBOT_ReadSound_Cond; //conditional variable which indicates that recording is done
pthread_mutex_t ROBOT_SoundAccess_Lock; //mutex variable to indicate read or write of /dev/dsp device
//pthread_mutex_t ROBOT_startSND_Lock; //lock startSND variable to increment it
//pthread_cond_t ROBOT_startSND_Cond; //conditional variable which indicates that recording is done
pthread_mutex_t ROBOT_curSND_Lock; //lock curSND variable to increment it

pthread_mutex_t ROBOT_PlaySound_Lock;//indicates SND data needs to be played
pthread_cond_t ROBOT_PlaySound_Cond; //conditional variable which indicates that playing is done

pthread_mutex_t ROBOT_PlayTrackSound_Lock;//indicates Track data needs to be played
pthread_cond_t ROBOT_PlayTrackSound_Cond; //conditional variable which indicates that playing Track data is done

pthread_mutex_t ROBOT_ProcessData_SND_Lock; //indicates SND data needs to be processed
pthread_cond_t ROBOT_ProcessData_SND_Cond; //conditional variable that indicates that there is SND data that needs to be processed

pthread_mutex_t ROBOT_Process_USB_Lock; //indicates USB data needs to be processed
pthread_cond_t ROBOT_Process_USB_Cond; //conditional variable that indicates that there is USB data to process 

pthread_mutex_t ROBOT_CurrentStatus_USB_Lock; //to change the status of USB data
#endif

#if Linux
//SND DATA BUFFER
unsigned int SNDbits[ROBOT_MAX_SND_POINTERS]; //Bits describing how to process this data: Draw/Play, Save, Send, etc - uses same bits as VStatus.flags - but perhaps should use its own perhaps ROBOT_SNDbits_PLAY_SND, etc
unsigned char SNDdata[ROBOT_MAX_SND_BUFFER]; //(uncompressed) SND data buffer (WAV)
int SNDptr[ROBOT_MAX_SND_POINTERS]; //pointers into SND data buffer 
int SNDlen[ROBOT_MAX_SND_POINTERS]; //length of SND data this pointer contains
int SNDtype[ROBOT_MAX_SND_POINTERS]; //data subtype (pktWAV, pktMP3, etc) - this way one ProcessData_SND thread can handle all types
int startSND;  //first (earliest) pointer into uncompressed SND data buffer
int curSND;  //current (latest) pointer into uncompressed SND data buffer
//SND2 DATA BUFFER (Only used to store compressed or decompressed SND data and is not processed directly)
unsigned char SND2data[ROBOT_MAX_SND_BUFFER]; //(uncompressed) SND data buffer (WAV)
int SND2ptr[ROBOT_MAX_SND_POINTERS]; //pointers into wav data buffer 
int SND2len[ROBOT_MAX_SND_POINTERS]; //length of SND2 data this pointer contains
int SND2type[ROBOT_MAX_SND_POINTERS]; //data subtype (pktWAV, pktMP3, etc) - this way one ProcessData_SND 
unsigned char *data;//[ROBOT_SOUNDSIZE]; //used to store mixed SNDdata for final output to audio out
//unsigned char *data; //used as a pointer to mixed SNDdata for final output to audio out
#if USE_FFMPEG
//FFMPEG variables:
AVCodec *avc_enc;  //format to encode video call audio - currently set to MP3
AVCodecContext *avcc_enc; //audio encoding codec context 
AVOutputFormat *avof; //
AVFormatContext *avfc;  //
AVCodec *avc_dec;  //format to encode video call audio - currently set to MP3
AVCodecContext *avcc_dec; //audio decoding codec context 
AVPacket avpkt_dec; //packet for decoding mp3 audio to wav audio
int BitRate; //bitrate of mp3 compression (bits per second)
#endif //USE_FFMPEG
#endif //Linux
};


