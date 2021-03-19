//FT_Utils.c
/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
*/
// FT_Utils.c: FreeThought Library Utility functions.
//
#include "freethought.h"
#include "FT_Utils.h"
#ifdef WIN32
#include <time.h>
#endif

extern ProgramStatus PStatus;

#ifdef WIN32
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;
 
  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);
 
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;
 
    /*converting file time to unix epoch*/
    tmpres /= 10;  /*convert into microseconds*/
	tmpres -= DELTA_EPOCH_IN_MICROSECS; 
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }
 
  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }
 
  return 0;
}
#if 0 
//    #include <windows.h>
     
int gettimeofday( struct timeval *tv, struct timezone *tz )
{
time_t rawtime;
LARGE_INTEGER tickPerSecond;
LARGE_INTEGER tick; // a point in time
//SYSTEMTIME stime;

/*
GetSystemTime(&stime);
tv->tv_sec=stime.wSecond;
tv->tv_usec=stime.wMilliseconds*1000;
*/

//somehow with this code- the microseconds could be > 999999
//this is because gettimeofday "gives the number of seconds and microseconds since the Epoch"
//presumably they are equivalent (not to be added)
time(&rawtime);
tv->tv_sec = (long)rawtime;
     
// here starts the microsecond resolution:
     
     
// get the high resolution counter's accuracy
QueryPerformanceFrequency(&tickPerSecond);
     
// what time is it ?
QueryPerformanceCounter(&tick);
     
// and here we get the current microsecond
tv->tv_usec = (long int)(tick.QuadPart % tickPerSecond.QuadPart);  //note LONGLONG to long loss of data


return 0;
}
#endif
#endif // _WIN32_

void GetTime(void)
{
//fprintf(stderr,"GetTime\n");
PStatus.ttime=time(NULL);
PStatus.ctime=localtime(&PStatus.ttime);  //is mallocing?
gettimeofday(&PStatus.utime,NULL);
}  //end GetTime



//Formated time
//itime is the time in seconds
//This is used only for TimeLine controls - perhaps FT_FormatTimeLine
void FT_FormatTime(char *text,long double itime,int format,long double TimeScale,int TimeSpace)
{
int hrs,mns,sec,msec,usec,nsec;
//FTControl *tcontrol;
char units[10];
long double timeunit;

//probably each CTTimeLine should have a TimeSpace value

//itime is time in floating point, for example 100.0= 100 seconds, 0.001 = 1ms
//fprintf(stderr,"ns2ht\n");


hrs=(int)(itime/3600.0);
itime-=hrs*3600;
mns=(int)(itime/60.0);
itime-=mns*60;
sec=(int)(itime);
itime-=sec;
msec=(int)(itime*1000.0);
itime-=(msec*0.001);
usec=(int)(itime*1000000.0);
itime-=(usec*0.000001);
nsec=(int)(itime*1000000000.0);


//fprintf(stderr,"in ns2tm usec=%lli\n",usec);
//  fprintf(stderr,"%i:%02i:%02i.%03i",hrs,mns,sec,msec);
if (format==1)
{
//if (hrs==0 && mns==0 && sec==0)
//  sprintf(text,"0.%03lli %03lli %03lli",msec,usec,nsec);
//else
  sprintf(text,"%i:%02i:%02i.%03i",hrs,mns,sec,msec);
}
else
{

//tcontrol=GetFTControl("btnTimeLine");
//fprintf(stderr,"Timescale= %Lg\n",tcontrol->TimeScale);

//timeunit is = to the space between two lines on the timeline
//timeunit=tcontrol->TimeScale*tcontrol->TimeSpace;
timeunit=((long double)TimeScale)*TimeSpace;
//fprintf(stderr,"timeunit=%Lg\n",timeunit);
//what time is displayed depends on the scale of resolution
//all larger resolutions are displayed too, although perhaps this can be a setting - to ignore the higher scale values at small scale
//at scale=1 day, we do not display hours, minutes, etc
//at scale=1 second we do not display milliseconds
//at scale=1 ms we do not display us
//at scale=1 us we do not display ns

//TimeScale is in seconds
strcpy(units,"");
strcpy(text,"");
//scale is >= 1 day or there is at least 1 day on the timeline
if (timeunit>=86400.0 || hrs>86400)	{
	if (hrs!=0) { //dont bother to print if no hours
		if ((hrs/24)<24) {
			sprintf(text,"%i day ",hrs/24);
		} else {
			sprintf(text,"%i days ",hrs/24);
		}
		hrs-=hrs/24;
	}
//  sprintf(text,"%i days %i:%02i:%02i",hrs/24,hrs-hrs/24,mns,sec);
}

//scale is >= 1 second or there is at least 1 second on the timeline
if (timeunit>=1.0 || sec>0 || mns>0 || hrs>0)	{
	if (!(sec==0 && mns==0 && hrs==0)) { //dont bother to print if no seconds minutes or hours
		if (hrs>0) {
			sprintf(text,"%i:%02i:%02i",hrs,mns,sec);
		} else {
			if (mns>0) {
				sprintf(text,"%i:%02i",mns,sec);
			} else {
				if (sec>0) {
					sprintf(text,"%i",sec);
					strcpy(units,"s");
				} //sec>0
			} //mns>0
		} //hrs>0
	} //!(sec==0 && min==0
}  //TimeScale>=1.0 || sec>0

//scale is 1-999 milliseconds
//fprintf(stderr,"tcontrol->TimeScale=%Lg timescale>=0.0001 is %d\n",tcontrol->TimeScale,(tcontrol->TimeScale>=(long double)0.0001));
//for some reason tcontrol->TimeScale>=0.0001 does not work when tcontrol->TimeScale==0.0001
//fprintf(stderr,"%Lg>1.0 is %d\n",tcontrol->TimeScale*10000.0,((tcontrol->TimeScale*10000.0)>=1.0));
//if (tcontrol->TimeScale>=(long double)0.0001)	{

if ((timeunit*1000.0)>=1.0)	{
	if (strlen(text)>0) {
		sprintf(text,"%s.%03i%s",text,msec,units);
	} else {
		sprintf(text,"%ims",msec);
	}
} else {

	//scale is 1-999 microseconds
//	if (tcontrol->TimeScale>=0.0000001)	{
	if ((timeunit*1000000.0)>=1.0)	{
		if (strlen(text)>0) {
			sprintf(text,"%s.%03i%03i%s",text,msec,usec,units);
		} else {
			if (msec>0) {
				sprintf(text,"%i.%03ims",msec,usec);
				//sprintf(text,"%i%03ius",msec,usec);
			} else {
				sprintf(text,"%ius",usec);
			} //msec>0
		}
	} else {
	
		//scale is 1-999 nanoseconds
//		if (tcontrol->TimeScale>=0.0000000001)	{
		if ((timeunit*10000000000.0)>=1.0)	{
			if (strlen(text)>0) { //there is at least 00:00:00
				sprintf(text,"%s.%03i%03i%03i%s",text,msec,usec,nsec,units);
			} else {
				if (msec>0) {
					sprintf(text,"%i.%03i%03ims",msec,usec,nsec);
				} else { //msec>0
					if (usec>0) {
						sprintf(text,"%i.%03ius",usec,nsec);
					} else {
						sprintf(text,"%ins",nsec);
					} //usec>0
				} //msec>0
			} //strlen(text)>0
		} //>=0.000000001
	} //microsecond >0.000001

} //0.001


//later make PStatus bit that makes txtMarkIn/Out hh:mm:ss.mls
#if 0 
if (hrs>23)
  {
  sprintf(text,"%i days ",hrs/24);
//  sprintf(text,"%i days %i:%02i:%02i",hrs/24,hrs-hrs/24,mns,sec);
  }
else
  {

	if (hrs>0 || mns>0) //||sec>0)
		{
		sprintf(text,"%i:%02i:%02i ",hrs,mns,sec);
		}
	else
		{

		if (sec>0)
		  {
		  //if (tcontrol->TimeScale*VStatus.TimeSpace<1.0)
//			if (tcontrol->TimeScale*PStatus.ScrollPixelsX<1.0)
			if (tcontrol->TimeScale*tcontrol->TimeSpace<1.0)
		    sprintf(text,"%i.%03is",sec,msec);
		  else
		    sprintf(text,"%is",sec);
		  }
		else
		  {
		  if (msec>0)
		     {
//		    if (tcontrol->TimeScale*VStatus.TimeSpace<0.001)
//		    if (tcontrol->TimeScale*PStatus.ScrollPixelsX<0.001)
		    if (tcontrol->TimeScale*tcontrol->TimeSpace<0.001)
		      sprintf(text,"%i.%03ims",msec,usec);
		    else
		      sprintf(text,"%ims",msec);
		    }
		  else
		    {
		    if (usec>0)
		      {
//		      if (tcontrol->TimeScale*VStatus.TimeSpace<0.000001)
//		      if (tcontrol->TimeScale*PStatus.ScrollPixelsX<0.000001)
		      if (tcontrol->TimeScale*tcontrol->TimeSpace<0.000001)
		        sprintf(text,"%i.%03ius",usec,nsec);
					else
		        sprintf(text,"%ius",usec);
		      }
		    else
		      {
		      sprintf(text,"%ins",nsec);
		      }  //usec
		    }  //msec
		  }  //sec
		} //hr:mn:sec
	} //days
#endif
}  //format 0

} //FT_FormatTime

