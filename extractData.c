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


void skipHeader(FILE * fp)
{
	if( fseek(fp, SKIP_HEADER, SEEK_SET) == -1 )   //从文件指针开头移动165个字符
	{									   //加上换行符 164 ＋ 1 ＝ 165
		fprintf(stderr, "skip header failed!\n");
		exit(EXIT_FAILURE);
	}
	return;
}

void skipLine(FILE * fp)
{
//	recordNumbers++;

	char c = fgetc(fp);     //之所以不用fseek跳跃，为了判断文件结尾EOF，
							//如果没有读取操作可能会直接跳过最后的EOF
	return;
}

void trim( char * str )      //注意去掉前后空格即可
{
	char tmp[30];
	strcpy(tmp, str);
	char *p = tmp;

	while( *p != '\0' )
	{
		if( *p == ' ' )
		{
			p++;
			continue;
		}

		*str++ = *p++;
	}
	*str = '\0';

	return;
}
#if 1
int compare(char *date1, char *date2)   // 返回 大于 1 ； 小于 －1 ；相等 0；
{
	char tmp[10] = "0";
	int flag = 0;
	char *c;

	if( strlen(date1) > strlen(date2) )    //为了保证两个时间格式一致
	{
		strcat(tmp, date2);
		flag = 1;           //   date1 长度长些
	}
	else if( strlen(date1) < strlen(date2) )
	{
		strcat(tmp, date1);
		flag = 2;
	}

	char *p;
	char *q;

	if( flag == 1 )   // tmp 表示date2
	{
		p = date1;
		q = tmp;
	}

	else if( flag == 2)     // tmp 表示date1
	{
		p = tmp;
		q = date2;
	}

	else              //  两者长度相等
	{
		p = date1;
		q = date2;
	}

	while( *p != '\0' && *q != '\0' )
	{
#if 1
		if( *p < '0' || *p > '9' )
		{
			p++;
			q++;
			continue;
		}
#endif
		c = p;
		c++;

		if( *p > *q )
		{
			return 1;
		}
		else if( *p < *q )
		{
			return -1;
		}

		p++;
		q++;
	}
	return 0;
}

#endif

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

int getHoliday( int Year, int Month, int Day )    // 根据日期，返回节日权重
{
	int holiday = 1;

	return holiday;
}

int main(int argc, char **argv)
{
#if 1
	FILE *fp, *fp_data;
	long callVolume = 0;
	long handlingTime = 0;
	long callHalfHour = 0;
	char message[160];
	char date[20];
	int Month = 0;
	int Day = 0;
	char tmp_string[3];
	int count = 0;
	char delims = '|';
	char *result = NULL;
	int index = 0;		   //当前解析的是第几个字段，6 代表Call_date，
	int flag = 0;		   //9 代表IRV_TIME, 16 代表servc_time
//	int AGENT = 0;         //  (在处理handlingTime时) 为了防止与flag状态冲突
	char current_time[10];
	int k = 0;
	int check = 0;    // 判断在该时间区间内的计数是否有效（ IRV_TIME < 0 情况无效！）
	struct DataTotal dataTotal;

#if 0
	if( argc != 2 )
	{
		fprintf(stderr, "date not found!\n");
		exit(EXIT_FAILURE);
	}
#endif
	if( (fp = fopen("data.txt", "r")) == NULL )
	{
		fprintf(stderr, "file data.txt opens failed !\n");
		exit(EXIT_FAILURE);
	}

	if( (fp_data = fopen("MoreSimple.db", "w+")) == NULL )
	{
		fprintf(stderr, "file test.dat opens failed! \n");
		exit(EXIT_FAILURE);
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

			memset( date, 0, sizeof(char)*20 );
			memset( current_time, 0, sizeof(char)*10 );

			strcpy( date, "1999" );
			strcpy( current_time, "00:00:00" );

			if( Month < 10 )
			{
				strcat( date, "-0" );      // 拼接日期
			}
			else
				strcat( date, "-" );

			memset( tmp_string, 0, sizeof(char)*3 );
			sprintf( tmp_string, "%d", Month );
			strcat( date,  tmp_string );   // 拼接月份

			if( Day < 10 )
			{
				strcat( date, "-0" );
			}
			else
				strcat( date, "-" );

			memset( tmp_string, 0, sizeof(char)*3 );
			sprintf( tmp_string, "%d", Day );
			strcat( date, tmp_string );     // 拼接每天
#endif
			callVolume = 0;
			handlingTime = 0;
			
			dataTotal.weekday = getWeek(1999,Month,Day);
			dataTotal.holiday = getHoliday(1999,Month,Day);

			strcpy(current_time, "00:00:00");
			for( k = 0; k < 48; k++ )
			{
				char tmp[10];     //  存放当前时间

				memset(tmp, 0, sizeof(char)*10 );
				memset(dataTotal.halfHour[k].time, 0, sizeof(char)*10);
				strcpy(dataTotal.halfHour[k].time, current_time);
				strcpy(tmp, current_time);
				addHalfHour( current_time );
				dataTotal.halfHour[k].index = k;

				if( fseek(fp, 0, SEEK_SET) == -1 )   //从文件指针指向开头
				{
					fprintf(stderr, "skip header failed!\n");
					exit(EXIT_FAILURE);
				}
				skipHeader(fp);

				callHalfHour = 0;

			#if 1

				while( feof(fp) == 0 )
				{
					fread(message, 152, 1, fp);

					result = strtok( message, &delims );
					while( result != NULL )
					{
						trim(result);          			//去掉多余的空格
						index++;
			#endif
						if( ( index == 6 ) && ( strcmp(result, date) == 0 ) )
//						if( ( index == 6 ) && ( strcmp(result, "1999-01-03") == 0 ) )
						{
							callVolume++;
							flag = 1;          // 该标志记作，匹配正确的查找日期
						}

						if( ( flag == 1 ) && ( index == 7 ) )
						{
							if( ( compare(result, tmp) >= 0 ) && ( compare(result, current_time) < 0 ) )
							{
								check = 1;       // 满足查找日期，且在指定的半小时以内
								callHalfHour++;
							}
						}
						
						if( ( flag == 1 ) && ( index == 9 ) )
						{
							if( atoi(result) < 0 )
							{
								callVolume--;
								flag = 2;       // 该标志记作 IRV_TIME < 0 状态

								if( check == 1 )
									callHalfHour--;
							}
						}

						if( ( flag == 1 ) && ( index == 13 ) && ( strcmp(result, "AGENT") != 0 ) )
						{
							flag = 3;        //该标志记作，该通话的outcome 不是 AGENT
						}
	#if 1
						if( ( flag == 1 ) && ( index == 16 ) )   //  servc_time字段的值
						{
							handlingTime += atoi(result);
						}
	#endif
						result = strtok( NULL, &delims);
					}

					index = 0;
					flag = 0;
					check = 0;

					skipLine(fp);
				}
			
			dataTotal.halfHour[k].volume = callHalfHour;
//			fprintf(stdout, "%s  %s callHalfHour = %ld\n",date, tmp, callHalfHour);
			}
#if 1
			dataTotal.callVolume = callVolume / 48;
			dataTotal.handlingTime = handlingTime / 48;
//			fprintf(stdout, "%s callVolume = %ld\n", date, dataTotal.callVolume );
			fprintf(stdout, "%s handlingTime = %ld\n", date, dataTotal.handlingTime );
			fprintf(stdout, "----------------%s-------------------\n", date);

			if( fwrite(&dataTotal, sizeof(struct DataTotal), 1, fp_data) <= 0 )
			{
				fprintf(stderr, "Write simple.db Error!\n");
				exit(EXIT_FAILURE);
			}

			fflush(fp_data);
		}
	}
#endif

	fclose(fp);
	fclose(fp_data);
#endif

	return 0;
}
