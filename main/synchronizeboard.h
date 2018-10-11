#pragma once

class SynchronizeBoard
{
	static void initialize_sntp(void);

public:
	static void obtain_time(void);
	static void print_time(void);
};
