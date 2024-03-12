#pragma once
#pragma warning(disable:4996)// This line is written to prevent Visual Studio from forcing us to use its own localtime implementation.

#include <string>
#include <iostream>

class DateTime
{



public:
	struct TimeSpan
	{
		long long seconds=0;
		
		long long getTotalDays()
		{
			if (seconds == 0)return 0;
			return seconds / 86400;
		}
		long long getTotalHours()
		{
			if (seconds == 0)return 0;
			return seconds / 3600;
		}

		long long getTotalMinutes()
		{
			if (seconds == 0)return 0;
			return seconds / 60;
		}

		long long getTotalSeconds()
		{
			return seconds;
		}

		std::string getPrintableString()
		{
			int day = getTotalDays();
			int hour = getTotalHours() % 24;
			int minute = getTotalMinutes() % 60;
			int second = seconds % 60;

			std::string output = "Timespan : ";
			output += std::to_string(day) + " days, ";
			output += std::to_string(hour) + " hours, ";
			output += std::to_string(minute) + " minutes, ";
			output += std::to_string(second) + " seconds.";

			return output;
		}
	};

	struct TimePoint
	{
	protected:
		void addOneSecond() { second++; if (second > 59) { second -= 60; addOneMinute(); } }
		void addOneMinute() { minute++; if (minute > 59) { minute -= 60; addOneHour(); } }
		void addOneHour() { hour++; if (hour > 23) { hour -= 24; addOneDay(); } }
		void addOneDay() { 	day++; 	if (day > DateTime::days_of_month(month, year)) 	{ 	day = 1; addOneMonth(); } }
		void addOneMonth() { 	month++; if (month > 12) 	{ 	month = 1; year++; 	} }

		void subOneSecond() { second--; if (second <= 0) 	{ second += 60; subOneMinute(); } 	}
		void subOneMinute() 	{ 	minute--; if (minute <= 0) { minute += 60; 	subOneHour(); } }
		void subOneHour() { 	hour--; 		if (hour <= 0) { hour += 24; subOneDay(); } }
		void subOneDay() { day--; if (day <= 0) { subOneMonth(); day = DateTime::days_of_month(month, year); } }
		void subOneMonth() { month--; if (month <= 0) 	{ 	month = 12; year--; } }

	public:
		int year;
		int month;
		int day;
		int hour;
		int minute;
		int second;

		int day_of_year()//0-365
		{
			static int t[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304,334 };
			int remainder = 0;
			if (month > 2 && is_leap(year))remainder = 1;

			return t[month-1]+remainder+day-1;
		}
		int week_of_year()//starts from 0
		{
			int doy = day_of_year();
			int day_of_the_week_of_the_first_day = ((day_of_week() - (doy % 7)) + 7) % 7;
			std::cout << "Date : " << day << "/" << month << "/" << year << " day of the week of the first day : " << day_of_the_week_of_the_first_day << std::endl;
			return (doy + day_of_the_week_of_the_first_day) / 7;
		}

		int day_of_week()// Monday is 0 - Sunday is 6
		{
			static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
			int y = year;
			if (month < 3)
			{
				y -= 1;
			}
			return (((y + y / 4 - y / 100 + y / 400 + t[month - 1] + day)+6) % 7);
		}

		TimePoint(std::tm* t)
		{
			year = t->tm_year + 1900;
			month = t->tm_mon + 1;
			day = t->tm_mday;
			hour = t->tm_hour;
			minute = t->tm_min;
			second = t->tm_sec;
						
		}

		TimePoint(TimePoint* tp)
		{
			year = tp->year;
			month = tp->month;
			day = tp->day;
			hour = tp->hour;
			minute = tp->minute;
			second = tp->second;

		}

		TimePoint(int Year, int Month, int Day, int Hour=0, int Minute=0, int Second=0)
		{
			year = Year;
			month = Month;
			day = Day;
			hour = Hour;
			minute = Minute;
			second = Second;
		}

		TimeSpan operator-(TimePoint& other)
		{
			TimeSpan ts;

			TimePoint* first;
			TimePoint* second;
			int multiplier = 1;
			if (getNowLong() < other.getNowLong())
			{
				first = this;
				second = &other;
				multiplier = -1;
			}
			else
			{
				first = &other;
				second = this;
			}

			ts.seconds += second->second - first->second;
			ts.seconds += ((second->minute - first->minute) * 60);
			ts.seconds += ((second->hour - first->hour) * 3600);
			ts.seconds += ((second->day - first->day) * 86400);
			int total_days = 0;
			while (first->year < second->year || first->month < second->month)
			{
				if (first->month != second->month)
				{
					total_days += days_of_month(first->month, first->year);
					first->addOneMonth();
				}
				else
				{
					total_days += 365;
					if (first->month<=2 && is_leap(first->year))total_days++;
					else if (first->month>2 && is_leap((first->year+1)))total_days++;
					first->year++;

				}

			}
			ts.seconds += total_days * 86400;

			return ts;
		}
		bool isSameWeek(TimePoint& tp)
		{
			long long start_week = startOfWeek().getNowLong();
			long long end_week = endOfWeek().getNowLong();
			long long tp_time = tp.getNowLong();

			return ((tp_time >= start_week) && (tp_time <= end_week));
		}
		TimePoint SetTime(int hour, int minute, int second)
		{
			TimePoint tp(this);
			tp.hour = hour;
			tp.minute = minute;
			tp.second = second;
			return tp;

		}
		TimePoint startOfWeek()
		{
			return addDays(-day_of_week()).SetTime(0,0,0);
		}
		
		TimePoint endOfWeek()
		{
			return addDays(6-day_of_week()).SetTime(23, 59, 59);
		}

		long long getNowLong()
		{
			long long val = year * 10000000000;
			val += month * 100000000;
			val += day * 1000000;
			val += hour * 10000;
			val += minute * 100;
			val += second;
			
			return val;
		}

		std::string GetPrintableString()
		{
			std::string s = "";
			if (hour < 10)s = "0"; s += std::to_string(hour) + ":";
			if (minute < 10)s += "0"; s += std::to_string(minute) + ":";
			if (second < 10)s += "0"; s += std::to_string(second) + " ";
			if (day < 10)s += "0"; s += std::to_string(day) + "/";
			if (month < 10)s += "0"; s += std::to_string(month) + "/" + std::to_string(year);
			
			return s;
		}

		TimePoint addDays(long long i)
		{
			return addSeconds(i * 86400);
		}
		TimePoint addHours(long long i)
		{
			return addSeconds(i * 3600);
		}
		TimePoint addMinutes(long long i)
		{
			return addSeconds(i * 60);
		}
		TimePoint addSeconds(long long i)
		{
			TimePoint tp(this);
			if (i == 0)return tp;
			if (i >= 86400 || i<=-86400)//1 day in seconds
			{
				int days = i / 86400;
				i %= 86400;
				if(days>0)
				while (days > 0)
				{
					int days_of_month = DateTime::days_of_month(tp.month, tp.year);

					if(tp.day+days>days_of_month)
					{
						days -= 1+days_of_month - tp.day;
						tp.day = 1;
						tp.addOneMonth();
					}
					else
					{
						tp.day += days;
						days = 0;
					}
				}
				else if(days<0)
				while (days < 0)
				{
					if (days + tp.day <= 0)
					{
						days += tp.day;
						tp.subOneMonth();

						int days_of_month = DateTime::days_of_month(tp.month,tp.year);
						tp.day = days_of_month;
					}
					else
					{
						tp.day += days;
						days = 0;
					}
				}
			}
			if (i >= 3600 || i <= -3600)//1 hour in seconds
			{
				int hours = i / 3600;
				i %= 3600;
				
				tp.hour += hours;

				if (tp.hour > 23)
				{
					tp.hour -= 24;
					tp.addOneDay();
				}
				else if (tp.hour < 0)
				{
					tp.hour += 24;
					tp.subOneDay();
				}
			}
			if (i >= 60 ||i <= -60)//1 minute in seconds
			{
				int minutes = i / 60;
				i %= 60;

				tp.minute += minutes;

				if (tp.minute > 59)
				{
					tp.minute -= 60;
					tp.addOneHour();
				}
				else if (tp.minute < 0)
				{
					tp.minute += 60;
					tp.subOneHour();
				}
				if (i != 0)
				{
					tp.second += i;
					i = 0;
					if (tp.second > 59)
					{
						tp.second -= 60;
						tp.addOneMinute();
					}
					else if (tp.second < 0)
					{
						tp.second += 60;
						tp.subOneMinute();
					}
				}



			}
			

			return tp;
		}
		
		std::string getNowString()
		{
			std::string output = std::to_string(year);
			if (month < 10)output += "0"; output += std::to_string(month);
			if (day < 10)output += "0"; output += std::to_string(day);
			if (hour < 10)output += "0"; output += std::to_string(hour);
			if (minute < 10)output += "0"; output += std::to_string(minute);
			if (second < 10)output += "0"; output += std::to_string(second);

			return output;
		}
	};

	static const int month_days_arr[12];

	static bool is_leap(int year){return (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0);}

	static int days_of_month(int month, int year)
	{
		static int month_days_arr[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
		if (month == 2 && is_leap(year))return 29;
		if (month < 1 && month>12)return 0;

		return month_days_arr[month - 1];
	}

	static TimePoint now()
	{
		auto now = std::chrono::system_clock::now();

		std::time_t time = std::chrono::system_clock::to_time_t(now);
		std::tm* tm = std::localtime(&time);
		TimePoint tp(tm);

		return tp;
	}


	static std::string getNowString()
	{
		return now().getNowString();
	}


	static long long getNowLong()
	{
		return now().getNowLong();
	}

};