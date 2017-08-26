#ifndef _TIME_STRUCTS_H
#define _TIME_STRUCTS_H

#define DAYS_PER_4YEARS               (365*4 + 1)
#define SECONDS_PER_HOUR              (60*60)
#define SECONDS_PER_DAY               (24*SECONDS_PER_HOUR)
#define SECONDS_PER_WEEK              (7*SECONDS_PER_DAY)
#define SECONDS_PER_4YEARS            (SECONDS_PER_DAY*DAYS_PER_4YEARS)

typedef enum 
{
	Month_January = 0,
	Month_February,
	Month_March,
	Month_April,
	Month_May,
	Month_June,
	Month_July,
	Month_August,
	Month_September,
	Month_October,
	Month_November,
	Month_December,
} Month_t;

typedef enum 
{
	DayOfWeek_Sunday = 0,
	DayOfWeek_Monday,
	DayOfWeek_Tuesday,
	DayOfWeek_Wednesday,
	DayOfWeek_Thursday,
	DayOfWeek_Friday,
	DayOfWeek_Saturday,
} DayOfWeek_t;

typedef enum
{
	TimeUnit_Milliseconds,
	TimeUnit_Seconds,
	TimeUnit_Minutes,
	TimeUnit_Hours,
	TimeUnit_Days,
	TimeUnit_Months,
	TimeUnit_Years,
} TimeUnit_t;

const char* GetMonthString(Month_t month)
{
	switch (month)
	{
		case Month_January:   return "January";
		case Month_February:  return "February";
		case Month_March:     return "March";
		case Month_April:     return "April";
		case Month_May:       return "May";
		case Month_June:      return "June";
		case Month_July:      return "July";
		case Month_August:    return "August";
		case Month_September: return "September";
		case Month_October:   return "October";
		case Month_November:  return "November";
		case Month_December:  return "December";
		default:              return "Unknown";
	};
}
const char* GetMonthStr(Month_t month)
{
	switch (month)
	{
		case Month_January:   return "Jan";
		case Month_February:  return "Feb";
		case Month_March:     return "Mar";
		case Month_April:     return "Apr";
		case Month_May:       return "May";
		case Month_June:      return "Jun";
		case Month_July:      return "Jul";
		case Month_August:    return "Aug";
		case Month_September: return "Sep";
		case Month_October:   return "Oct";
		case Month_November:  return "Nov";
		case Month_December:  return "Dec";
		default:              return "Unk";
	};
}

const char* GetDayOfWeekString(DayOfWeek_t dayOfWeek)
{
	switch (dayOfWeek)
	{
		case DayOfWeek_Sunday:    return "Sunday";
		case DayOfWeek_Monday:    return "Monday";
		case DayOfWeek_Tuesday:   return "Tuesday";
		case DayOfWeek_Wednesday: return "Wednesday";
		case DayOfWeek_Thursday:  return "Thursday";
		case DayOfWeek_Friday:    return "Friday";
		case DayOfWeek_Saturday:  return "Saturday";
		default:                  return "Unknown";
	};
}
const char* GetDayOfWeekStr(DayOfWeek_t dayOfWeek)
{
	switch (dayOfWeek)
	{
		case DayOfWeek_Sunday:    return "Sun";
		case DayOfWeek_Monday:    return "Mon";
		case DayOfWeek_Tuesday:   return "Tue";
		case DayOfWeek_Wednesday: return "Wed";
		case DayOfWeek_Thursday:  return "Thu";
		case DayOfWeek_Friday:    return "Fri";
		case DayOfWeek_Saturday:  return "Sat";
		default:                  return "Unk";
	};
}

const char* GetDayOfMonthString(u16 dayOfMonth)
{
	switch (dayOfMonth)
	{
		case 0:  return "1st";
		case 1:  return "2nd";
		case 2:  return "3rd";
		case 3:  return "4th";
		case 4:  return "5th";
		case 5:  return "6th";
		case 6:  return "7th";
		case 7:  return "8th";
		case 8:  return "9th";
		case 9:  return "10th";
		case 10: return "11th";
		case 11: return "12th";
		case 12: return "13th";
		case 13: return "14th";
		case 14: return "15th";
		case 15: return "16th";
		case 16: return "17th";
		case 17: return "18th";
		case 18: return "19th";
		case 19: return "20th";
		case 20: return "21st";
		case 21: return "22nd";
		case 22: return "23rd";
		case 23: return "24th";
		case 24: return "25th";
		case 25: return "26th";
		case 26: return "27th";
		case 27: return "28th";
		case 28: return "29th";
		case 29: return "30th";
		case 30: return "31st";
		default: return "Unk";
	};
}

inline u16 Convert24HourTo12Hour(u16 twentyFourHourValue)
{
	switch (twentyFourHourValue)
	{
		case 0:  return 12;
		case 1:  return 1;
		case 2:  return 2;
		case 3:  return 3;
		case 4:  return 4;
		case 5:  return 5;
		case 6:  return 6;
		case 7:  return 7;
		case 8:  return 8;
		case 9:  return 9;
		case 10: return 10;
		case 11: return 11;
		case 12: return 12;
		case 13: return 1;
		case 14: return 2;
		case 15: return 3;
		case 16: return 4;
		case 17: return 5;
		case 18: return 6;
		case 19: return 7;
		case 20: return 8;
		case 21: return 9;
		case 22: return 10;
		case 23: return 11;
		default: return 0;
	};
}

inline bool IsPostMeridian(u16 twentyFourHourValue)
{
	if (twentyFourHourValue >= 0 && twentyFourHourValue <= 11)
		return false;
	else
		return true;
}

const char* GetTimeUnitString(TimeUnit_t timeUnit)
{
	switch (timeUnit)
	{
		case TimeUnit_Milliseconds: return "milliseconds";
		case TimeUnit_Seconds:      return "seconds";
		case TimeUnit_Minutes:      return "minutes";
		case TimeUnit_Hours:        return "hours";
		case TimeUnit_Days:         return "days";
		case TimeUnit_Months:       return "months";
		case TimeUnit_Years:        return "years";
		default:                    return "unknown";
	};
}
const char* GetTimeUnitStr(TimeUnit_t timeUnit)
{
	switch (timeUnit)
	{
		case TimeUnit_Milliseconds: return "ms";
		case TimeUnit_Seconds:      return "sec";
		case TimeUnit_Minutes:      return "min";
		case TimeUnit_Hours:        return "hrs";
		case TimeUnit_Days:         return "dys";
		case TimeUnit_Months:       return "mts";
		case TimeUnit_Years:        return "yrs";
		default:                    return "unk";
	};
}

struct RealTime_t
{
	u16 year;
	u16 month;
	u16 day;
	u16 hour;
	u16 minute;
	u16 second;
	u16 millisecond;
};

u64 GetTimestamp(const RealTime_t& realTime)
{
	if (realTime.year < 1970)
		return 0;
	
	u64 result = 0;
	u64 temp = realTime.year - 1970;
	
	while (temp >= 4)
	{
		result += SECONDS_PER_4YEARS;
		temp -= 4;
	}
	
	bool isLeapYear = (temp == 2);
	result += temp * 365 * SECONDS_PER_DAY;
	if (temp >= 2) result += 1 * SECONDS_PER_DAY;
	
	
	for (temp = 0; temp < realTime.month; temp++)
	{
		Assert(temp < 12);
		
		uint8_t numDaysInMonth = (temp%2 == 0) ? 31 : 30;
		if (temp == Month_February)
		{
			numDaysInMonth = (isLeapYear ? 29 : 28);
		}
		
		result += numDaysInMonth * SECONDS_PER_DAY;
	}
	
	result += realTime.day * SECONDS_PER_DAY;
	result += realTime.hour * SECONDS_PER_HOUR;
	result += realTime.minute * 60;
	result += realTime.second;
	
	return result;
}

DayOfWeek_t GetDayOfWeek(u64 timestamp)
{
	// Jan 1st 1970 was a Thursday so we have to offset the
	// timestamp by 4 days before taking the modulo
	u8 dayOfWeek = (u8)(((timestamp + SECONDS_PER_DAY*DayOfWeek_Thursday) % SECONDS_PER_WEEK) / SECONDS_PER_DAY);
	
	Assert(dayOfWeek < 7);
	return (DayOfWeek_t)dayOfWeek;
}

DayOfWeek_t GetDayOfWeek(const RealTime_t& realTime)
{
	return GetDayOfWeek(GetTimestamp(realTime));
}

RealTime_t RealTimeAt(u64 timestamp)
{
	RealTime_t result = {};
	
	u16 numFourYears = (u16)(timestamp / SECONDS_PER_4YEARS);
	result.year = 1970 + 4*numFourYears;
	timestamp -= numFourYears * SECONDS_PER_4YEARS;
	
	u16 tempDays = (u16)(timestamp / SECONDS_PER_DAY);
	timestamp -= tempDays * SECONDS_PER_DAY;
	
	//since the first leap year was 1972 then every leap year lands on the 3rd
	//of every 4 year chunk
	bool isLeapYear = false;
	if (tempDays < 365)
	{
		//We're in the 1st of 4 years.
		result.year += 0;
		tempDays -= 0;
	}
	else if (tempDays < 365+365)
	{
		//We're in the 2nd of 4 years.
		result.year += 1;
		tempDays -= 365;
	}
	else if (tempDays < 365+365+366)
	{
		//We're in the 3rd of 4 years. (Leap Year)
		result.year += 2;
		tempDays -= 365+365;
		isLeapYear = true;
	}
	else if (tempDays < 365+365+366+365)
	{
		//We're in the 4th of 4 years.
		result.year += 3;
		tempDays -= 365+365+366;
	}
	else
	{
		//Shouldn't hit year 5 since we already subtracted 
		//4 year chunks above
		Assert(false);
	}
	
	for (result.month = Month_January; true; result.month++)
	{
		Assert(result.month <= Month_December);
		
		uint8_t numDaysInMonth = (result.month%2 == 0) ? 31 : 30;
		if (result.month == Month_February)
		{
			numDaysInMonth = (isLeapYear ? 29 : 28);
		}
		
		if (tempDays < numDaysInMonth)
		{
			break;
		}
		else
		{
			tempDays -= numDaysInMonth;
		}
	}
	
	result.day = tempDays;
	
	result.hour = (u16)(timestamp / SECONDS_PER_HOUR);
	timestamp -= result.hour * SECONDS_PER_HOUR;
	
	result.minute = (u16)(timestamp / 60);
	timestamp -= result.minute * 60;
	
	result.second = (u16)timestamp;
	
	//NOTE: Our uint16 for year will overflow before the uint64 timestamp...
	// Assert(result.year <= 584942419325);
	Assert(result.month < 12);
	Assert(result.day < 31);
	Assert(result.hour < 24);
	Assert(result.minute < 60);
	Assert(result.second < 60);
	
	return result;
}

i64 SubtractTimes(const RealTime_t& left, const RealTime_t& right, TimeUnit_t returnUnit)
{
	Assert(returnUnit != TimeUnit_Milliseconds);
	RealTime_t result = {};
	
	i64 leftTimestamp  = (i64)GetTimestamp(left);
	i64 rightTimestamp = (i64)GetTimestamp(right);
	i64 secDiff = leftTimestamp - rightTimestamp;
	
	switch (returnUnit)
	{
		case TimeUnit_Milliseconds:
		{
			Assert(false);
			return 0;
		} break;
		
		case TimeUnit_Seconds:
		{
			return secDiff;
		} break;
		
		case TimeUnit_Minutes:
		{
			return secDiff / 60;
		} break;
		
		case TimeUnit_Hours:
		{
			return secDiff / SECONDS_PER_HOUR;
		} break;
		
		case TimeUnit_Days:
		{
			return secDiff / SECONDS_PER_DAY;
		} break;
		
		case TimeUnit_Months:
		{
			Assert(false);
			return 0;
		} break;
		
		case TimeUnit_Years:
		{
			Assert(false);
			return 0;
		} break;
		
		default:
		{
			Assert(false);
			return 0;
		} break;
	};
}

#endif // _TIME_STRUCTS_H