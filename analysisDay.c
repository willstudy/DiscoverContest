#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int SKIP_HEADER = 165;
const int MONTH = 12;
const int DAY = 31;

enum {
	SUN = 0,
	MON,
	TUE,
	WED,
	THU,
	FRI,
	SAT
};

enum {
	CHRI = 1,    //  圣诞节
	SPRI = 1    // 春节
};

struct HalfHour
{
	char time[10];    // 每半个小时的时间
	long volume;       // 通话容量
	int index;
};

struct DataTotal
{
	int weekday;     // 星期几
	int holiday;     //哪个节日
	struct HalfHour halfHour[48];
	long callVolume;
	long handlingTime;
};

void addHalfHour( char *current_time )
{
	char *p = current_time;
	char *q = current_time;
	int index = 1;

	while( *p != '\0' )
	{
		index++;
		p++;

		if( index == 4 && *p == '0' )
		{
			*p = '3';
			return;
		}

		else if( index == 4 && *p == '3' )
		{
			*p = '0';
			index = 1;

			while( *q != '\0' )
			{
				index++;
				q++;

				if( index == 2 )
				{
					if( *q < '9' )
					{
						*q += 1;
						return;
					}
					else
					{
						*q = '0';
						*current_time += 1;
						return;
					}
				}

			}
		}
	}
}


int getWeek( int Year, int Month, int Day )    // 根据日期，返回星期几
{
	int result = 0;        	//  利用吉姆拉尔森计算公式

	if( Month == 1 || Month == 2 )
	{
		Year--;
		Month += 12;
	}

	result = (Day+1+2*Month+3*(Month+1)/5+Year+Year/4-Year/100+Year/400)%7;

	return result;
}

float getHoliday( int Year, int Month, int Day )
{
	float holiday = 1.0;

	return holiday;
}

int main(int argc, char **argv)
{
#if 1
	FILE *fp_data;
	char current_time[10];     //  保存半个刻钟的时间
	int weekday = 0;           //  保存当前星期几
	int Month = 0;
	int Day = 0;
	int weekDay[7];            // 记录有多少个周一，周二等
	int day[31];               // 一个月的第几号有几个， 比如1号有11天
	long weekCallVolume[7];    // 记录每周一、周二等共有多少通话量
	long weekHandlingTime[7];  // 记录每周一、周二等共处理的时间

	long halfHourCallVolume[48][7];       // 每天每个时段的通话量
	long callVolumeDay[31];              //  哪一天天总共的通话量，比如说1号的总共通话量
	long handlingTimeDay[31];            //  每天总共的处理时间

	long totalCallVolume = 0;  // 总的通话量
	long totalHandlingTime = 0; //总的处理时间
	long totalCallVolumeHalfHour[48];

	int allDays = 0;            //总的天数
	int k = 0;
	int i = 0;

	float weightCallVolumeWeekDay[7];       // CallVolume 对每个对应的工作日 －－权重
	float weightHandlingTimeWeekDay[7];     // HandlingTime 对每个对应的工作日 －－权重
	float weightCallVolumeHalfHour[48][7];
	float weightCallVolumeDay[31];
	float weightHandlingTimeDay[31];

	float averageCallVolume = 0.0;
	float averageHandlingTime = 0.0;
	float averageCallVolumeHalfHour[48];

	struct DataTotal dataTotal[MONTH][DAY];     // 为了减少字段，直接用数组下标代替日期
	// 这里没有初始化，考虑赋值的时候，再初始化，没用到的，不去读取！

	if( (fp_data = fopen("MoreSimple.db", "r+")) == NULL )
	{
		fprintf(stderr, "file test.dat opens failed! \n");
		exit(EXIT_FAILURE);
	}

	for( k = 0; k < 7; k++ )     	//	  初始化各个数组
	{
		weekDay[k] = 0;
		weekCallVolume[k] = 0;
		weekHandlingTime[k] = 0;
		weightCallVolumeWeekDay[k] = 0.0;
		weightHandlingTimeWeekDay[k] = 0.0;
	}

	for( k = 0; k < 31; k++ )
	{
		day[k] = 0;
		callVolumeDay[k] = 0;
		handlingTimeDay[k] = 0;
		weightCallVolumeDay[k] = 0.0;
		weightHandlingTimeDay[k] = 0.0;
	}

	for( k = 0; k < 48; k++ )
	{
		for( i = 0; i < 7; i++ )
		{
			halfHourCallVolume[k][i] = 0;
			weightCallVolumeHalfHour[k][i] = 0.0;
		}

		averageCallVolumeHalfHour[k] = 0.0;
		totalCallVolumeHalfHour[k] = 0;
	}


#if 1
	for( Month = 1; Month <= 11; Month++ )
	{
		for( Day = 1; Day <= 31; Day++ )
		{
			if( Month == 2 && Day == 29 )   	//  1999年2月份，只有28天
				break;
			if( ( Month == 4 || Month == 6 || Month == 9 || Month == 11 ) && Day == 31 ) // 这几个月只有30天
				break;

			if( fread(&dataTotal[Month-1][Day-1], sizeof(struct DataTotal), 1, fp_data) <=0 )
			{
				fprintf(stderr, "fread error!\n");
				exit(EXIT_FAILURE);
			}
			fflush(fp_data);
		}
	}

	for( Month = 1; Month <= 11; Month++ )
	{
		for( Day = 1; Day <= 31; Day++ )
		{
			if( ( Month == 1 && Day == 11 ) || ( Month == 1 && Day == 12 ))
				continue;

			if( Month == 2 && Day == 29 )   	//  1999年2月份，只有28天
				break;
			if( ( Month == 4 || Month == 6 || Month == 9 || Month == 11 ) && Day == 31 ) // 这几个月只有30天
				break;

			if( Month == 5 && Day == 21 )
				continue;

			if( Month == 9 && Day == 11 )
				continue;

			for( k = 0; k < 48; k++ )
			{
				totalCallVolumeHalfHour[k] += dataTotal[Month-1][Day-1].halfHour[k].volume;
			}

			day[Day-1]++;  // 总共有多少个 1号 2号等等
			callVolumeDay[Day-1] += dataTotal[Month-1][Day-1].callVolume;
			handlingTimeDay[Day-1] += dataTotal[Month-1][Day-1].handlingTime;

#if 1
			switch( dataTotal[Month-1][Day-1].weekday )
			{
				case 0:
				{

					for( k = 0; k < 48; k++ )
					{
						halfHourCallVolume[k][0] += dataTotal[Month-1][Day-1].halfHour[k].volume;
					}

					weekDay[0]++;
					weekCallVolume[0] += dataTotal[Month-1][Day-1].callVolume;
					weekHandlingTime[0] += dataTotal[Month-1][Day-1].handlingTime;
					break;
				}
				case 1:
				{
					for( k = 0; k < 48; k++ )
					{
						halfHourCallVolume[k][1] += dataTotal[Month-1][Day-1].halfHour[k].volume;
					}

					weekDay[1]++;
					weekCallVolume[1] += dataTotal[Month-1][Day-1].callVolume;
					weekHandlingTime[1] += dataTotal[Month-1][Day-1].handlingTime;
					break;
				}
				case 2:
				{
					for( k = 0; k < 48; k++ )
					{
						halfHourCallVolume[k][2] += dataTotal[Month-1][Day-1].halfHour[k].volume;
					}

					weekDay[2]++;
					weekCallVolume[2] += dataTotal[Month-1][Day-1].callVolume;
					weekHandlingTime[2] += dataTotal[Month-1][Day-1].handlingTime;
					break;
				}
				case 3:
				{
					for( k = 0; k < 48; k++ )
					{
						halfHourCallVolume[k][3] += dataTotal[Month-1][Day-1].halfHour[k].volume;
					}

					weekDay[3]++;
					weekCallVolume[3] += dataTotal[Month-1][Day-1].callVolume;
					weekHandlingTime[3] += dataTotal[Month-1][Day-1].handlingTime;
					break;
				}
				case 4:
				{
					for( k = 0; k < 48; k++ )
					{
						halfHourCallVolume[k][4] += dataTotal[Month-1][Day-1].halfHour[k].volume;
					}

					weekDay[4]++;
					weekCallVolume[4] += dataTotal[Month-1][Day-1].callVolume;
					weekHandlingTime[4] += dataTotal[Month-1][Day-1].handlingTime;
					break;
				}
				case 5:
				{
					for( k = 0; k < 48; k++ )
					{
						halfHourCallVolume[k][5] += dataTotal[Month-1][Day-1].halfHour[k].volume;
					}

					weekDay[5]++;
					weekCallVolume[5] += dataTotal[Month-1][Day-1].callVolume;
					weekHandlingTime[5] += dataTotal[Month-1][Day-1].handlingTime;
					break;
				}
				case 6:
				{
					for( k = 0; k < 48; k++ )
					{
						halfHourCallVolume[k][6] += dataTotal[Month-1][Day-1].halfHour[k].volume;
					}

					weekDay[6]++;
					weekCallVolume[6] += dataTotal[Month-1][Day-1].callVolume;
					weekHandlingTime[6] += dataTotal[Month-1][Day-1].handlingTime;
					break;
				}

				default:
					break;
			}

			allDays++;

			totalCallVolume += dataTotal[Month-1][Day-1].callVolume;
			totalHandlingTime += dataTotal[Month-1][Day-1].handlingTime;
#endif
		}
	}
#endif

#if 1

	averageCallVolume = (totalCallVolume / (float)(allDays));
	averageHandlingTime = (totalHandlingTime / (float)allDays);

	for( k = 0; k < 7; k++ )
	{
		weightCallVolumeWeekDay[k] = ( weekCallVolume[k] / (float)weekDay[k] ) / averageCallVolume;
		weightHandlingTimeWeekDay[k] = ( weekHandlingTime[k] / (float)weekDay[k] ) / averageHandlingTime;
	}


	for( k = 0; k < 31; k++ )
	{
		weightCallVolumeDay[k] = ( callVolumeDay[k] / (float)day[k] ) / averageCallVolume;
		weightHandlingTimeDay[k] = ( handlingTimeDay[k] / (float)day[k] ) / averageHandlingTime;
	}
#if 0
	for( k = 0; k < 31; k++ )
	{
		printf("%dth day:weightCall = %ld, weightHand = %ld\n",k+1, callVolumeDay[k], handlingTimeDay[k] );
		printf("%dth day:weightCall = %.4f, weightHand = %.4f\n",k+1, weightCallVolumeDay[k], weightHandlingTimeDay[k] );
	}
#endif
#if 1
	for( k = 0; k < 48; k++ )
	{
		averageCallVolumeHalfHour[k] = totalCallVolumeHalfHour[k] / (float)(allDays);
	}
#if 1
	for( k = 0; k < 48; k++ )
	{
		for( i = 0; i < 7; i++ )
		{
			weightCallVolumeHalfHour[k][i] =( ( halfHourCallVolume[k][i] / (float)weekDay[i] ) ) / averageCallVolumeHalfHour[k];
		}
	}
#endif
#if 1
	for( Month = 12; Month <= 12; Month++ )   //  预测12月份的数据
	{
		for( Day = 1; Day <= 31; Day++ )
		{
			strcpy( current_time , "00:00:00" );
			weekday = getWeek(1999, Month, Day);
			dataTotal[Month-1][Day-1].weekday = weekday;
			dataTotal[Month-1][Day-1].holiday = getHoliday(1999, Month, Day);
#if 1
			for( k = 0; k < 48; k++ )
			{
				dataTotal[Month-1][Day-1].halfHour[k].index = k;
				strcpy( dataTotal[Month-1][Day-1].halfHour[k].time, current_time );
				addHalfHour( current_time );

				dataTotal[Month-1][Day-1].halfHour[k].volume = averageCallVolumeHalfHour[k] * weightCallVolumeHalfHour[k][weekday] * weightCallVolumeDay[Day-1] * getHoliday(1999, Month, Day);
			}
#endif
			dataTotal[Month-1][Day-1].callVolume = averageCallVolume * weightCallVolumeWeekDay[weekday] * weightCallVolumeDay[Day-1] * getHoliday(1999, Month, Day);
			dataTotal[Month-1][Day-1].handlingTime = averageHandlingTime * weightHandlingTimeWeekDay[weekday] * weightHandlingTimeDay[Day-1] * getHoliday(1999, Month, Day);
		}
	}

	for( Month = 12; Month <= 12; Month++ )
	{
		for( Day = 1; Day <= 31; Day++ )
		{
			for( k = 0; k < 48; k++ )
			{
				printf("%d %d %s %ld\n", Month, Day, dataTotal[Month-1][Day-1].halfHour[k].time, dataTotal[Month-1][Day-1].halfHour[k].volume);
			}

			printf("---%d %d %ld %ld\n",Month, Day, dataTotal[Month-1][Day-1].callVolume, dataTotal[Month-1][Day-1].handlingTime);

		}
	}
#endif

#endif
	fprintf(stdout, "TotalDay = %d, totalCallVolume = %ld, totalHandlingTime = %ld\n", allDays, totalCallVolume, totalHandlingTime);

	printf("averageCallVolume = %f,averageHandlingTime = %f\n", averageCallVolume, averageHandlingTime);
#endif
#endif
	fclose(fp_data);

	return 0;
}
