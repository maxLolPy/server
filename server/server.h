#pragma once
#define BACKLOG  20
#define MAXLENMESS 100

enum command {
	sendMessage,
	getCount
};

struct res {
	int words;
	int lines;
};
