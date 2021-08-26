#pragma once
#include "Windows.h"
#include <string>
#include "Logger.h"
#include <vector>
#include "Sender.h"
#include "malloc.h"
#include "wtsapi32.h"
#include "cchannel.h"

#define MAX_NUM_SERVER_WRITE_TRIES		10000
#define MAX_CONSECUTIVE_FAILS			20

#define READ_MAX_SIZE					0x100000u		// large enough to always read only once
#define MAX_NUM_WRITE_TRIES				10

#define STARTUP_SLEEP					60000

#define FIRST_MSG						"\x02\x04\x13\x00\x00\x00\x14\x00\x00\x00\x00\x06\x0A\x00\x04\x00\x00\x00\x00\x00\x00\x00"
#define FIRST_MSG_SIZE					22


#define CRASH_MSG						"\x00\x04\x0b\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x01\x00\x5a\x00\x0c\x00\x00\x00\x0c\x00\x00\x00\x01\x00\x00\xff\x0c\x00\x00\x00\x0c\x00\x00\x00\x01\x00\x01\x00\x0c\x00\x00\x00\x0c\x00\x00\x00\x00\x00\x5f\x5f\x0c\x00\x00\x00\x0c\x00\x00\x00\x01\x00\x00\xff\x0c\x00\x00\x00\x0c\x00\x00\x00\x00\x00\x5f\x5f\x0c\x00\x00\x00\x0c\x00\x00\x00\x00\xee\xff\x00\x0c\x00\x00\x00\x0c\x00\x00\x00\x00\xee\xff\x00\x0c\x00\x00\x00\x0c\x00\x00\x00\x01\x00\x00\xff\x0c\x00\x00\x00\x0c\x00\x00\x00\x01\x00\x00\xff\x0c\x00\x00\x00\x0c\x00\x00\x00\x01\x00\x00\xff\x0c\x00\x00\x00\x0c\x00\x00\x00\x01\x00\x00\xff\x0c\x00\x00\x00\x0c\x00\x00\x00\x00\x00\x5f\x5f\x0e\x00\x00\x00\x54\x01\x00\x00\x80\x07\x00\x00\x38\x04\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\x00\x01\xff\xff\xff\xff\x8b\x00\x01\x20\x5f\x5f\x4e\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\x4e\x4e\x4e\x4e\x62\x00\x22\x00\x00\x01\x00\x00\x00\x5f\x5f\x6f\x63\x6d\x64\xff\xff\x5f\x5f\x5f\x63\x5f\x5f\x00\xf3\x00\x00\x63\x6d\x64\x30\x61\x00\x00\x01\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x54\x01\x00\x00\x80\x07\x00\x00\x38\x04\x00\x00\x0d\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7f\x07\x00\x00\x37\x04\x00\x00\x01\x00\x00\x00\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xee\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\x64\xe5\x00\x00\x00\x00\x00\x7f\x07\x00\x00\x37\x04\x00\x00\x01\x00\x00\x00\x00\x4e\x4e\x62\x00\x22\x00\x00\x01\x00\x00\x00\x5f\x5f\x6f\x63\x6d\x64\xff\xff\x5f\x5f\x5f\x63\x5f\x5f\x00\xf3\x00\x00\x63\x6d\x64\x30\x61\x00\x00\x01\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x54\x01\x00\x00\x80\x07\x00\x00\x38\x04\x00\x00\x0d\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7f\x07\x00\x00\x37\x04\x00\x00\x01\x00\x00\x00\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\x64\xfe\x00\x00\x00\x00\x00\x40\x07\x00\x00\x37\x04\x00\x00\x01\x00\x38\x04\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7f\x13\x00\x00\x37\x04\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x7f\x5f\x63\x6d\x64\x30\x32\x79\x00\x09\x00\x00\xff\xff\x00\x20\x8f\x00\x00\x00\xc1\xcc\x0c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x54\x01\x00\x00\x6b\x07\x00\x00\x38\x04\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\x00\x01\xff\xff\xff\xff\x8b\x00\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\x4e\x4e\x4e\x4e\x62\x00\x22\x00\x00\x01\x00\x00\x00\x5f\x5f\x6f\x63\x6d\x64\xff\xff\x5f\x5f\x5f\x63\x5f\x5f\x00\xf3\x00\x00\x63\x6d\x64\x30\x61\x00\x00\x01\xf5\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x54\x01\x00\x00\x80\x07\x00\x00\x38\x04\x00\x00\x0d\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7f\x07\x00\x00\x37\x04\x00\x00\x01\x00\x00\x00\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\x64\xfe\x00\x00\x00\x00\x00\x7f\x07\x00\x00\x37\x04\x00\x00\x01\x00\x00\x00\x00\x4e\x4e\x62\x00\x22\x00\x00\x01\x00\x00\x00\x5f\x5f\x6f\x63\x6d\x64\xff\xff\x5f\x5f\x5f\x63\x5f\x5f\x00\xf3\x00\x00\x63\x6d\x64\x30\x61\x00\x00\x01\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x54\x01\x00\x00\x80\x26\x00\x00\x38\x04\x00\x00\x0d\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7f\x07\x00\x00\x37\x04\x00\x00\x01\x00\x00\x00\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\x02\xff\xfe\xfe\xfe\xfe\xfe\xfe\x64\xfe\x00\x00\x00\x00\x00\x7f\x07\x00\x00\x37\x04\x00\x00\x01\x00\x00\x00\x00\x00\x20\x00\x80\xff\xff\xff\x21\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00\x23\x00\x00\x00\x00\xea\xfe\x00\x20\x00\x80\xff\xff\xff\x21\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00\x23\x00\x00\x00\x00\xea\xfe\xff\x00\x00\x00\x00\x00\x00\xfd\xfe\x00\x00\x00\x00\x09\xff\xff\x00\x00\x00\x63\x6d\x64\x30\x61\x00\x80\x01\xff\x5f\x5f\x63\x6d\x64\x30\x00\x00\x00\x00\x00\x20\x00\x80\xff\xff\xff\x21\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00\x23\x00\x00\x00\x00\xea\xfe\x00\x20\x00\x80\xff\xff\xff\x21\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00\x23\x00\x00\x00\x00\xea\xfe\xff\x00\x00\x00\x00\x00\x00\xfd\xfe\x00\x00\x00\x00\x09\xff\xff\x00\x00\x00\x63\x00\xc1\xcc\x0c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x54\x01\x00\x00\x80\x07\x00\x00\x38\x04\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\x00\x01\xff\xff\xff\xff\x8b\x00\x01\x20\x5f\x5f\x4e\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\x4f\xf0\xf0\xf0\xf0\x4e\x4e\x4e\x4e\x62\x00\x22\x00\x00\x01\x00\x00\x00\x5f\x5f\x6f\x63\x6d\x64\xff\xff\x5f\x5f\x5f\x63\x5f\x5f\x00\xf3\x00\x00\x63\x6d\x64\x30\x61\x00\x00\x01\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x54\x01\x00\x00\x80\x07\x00\x00\x38\x04\x00\x00\x0d\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7f\x07\x00\x00\x37\x04\x00\x00\x01\x00\x00\x00\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\x64\xfe\x00\x00\x00\x00\x00\x7f\x07\x00\x00\x37\x04\x00\x00\x01\x00\x00\x00\x00\x4e\x4e\x62\x00\x22\x00\x00\x01\x00\x00\x00\x5f\x5f\x6f\x63\x6d\x64\xff\xff\x5f\x5f\x5f\x63\x5f\x5f\x00\xf3\x00\x00\x63\x6d\x64\x30\x61\x00\x00\x01\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x54\x01\x00\x00\xff\xff\xff\xff\x38\x04\x00\x00\x0d\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7f\x07\x00\x00\x37\x04\x00\x00\x01\x00\x00\x00\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\x00\x4e\x4e\x62\x00\x22\x00\x00\x01\x00\x00\x00\x5f\x5f\x6f\x63\x6d\x64\xff\xff\x5f\x5f\x5f\x63\x5f\x5f\x00\xf3\x00\x00\x63\x6d\x64\x30\x61\x00\x00\x01\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x54\x01\x00\x00\x80\x07\x00\x00\x38\x04\x00\x00\x0d\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7f\x07\x00\x00\x37\x04\x00\x00\x01\x00\x00\x00\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\x64\xfe\x00\x00\x00\x00\x00\x40\x07\x00\x00\x37\x04\x00\x00\x01\x00\x38\x04\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7f\x13\x00\x00\x37\x04\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x7f\x5f\x63\x6d\x64\x30\x32\x79\x00\x09\x00\x00\xff\xff\x00\x20\x8f\x00\x00\x00\xc1\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x54\x01\x00\x00\x6b\x07\x00\x00\x38\x04\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\x00\x01\xff\xff\xff\xff\x8b\x00\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\x4e\x4e\x4e\x4e\x62\x00\x22\x00\x00\x01\x00\x00\x00\x5f\x5f\x6f\x63\x6d\x64\xff\xff\x5f\x5f\x5f\x63\x5f\x5f\x00\xf3\x00\x00\x63\x6d\x64\x30\x61\x00\x00\x01\xf5\x5f\x5f\x5f\x63\x6d\xe5\x30\x65\x80\x07\x00\x00\x38\x04\x00\x00\x0d\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xfd\xfe\x00\x00\x00\x00\x09\xff\xff\x00\x00\x00\x63\x6d\x64\x30\x61\x00\x80\x01\xff\x5f\x5f\x63\x6d\x64\x30\x61\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define CRASH_MSG_SIZE					3902

#define BAD_MSG							"\x02\x04\x01\x00\x00\x00\x1a\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xFF\xFF\xFF\x00"
#define BAD_MSG_SIZE					28


#define BAD_MSG2						"\x02\x04\x01\x00\x00\x00\x1a\x00\x00\x00\x00\x00\xFF\xFF\x00\x00\x00\x00\x00\xFF\xFF\xFF\xFF\x01\x00\x00\x00\x00"
#define BAD_MSG2_SIZE					28



//#define TEST_MSG_17						"\x00\x04\x09\x00\x00\x00\x0f\x00\x00\x00\x00\x00\x00\x01\x5f\x5f\x21\x17\x00\x00\x00\x1c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x08\x00\x00\x0a\x00\x00\x00\x0a\x00\x00\x00\x00\x00"
//#define TEST_MSG_17_SIZE				55

#define TEST_MSG						"\x00\x04\x09\x00\x00\x00\x0F\x00\x00\x00\x00\x00\x00\x01\x00\x01\x20\x01\x00\x00\x00\x0c\x01\x00\x00\x20\x88\x88\x88\x88\xFF\xFF\xFF\xFF\x20\x20\x20\x20\x07\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\xa0\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x00\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x00\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x00\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x3a\x20\x20\x20\x20\x20\x20\xff\x7f\x20\x20\x20\x20\x20\x20\x21\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x26\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x2d\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\xff\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x32\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
#define TEST_MSG_SIZE					285

#define FAULTY_MSG						"\x00\x04\x09\x00\x00\x00\x0f\x00\x00\x00\x00\x00\x00\x01\x00\x01\x20\x02\x00\x00\x00\xa4\x00\x00\x00\x00\x00\x09\x00\x00\x00\x00\x00\x20\x8f\x00\x00\x00\xc1\xcc\x0c\x00\x00\x00\x06\x00\x00\x00\x01\x00\xc4\xcc\x7d\x00\x00\x00\x40\x01\x00\x01\x00\x01\x01\x00\x5e\x00\x00\x00\x42\x00\x80\x02\x3e\x00\x40\x00\x66\x76\x88\x99\xa9\xc6\xcc\x5e\x00\x00\x00\x00\x00\x00\x01\x00\x0a\x00\x00\xff\x3d\x00\x05\x00\x05\x00\x00\x00\x00\x00\x16\x38\x40\x04\x00\x80\x20\x08\x04\x04\x08\x10\x42\x23\x6d\x80\x00\x17\x4e\x80\xe8\x0b\x02\xc1\x61\x61\x62\xca\xd5\x55\x00\x0a\x9f\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x8b\x00\x07\x10\x2d\x08\x0a\x80\xf2\x44\x00\x00\x00\x08\x09\x00\x00\x00\x08\x09\x00\xc2\xcc\x06\x00\x00\x00\x03\x00\x00\x00\x0e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0a\x00\x00\x00\x0a\x00\x00\x00\x00\x00"
#define FAULTY_MSG_SIZE					205

#define BAD_E_MSG						"\xE0\x04\x0e\x00\x00\x00\x54\x01\x00\x00\xFE\x7F\x00\x00\xFE\x7F\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7f\x07\x00\x00\x37\x04\x00\x00\x01\x6d\x64\x30\x65\x80\x07\x00\x00\x38\x04\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7f\x07\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define BAD_E_MSG_SIZE					342

#define START_FRAME_MSG					"\x02\x04\x0B\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x17\x00\x00"
#define START_FRAME_SIZE				18

#define END_FRAME_MSG					"\x02\x04\x0C\x00\x00\x00\x0C\x00\x00\x00\x00\x17\x00\x00"
#define END_FRAME_SIZE					14


#define AUDIO_FORMATS_MSG				"\x07\x8C\x00\x03\x6F\x02\x00\x00\x90\xD8\xDE\xEB\x6F\x02\x00\x00\x27\x0C\x1A\x00\xFF\x08\x00\xB2\x06\xA1\x02\x00\x44\xAC\x00\x00\xC0\x5D\x00\x00\x04\x00\x10\x00\x00\x00\x06\xA1\x02\x00\x44\xAC\x00\x00\x20\x4E\x00\x00\x04\x00\x10\x00\x00\x00\x06\xA1\x02\x00\x44\xAC\x00\x00\x80\x3E\x00\x00\x04\x00\x10\x00\x00\x00\x06\xA1\x02\x00\x44\xAC\x00\x00\xE0\x2E\x00\x00\x04\x00\x10\x00\x00\x00\x01\x00\x02\x00\x44\xAC\x00\x00\x10\xB1\x02\x00\x04\x00\x10\x00\x00\x00\x02\x00\x02\x00\x44\xAC\x00\x00\x47\xAD\x00\x00\x00\x08\x04\x00\x20\x00\xF4\x07\x07\x00\x00\x01\x00\x00\x00\x02\x00\xFF\x00\x00\x00\x00\xC0\x00\x40\x00\xF0\x00\x00\x00\xCC\x01\x30\xFF\x88\x01\x18\xFF\x11\x00\x02\x00\x44\xAC\x00\x00\xDB\xAC\x00\x00\x00\x08\x04\x00\x02\x00\xF9\x07\x06\x00\x02\x00\x22\x56\x00\x00\x44\xAC\x00\x00\x02\x00\x08\x00\x00\x00\x02\x00\x02\x00\x22\x56\x00\x00\x27\x57\x00\x00\x00\x04\x04\x00\x20\x00\xF4\x03\x07\x00\x00\x01\x00\x00\x00\x02\x00\xFF\x00\x00\x00\x00\xC0\x00\x40\x00\xF0\x00\x00\x00\xCC\x01\x30\xFF\x88\x01\x18\xFF\x11\x00\x02\x00\x22\x56\x00\x00\xB9\x56\x00\x00\x00\x04\x04\x00\x02\x00\xF9\x03\x02\x00\x01\x00\x44\xAC\x00\x00\xA3\x56\x00\x00\x00\x04\x04\x00\x20\x00\xF4\x07\x07\x00\x00\x01\x00\x00\x00\x02\x00\xFF\x00\x00\x00\x00\xC0\x00\x40\x00\xF0\x00\x00\x00\xCC\x01\x30\xFF\x88\x01\x18\xFF\x11\x00\x01\x00\x44\xAC\x00\x00\x6D\x56\x00\x00\x00\x04\x04\x00\x02\x00\xF9\x07\x02\x00\x02\x00\x11\x2B\x00\x00\x19\x2C\x00\x00\x00\x02\x04\x00\x20\x00\xF4\x01\x07\x00\x00\x01\x00\x00\x00\x02\x00\xFF\x00\x00\x00\x00\xC0\x00\x40\x00\xF0\x00\x00\x00\xCC\x01\x30\xFF\x88\x00\xFC\xFF\x11\x00\x02\x00\x11\x2B\x00\x00\xA9\x2B\x00\x00\x00\x02\x04\x00\x02\x00\xF9\x01\x02\x00\x01\x00\x22\x56\x00\x00\x93\x2B\x00\x00\x00\xF2\x03\x00\x20\x00\xF4\x03\x07\x00\x00\x01\x00\x00\x00\x02\x00\xFF\x00\x00\x00\x00\xC0\x00\x40\x00\xF0\x00\x00\x00\xCC\x01\x30\xFF\x88\x01\x18\xFF\x11\x00\x01\x00\x22\x56\x00\x00\x5C\x2B\x00\x00\x00\x02\x04\x00\x02\x00\xF9\x03\x31\x00\x01\x00\x44\xAC\x00\x00\xFD\x22\x00\x00\x41\x00\x00\x00\x02\x00\x40\x01\x02\x00\x02\x00\x40\x1F\x00\x00\x00\x20\x00\x00\x00\x02\x04\x00\x20\x00\xF4\x01\x07\x00\x00\x01\x00\x00\x00\x02\x00\xFF\x00\x00\x00\x00\xC0\x00\x40\x00\xF0\x00\x00\x00\xCC\x01\x30\xFF\x88\x01\x18\xFF\x11\x00\x02\x00\x40\x1F\x00\x00\xAE\x1F\x00\x00\x00\x02\x04\x00\x02\x00\xF9\x01\x02\x00\x01\x00\x11\x2B\x00\x00\x0C\x16\x00\x00\x00\x01\x04\x00\x20\x00\xF4\x01\x07\x00\x00\x01\x00\x00\x00\x02\x00\xFF\x00\x00\x00\x00\x00\x00\x02\x04\x00\x02\x00\xF9\x03\x31\x00\x01\x00\x44\xAC\x00\x00\xFD\x22\x00\x00\x41\x00\x00\x00\x02\x00\x40\x01\x02\x00\x00\x02\x00\xF9\x01\x11\x00\x01\x00\x22\x56\x00\x00\x7E\x11\x00\x00\x41\x00\x00\x00\x02\x00\x40\x01\x02\x00\x01\x00\x40\x1F\x00\x00\x00\x10\x00\x00\x00\x01\x04\x00\x20\x00\xF4\x01\x07\x00\x00\x01\x00\x00\x00\x02\x00\xFF\x00\x00\x00\x00\xC0\x00\x40\x00\xF0\x00\x00\x00\xCC\x01\x30\xFF\x88\x01\x18\xFF\x11\x00\x01\x00\x40\x1F\x00\x00\xD7\x0F\x00\x00\x00\x01\x04\x00\x02\x00\xF9\x01\x31\x00\x01\x00\x11\x2B\x00\x00\xBF\x08\x00\x00\x41\x00\x00\x00\x02\x00\x40\x01\x31\x00\x01\x00\x40\x1F\x00\x00\x59\x06\x00\x00\x41\x00\x00\x00\x02\x00\x40\x01"
#define AUDIO_FORMATS_MSG_SIZE			0x304

struct channel_data
{
	HANDLE handle;
	std::string channel_name;
	ULONG timeout;
	bool dynamic;
	PVOID read_buffer;
};


class CVirtualChannel : public CSender
{
public:
	CVirtualChannel(std::string _channel_name, bool _dynamic, ULONG _timeout = 0)
	{
		CHAR log_file_name[MAX_PATH];
		channel_data data;

		data.channel_name = _channel_name;
		data.timeout = _timeout;
		data.dynamic = _dynamic;

		data.read_buffer = NULL;

		data.handle = NULL;

		channels.push_back(data);

		CAPS_sent = false;

		count = 0;

		sprintf_s(log_file_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\GfxVirtualChannel-%ld.txt", GetCurrentProcessId());
		logger = new CLogger(log_file_name, false, false);

		InitializeCriticalSection(&send_critical_section);
	}


	~CVirtualChannel()
	{
		for (auto it = channels.begin(); it != channels.end(); it++)
		{
			if (it->read_buffer)
			{
				free(it->read_buffer);
			}
		}

		delete logger;
	}

	bool connect()
	{
		return connect(1);
	}

	bool connect(DWORD num_retries)
	{
		DWORD last_error;
		DWORD retry;

		for (retry = 0; retry < num_retries; retry++)
		{
			channels[0].handle = WTSVirtualChannelOpenEx(WTS_CURRENT_SESSION, (LPSTR)channels[0].channel_name.c_str(), channels[0].dynamic);
			if (channels[0].handle == NULL)
			{
				last_error = GetLastError();
				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] failed to connect! last_error = %08x\r\n", last_error);
				logger->log(log_buffer);

				Sleep(100);
			}
			else
			{
				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] connected!\r\n");
				logger->log(log_buffer);

				break;
			}
		}
		if (channels[0].handle == NULL)
		{
			last_error = GetLastError();
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] failed to connect in all %ld retries\r\n", num_retries);
			logger->log(log_buffer);

			return false;
		}

		return true;
	}

	void disconnect()
	{
		WTSVirtualChannelClose(channels[0].handle);
		channels[0].handle = NULL;
	}

	bool reconnect(bool sleep = true)
	{
		disconnect();

		if (sleep)
		{
			// give the client time to restart
			Sleep(STARTUP_SLEEP);
		}

		return connect(10);
	}

	bool read()
	{
		bool rc;
		DWORD bytes_read = 0;
		DWORD last_error;

		if (channels[0].read_buffer == NULL)
		{
			channels[0].read_buffer = malloc(READ_MAX_SIZE);
			if (channels[0].read_buffer == NULL)
			{
				return false;
			}
		}

		rc = WTSVirtualChannelRead(channels[0].handle, channels[0].timeout, (PCHAR)channels[0].read_buffer, READ_MAX_SIZE, &bytes_read);
		if (rc == 0)
		{
			last_error = GetLastError();
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::read] failed to read! last_error = %08x\r\n", last_error);
			logger->log(log_buffer);
		}
		else
		{
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::read] read %ld bytes\r\n", bytes_read);
			logger->log(log_buffer);

			logger->log((PUCHAR)channels[0].read_buffer, bytes_read);
		}

		return rc;
	}

	bool write(PVOID data, DWORD data_size)
	{
		return write(data, data_size, true);
	}

	bool write(PVOID data, DWORD data_size, bool reconnect_required = true)
	{
		DWORD bytes_written = 0;

		if (!channels[0].handle)
		{
			connect();
		}

		if (data)
		{
			count++;
			if (WTSVirtualChannelWrite(channels[0].handle, (PCHAR)data, data_size, &bytes_written) == 0)
			{
				return false;
			}
		}

		return true;
	}

private:

	std::vector<channel_data> channels;

	CLogger *logger;
	std::string log_message;
	CHAR log_buffer[1024];

	bool CAPS_sent;

	DWORD count;

	CRITICAL_SECTION send_critical_section;

	bool first = true;
};

class CGfxVirtualChannel : public CSender
{
public:
	CGfxVirtualChannel()
	{
		CHAR log_file_name[MAX_PATH];
		channel_data data;

		data.channel_name = "Microsoft::Windows::RDS::Graphics";
		data.timeout = 0;
		data.dynamic = true;
		data.read_buffer = NULL;
		data.handle = NULL;

		channels.push_back(data);

		CAPS_sent = false;

		count = 0;

		sprintf_s(log_file_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\GfxVirtualChannel-%ld.txt", GetCurrentProcessId());
		logger = new CLogger(log_file_name, false, false);

		InitializeCriticalSection(&send_critical_section);
	}

	~CGfxVirtualChannel()
	{
		for (auto it = channels.begin(); it != channels.end(); it++)
		{
			if (it->read_buffer)
			{
				free(it->read_buffer);
			}
		}

		delete logger;
	}

	bool connect()
	{
		return connect(1);
	}

	bool connect(DWORD num_retries)
	{
		DWORD last_error;
		DWORD retry;

		for (retry = 0; retry < num_retries; retry++)
		{
			channels[0].handle = WTSVirtualChannelOpenEx(WTS_CURRENT_SESSION, (LPSTR)channels[0].channel_name.c_str(), channels[0].dynamic);
			if (channels[0].handle == NULL)
			{
				last_error = GetLastError();
				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] failed to connect! last_error = %08x\r\n", last_error);
				logger->log(log_buffer);

				Sleep(100);
			}
			else
			{
				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] connected!\r\n");
				logger->log(log_buffer);

				break;
			}
		}
		if (channels[0].handle == NULL)
		{
			last_error = GetLastError();
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] failed to connect in all %ld retries\r\n", num_retries);
			logger->log(log_buffer);

			return false;
		}

		return true;
	}

	void disconnect()
	{
		WTSVirtualChannelClose(channels[0].handle);
		channels[0].handle = NULL;
	}

	bool reconnect(bool sleep = true)
	{
		disconnect();

		if (sleep)
		{
			// give the client time to restart
			Sleep(STARTUP_SLEEP);
		}

		return connect(10);
	}

	bool read()
	{
		bool rc;
		DWORD bytes_read = 0;
		DWORD last_error;

		if (channels[0].read_buffer == NULL)
		{
			channels[0].read_buffer = malloc(READ_MAX_SIZE);
			if (channels[0].read_buffer == NULL)
			{
				return false;
			}
		}

		rc = WTSVirtualChannelRead(channels[0].handle, channels[0].timeout, (PCHAR)channels[0].read_buffer, READ_MAX_SIZE, &bytes_read);
		if (rc == 0)
		{
			last_error = GetLastError();
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::read] failed to read! last_error = %08x\r\n", last_error);
			logger->log(log_buffer);
		}
		else
		{
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::read] read %ld bytes\r\n", bytes_read);
			logger->log(log_buffer);

			logger->log((PUCHAR)channels[0].read_buffer, bytes_read);
		}

		return rc;
	}

	bool write(PVOID data, DWORD data_size)
	{
		return write(data, data_size, true);
	}

	bool write(PVOID data, DWORD data_size, bool reconnect_required = true)
	{
		DWORD bytes_written = 0;

		if (!channels[0].handle)
		{
			connect();
		}

		if (data)
		{
			count++;
			if (WTSVirtualChannelWrite(channels[0].handle, (PCHAR)data, data_size, &bytes_written) == 0)
			{
				return false;
			}
		}

		return true;
	}

private:

	std::vector<channel_data> channels;

	CLogger *logger;
	std::string log_message;
	CHAR log_buffer[1024];

	bool CAPS_sent;

	DWORD count;

	CRITICAL_SECTION send_critical_section;

	bool first = true;
};

class CAudioVirtualChannel : public CSender
{
public:
	CAudioVirtualChannel()
	{
		CHAR log_file_name[MAX_PATH];
		channel_data data;

		data.channel_name = "AUDIO_PLAYBACK_DVC";
		data.timeout = 0;
		data.dynamic = true;
		data.read_buffer = NULL;
		data.handle = NULL;

		channels.push_back(data);

		count = 0;

		sprintf_s(log_file_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\AudioVirtualChannel-%ld.txt", GetCurrentProcessId());
		logger = new CLogger(log_file_name, false, false);

		InitializeCriticalSection(&send_critical_section);
	}

	~CAudioVirtualChannel()
	{
		if (channels[0].read_buffer)
		{
			free(channels[0].read_buffer);
		}

		delete logger;
	}

	void enumerate()
	{

	}

	bool connect()
	{
		return connect(1);
	}
	bool connect(DWORD num_retries)
	{
		DWORD last_error;
		DWORD retry;
		DWORD flags = 0;

		if (channels[0].dynamic)
		{
			flags = WTS_CHANNEL_OPTION_DYNAMIC | WTS_CHANNEL_OPTION_DYNAMIC_PRI_REAL;
		}

		for (retry = 0; retry < num_retries; retry++)
		{
			channels[0].handle = WTSVirtualChannelOpenEx(WTS_CURRENT_SESSION, (LPSTR)channels[0].channel_name.c_str(), flags);
			if (channels[0].handle == NULL)
			{
				last_error = GetLastError();
				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] failed to connect! last_error = %08x\r\n", last_error);
				logger->log(log_buffer);

				Sleep(100);
			}
			else
			{
				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] connected!\r\n");
				logger->log(log_buffer);

				break;
			}
		}
		if (channels[0].handle == NULL)
		{
			last_error = GetLastError();
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] failed to connect in all %ld retries\r\n", num_retries);
			logger->log(log_buffer);

			return false;
		}

		return true;
	}

	void disconnect()
	{
		WTSVirtualChannelClose(channels[0].handle);
		channels[0].handle = NULL;
	}

	bool reconnect(bool sleep = true)
	{
		disconnect();

		if (sleep)
		{
			// give the client time to restart
			Sleep(STARTUP_SLEEP);
		}

		return connect(10);
	}

	bool read()
	{
		bool rc;
		DWORD bytes_read = 0;
		DWORD last_error;

		if (channels[0].read_buffer == NULL)
		{
			channels[0].read_buffer = malloc(READ_MAX_SIZE);
			if (channels[0].read_buffer == NULL)
			{
				return false;
			}
		}

		rc = WTSVirtualChannelRead(channels[0].handle, channels[0].timeout, (PCHAR)channels[0].read_buffer, READ_MAX_SIZE, &bytes_read);
		if (rc == 0)
		{
			last_error = GetLastError();
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::read] failed to read! last_error = %08x\r\n", last_error);
			logger->log(log_buffer);
		}
		else
		{
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::read] read %ld bytes\r\n", bytes_read);
			logger->log(log_buffer);
		}

		return rc;
	}

	bool write(PVOID data, DWORD data_size)
	{
		return write(data, data_size, true, false);
	}

	bool write(PVOID data, DWORD data_size, bool reconnect_required = true, bool lock = false)
	{
		bool rc = false;
		DWORD bytes_written = 0;
		DWORD last_error;

		if (!channels[0].handle)
		{
			connect();
		}

		if (lock)
		{
			EnterCriticalSection(&send_critical_section);
		}

		if (data)
		{
			count++;

			if (WTSVirtualChannelWrite(channels[0].handle, (PCHAR)data, data_size, &bytes_written) == 0)				// first write failed - if this is a disconnection try to reconnect
			{
				last_error = GetLastError();

				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::write] write failed last_error=%ld\r\n", last_error);
				logger->log(log_buffer);

				if (last_error == ERROR_PIPE_NOT_CONNECTED)
				{
					if (reconnect())
					{
						if (WTSVirtualChannelWrite(channels[0].handle, (PCHAR)data, data_size, &bytes_written) == 0)	// second write failed - too bad
						{
							sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::write] write failed after reconnect last_error=%ld\r\n", GetLastError());
							logger->log(log_buffer);
						}
						else
						{
							rc = true;
						}
					}
				}
			}
			else
			{
				rc = true;
			}
		}

		if (lock)
		{
			LeaveCriticalSection(&send_critical_section);
		}

		return rc;
	}

	bool reset()
	{
		char msg[1000];
		bool rc;

		rc = reconnect(false);

		if (!rc)
		{
			rc = reconnect(true);
		}

		if (rc)
		{
			memcpy(msg, AUDIO_FORMATS_MSG, AUDIO_FORMATS_MSG_SIZE);
			write((PVOID)msg, (DWORD)AUDIO_FORMATS_MSG_SIZE, false, false);
		}

		return rc;
	}

private:

	std::vector<channel_data> channels;

	CLogger *logger;
	std::string log_message;
	CHAR log_buffer[1024];

	DWORD count;

	CRITICAL_SECTION send_critical_section;
};

class CGeometryVirtualChannel : public CSender
{
public:
	CGeometryVirtualChannel()
	{
		CHAR log_file_name[MAX_PATH];
		channel_data data;

		data.channel_name = "Microsoft::Windows::RDS::Geometry::v08.01";
		data.timeout = 0;
		data.dynamic = true;
		data.read_buffer = NULL;
		data.handle = NULL;

		channels.push_back(data);

		count = 0;

		sprintf_s(log_file_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\GeometryVirtualChannel-%ld.txt", GetCurrentProcessId());
		logger = new CLogger(log_file_name, false, false);

		InitializeCriticalSection(&send_critical_section);
	}

	~CGeometryVirtualChannel()
	{
		if (channels[0].read_buffer)
		{
			free(channels[0].read_buffer);
		}

		delete logger;
	}

	void enumerate()
	{

	}

	bool connect()
	{
		return connect(1);
	}
	bool connect(DWORD num_retries)
	{
		DWORD last_error;
		DWORD retry;
		DWORD flags = 0;

		if (channels[0].dynamic)
		{
			flags = WTS_CHANNEL_OPTION_DYNAMIC | WTS_CHANNEL_OPTION_DYNAMIC_PRI_HIGH;
		}

		for (retry = 0; retry < num_retries; retry++)
		{
			channels[0].handle = WTSVirtualChannelOpenEx(WTS_CURRENT_SESSION, (LPSTR)channels[0].channel_name.c_str(), flags);
			if (channels[0].handle == NULL)
			{
				last_error = GetLastError();
				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] failed to connect! last_error = %08x\r\n", last_error);
				logger->log(log_buffer);

				Sleep(100);
			}
			else
			{
				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] connected!\r\n");
				logger->log(log_buffer);

				break;
			}
		}
		if (channels[0].handle == NULL)
		{
			last_error = GetLastError();
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] failed to connect in all %ld retries\r\n", num_retries);
			logger->log(log_buffer);

			return false;
		}

		return true;
	}

	void disconnect()
	{
		WTSVirtualChannelClose(channels[0].handle);
		channels[0].handle = NULL;
	}

	bool reconnect(bool sleep = true)
	{
		disconnect();

		if (sleep)
		{
			// give the client time to start
			Sleep(STARTUP_SLEEP);
		}

		return connect(10);
	}

	bool read()
	{
		bool rc;
		DWORD bytes_read = 0;
		DWORD last_error;

		if (channels[0].read_buffer == NULL)
		{
			channels[0].read_buffer = malloc(READ_MAX_SIZE);
			if (channels[0].read_buffer == NULL)
			{
				return false;
			}
		}

		rc = WTSVirtualChannelRead(channels[0].handle, channels[0].timeout, (PCHAR)channels[0].read_buffer, READ_MAX_SIZE, &bytes_read);
		if (rc == 0)
		{
			last_error = GetLastError();
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::read] failed to read! last_error = %08x\r\n", last_error);
			logger->log(log_buffer);
		}
		else
		{
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::read] read %ld bytes\r\n", bytes_read);
			logger->log(log_buffer);
		}

		return rc;
	}

	bool write(PVOID data, DWORD data_size)
	{
		return write(data, data_size, true, false);
	}

	bool write(PVOID data, DWORD data_size, bool reconnect_required = true, bool lock = false)
	{
		bool rc = false;
		DWORD bytes_written = 0;
		DWORD last_error;

		if (!channels[0].handle)
		{
			connect();
		}

		if (lock)
		{
			EnterCriticalSection(&send_critical_section);
		}

		if (data)
		{
			count++;

			if (WTSVirtualChannelWrite(channels[0].handle, (PCHAR)data, data_size, &bytes_written) == 0)				// first write failed - if this is a disconnection try to reconnect
			{
				last_error = GetLastError();

				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::write] write failed last_error=%ld\r\n", last_error);
				logger->log(log_buffer);

				if (last_error == ERROR_PIPE_NOT_CONNECTED)
				{
					if (reconnect())
					{
						if (WTSVirtualChannelWrite(channels[0].handle, (PCHAR)data, data_size, &bytes_written) == 0)	// second write failed - too bad
						{
							sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::write] write failed after reconnect last_error=%ld\r\n", GetLastError());
							logger->log(log_buffer);
						}
						else
						{
							rc = true;
						}
					}
				}
			}
			else
			{
				rc = true;
			}
		}

		if (lock)
		{
			LeaveCriticalSection(&send_critical_section);
		}

		return rc;
	}

private:

	std::vector<channel_data> channels;

	CLogger *logger;
	std::string log_message;
	CHAR log_buffer[1024];

	DWORD count;

	CRITICAL_SECTION send_critical_section;
};

class CVideoVirtualChannel : public CSender
{
public:
	CVideoVirtualChannel()
	{
		CHAR log_file_name[MAX_PATH];
		channel_data control_channel_data;
		channel_data data_channel_data;

		control_channel_data.channel_name = "Microsoft::Windows::RDS::Video::Control::v08.01";
		control_channel_data.timeout = 0;
		control_channel_data.dynamic = true;
		control_channel_data.read_buffer = NULL;
		control_channel_data.handle = NULL;

		data_channel_data.channel_name = "Microsoft::Windows::RDS::Video::Data::v08.01";
		data_channel_data.timeout = 0;
		data_channel_data.dynamic = true;
		data_channel_data.read_buffer = NULL;
		data_channel_data.handle = NULL;

		channels.push_back(control_channel_data);
		channels.push_back(data_channel_data);

		count = 0;

		sprintf_s(log_file_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\VideoVirtualChannel-%ld.txt", GetCurrentProcessId());
		logger = new CLogger(log_file_name, false, false);

		InitializeCriticalSection(&send_critical_section);
	}

	~CVideoVirtualChannel()
	{
		for (auto it = channels.begin(); it != channels.end(); it++)
		{
			if (it->read_buffer)
			{
				free(it->read_buffer);
			}
		}

		delete logger;
	}

	void enumerate()
	{

	}

	bool connect()
	{
		return connect(1);
	}
	bool connect(DWORD num_retries)
	{
		DWORD last_error;
		DWORD retry;
		DWORD flags = 0;


		for (auto it = channels.begin(); it != channels.end(); it++)
		{
			if (it->dynamic)
			{
				flags = WTS_CHANNEL_OPTION_DYNAMIC | WTS_CHANNEL_OPTION_DYNAMIC_PRI_REAL;
			}

			for (retry = 0; retry < num_retries; retry++)
			{
				it->handle = WTSVirtualChannelOpenEx(WTS_CURRENT_SESSION, (LPSTR)it->channel_name.c_str(), flags);
				if (it->handle == NULL)
				{
					last_error = GetLastError();
					sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] failed to connect! last_error = %08x\r\n", last_error);
					logger->log(log_buffer);

					Sleep(100);
				}
				else
				{
					sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] connected!\r\n");
					logger->log(log_buffer);

					break;
				}
			}
			if (it->handle == NULL)
			{
				last_error = GetLastError();
				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] failed to connect in all %ld retries\r\n", num_retries);
				logger->log(log_buffer);

				return false;

				// 2do - fix handle leak here on error in the second channel
			}
		}

		return true;
	}

	void disconnect()
	{
		for (auto it = channels.begin(); it != channels.end(); it++)
		{
			WTSVirtualChannelClose(it->handle);
			it->handle = NULL;
		}
	}

	bool reconnect(bool sleep = true)
	{
		disconnect();

		if (sleep)
		{
			// give the client time to start
			Sleep(STARTUP_SLEEP);
		}

		return connect(10);
	}

	bool read()
	{
		// 2do - read is not really supported because we do not know which channel to read from. 
		// just read from the first for now

		bool rc;
		DWORD bytes_read = 0;
		DWORD last_error;

		if (channels[0].read_buffer == NULL)
		{
			channels[0].read_buffer = malloc(READ_MAX_SIZE);
			if (channels[0].read_buffer == NULL)
			{
				return false;
			}
		}

		rc = WTSVirtualChannelRead(channels[0].handle, channels[0].timeout, (PCHAR)channels[0].read_buffer, READ_MAX_SIZE, &bytes_read);
		if (rc == 0)
		{
			last_error = GetLastError();
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::read] failed to read! last_error = %08x\r\n", last_error);
			logger->log(log_buffer);
		}
		else
		{
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::read] read %ld bytes\r\n", bytes_read);
			logger->log(log_buffer);
		}

		return rc;
	}

	bool write(PVOID data, DWORD data_size)
	{
		return write(data, data_size, true, false);
	}

	bool write(PVOID data, DWORD data_size, bool reconnect_required = true, bool lock = false)
	{
		bool rc = false;
		DWORD bytes_written = 0;
		DWORD last_error;
		DWORD *header = (DWORD *)data;
		DWORD type;
		channel_data * channel;

		if (!channels[0].handle)
		{
			connect();
		}

		if (lock)
		{
			EnterCriticalSection(&send_critical_section);
		}

		if (data)
		{
			count++;

			type = header[1] & 0xf;

			if (type == 1)
			{
				channel = &channels[0];
			}
			else
			{
				channel = &channels[1];
			}

			if (WTSVirtualChannelWrite(channel->handle, (PCHAR)data, data_size, &bytes_written) == 0)				// first write failed - if this is a disconnection try to reconnect
			{
				last_error = GetLastError();

				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::write] write failed last_error=%ld\r\n", last_error);
				logger->log(log_buffer);

				if (last_error == ERROR_PIPE_NOT_CONNECTED)
				{
					if (reconnect())
					{
						if (WTSVirtualChannelWrite(channel->handle, (PCHAR)data, data_size, &bytes_written) == 0)	// second write failed - too bad
						{
							sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::write] write failed after reconnect last_error=%ld\r\n", GetLastError());
							logger->log(log_buffer);
						}
						else
						{
							rc = true;
						}
					}
				}
			}
			else
			{
				rc = true;
			}
		}

		if (lock)
		{
			LeaveCriticalSection(&send_critical_section);
		}

		return rc;
	}

	bool reset()
	{
		bool rc;

		rc = reconnect(false);

		if (!rc)
		{
			rc = reconnect(true);
		}

		return rc;
	}

private:

	std::vector<channel_data> channels;

	CLogger *logger;
	std::string log_message;
	CHAR log_buffer[1024];

	DWORD count;

	CRITICAL_SECTION send_critical_section;
};

class CdwmproxVirtualChannel : public CSender
{
public:
	CdwmproxVirtualChannel()
	{
		CHAR log_file_name[MAX_PATH];
		channel_data control_channel_data;
		channel_data data_channel_data;

		control_channel_data.channel_name = "DWMPROX";
		control_channel_data.timeout = 0;
		control_channel_data.dynamic = true;
		control_channel_data.read_buffer = NULL;
		control_channel_data.handle = NULL;

		channels.push_back(control_channel_data);

		count = 0;

		sprintf_s(log_file_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\VideoVirtualChannel-%ld.txt", GetCurrentProcessId());
		logger = new CLogger(log_file_name, false, false);

		InitializeCriticalSection(&send_critical_section);
	}

	~CdwmproxVirtualChannel()
	{
		for (auto it = channels.begin(); it != channels.end(); it++)
		{
			if (it->read_buffer)
			{
				free(it->read_buffer);
			}
		}

		delete logger;
	}

	void enumerate()
	{

	}

	bool connect()
	{
		return connect(1);
	}
	bool connect(DWORD num_retries)
	{
		DWORD last_error;
		DWORD retry;
		DWORD flags = 0;


		for (auto it = channels.begin(); it != channels.end(); it++)
		{
			if (it->dynamic)
			{
				flags = WTS_CHANNEL_OPTION_DYNAMIC | WTS_CHANNEL_OPTION_DYNAMIC_PRI_HIGH;
			}

			for (retry = 0; retry < num_retries; retry++)
			{
				it->handle = WTSVirtualChannelOpenEx(WTS_CURRENT_SESSION, (LPSTR)it->channel_name.c_str(), flags);
				if (it->handle == NULL)
				{
					last_error = GetLastError();
					sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] failed to connect! last_error = %08x\r\n", last_error);
					logger->log(log_buffer);

					Sleep(100);
				}
				else
				{
					sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] connected!\r\n");
					logger->log(log_buffer);

					break;
				}
			}
			if (it->handle == NULL)
			{
				last_error = GetLastError();
				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] failed to connect in all %ld retries\r\n", num_retries);
				logger->log(log_buffer);

				return false;

				// 2do - fix handle leak here on error in the second channel
			}
		}

		return true;
	}

	void disconnect()
	{
		for (auto it = channels.begin(); it != channels.end(); it++)
		{
			WTSVirtualChannelClose(it->handle);
			it->handle = NULL;
		}
	}

	bool reconnect(bool sleep = true)
	{
		disconnect();

		if (sleep)
		{
			// give the client time to restart
			Sleep(STARTUP_SLEEP);
		}

		return connect(10);
	}

	bool read()
	{
		// 2do - read is not really supported because we do not know which channel to read from. 
		// just read from the first for now

		bool rc;
		DWORD bytes_read = 0;
		DWORD last_error;

		if (channels[0].read_buffer == NULL)
		{
			channels[0].read_buffer = malloc(READ_MAX_SIZE);
			if (channels[0].read_buffer == NULL)
			{
				return false;
			}
		}

		rc = WTSVirtualChannelRead(channels[0].handle, channels[0].timeout, (PCHAR)channels[0].read_buffer, READ_MAX_SIZE, &bytes_read);
		if (rc == 0)
		{
			last_error = GetLastError();
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::read] failed to read! last_error = %08x\r\n", last_error);
			logger->log(log_buffer);
		}
		else
		{
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::read] read %ld bytes\r\n", bytes_read);
			logger->log(log_buffer);
		}

		return rc;
	}

	bool write(PVOID data, DWORD data_size)
	{
		return write(data, data_size, true, false);
	}

	bool write(PVOID data, DWORD data_size, bool reconnect_required = true, bool lock = false)
	{
		bool rc = false;
		DWORD bytes_written = 0;
		DWORD last_error;

		if (!channels[0].handle)
		{
			connect();
		}

		if (lock)
		{
			EnterCriticalSection(&send_critical_section);
		}

		if (data)
		{
			count++;

			if (WTSVirtualChannelWrite(channels[0].handle, (PCHAR)data, data_size, &bytes_written) == 0)				// first write failed - if this is a disconnection try to reconnect
			{
				last_error = GetLastError();

				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::write] write failed last_error=%ld\r\n", last_error);
				logger->log(log_buffer);

				if (last_error == ERROR_PIPE_NOT_CONNECTED)
				{
					if (reconnect())
					{
						if (WTSVirtualChannelWrite(channels[0].handle, (PCHAR)data, data_size, &bytes_written) == 0)	// second write failed - too bad
						{
							sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::write] write failed after reconnect last_error=%ld\r\n", GetLastError());
							logger->log(log_buffer);
						}
						else
						{
							rc = true;
						}
					}
				}
			}
			else
			{
				rc = true;
			}
		}

		if (lock)
		{
			LeaveCriticalSection(&send_critical_section);
		}

		return rc;
	}

	bool reset()
	{
		bool rc;

		rc = reconnect(false);

		if (!rc)
		{
			rc = reconnect(true);
		}

		return rc;
	}

private:

	std::vector<channel_data> channels;

	CLogger *logger;
	std::string log_message;
	CHAR log_buffer[1024];

	DWORD count;

	CRITICAL_SECTION send_critical_section;
};

class CRdpdrVirtualChannel : public CSender
{
public:
	CRdpdrVirtualChannel(std::string _channel_name, bool _dynamic, ULONG _timeout = 0)
	{
		CHAR log_file_name[MAX_PATH];

		channel_name = _channel_name;
		timeout = _timeout;
		dynamic = _dynamic;

		read_buffer = NULL;

		handle = NULL;

		count = 0;

		sprintf_s(log_file_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\RdpdrVirtualChannel-%ld.txt", GetCurrentProcessId());
		logger = new CLogger(log_file_name, false, false);

		InitializeCriticalSection(&send_critical_section);
	}


	~CRdpdrVirtualChannel()
	{
		if (read_buffer)
		{
			free(read_buffer);
		}

		delete logger;
	}

	void enumerate()
	{

	}

	bool connect()
	{
		return connect(1);
	}

	bool connect(DWORD num_retries)
	{
		DWORD last_error;
		DWORD retry;

		for (retry = 0; retry < num_retries; retry++)
		{
			handle = WTSVirtualChannelOpenEx(WTS_CURRENT_SESSION, (LPSTR)channel_name.c_str(), dynamic);
			if (handle == NULL)
			{
				last_error = GetLastError();
				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] failed to connect! last_error = %08x\r\n", last_error);
				logger->log(log_buffer);

				Sleep(100);
			}
			else
			{
				break;
			}
		}
		if (handle == NULL)
		{
			last_error = GetLastError();
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::connect] failed to connect in all %ld retries\r\n", num_retries);
			logger->log(log_buffer);

			return false;
		}
		return true;
	}

	void disconnect()
	{
		WTSVirtualChannelClose(handle);
		handle = NULL;
	}

	bool reconnect(bool sleep = true)
	{
		disconnect();

		if (sleep)
		{
			// give the client time to restart
			Sleep(STARTUP_SLEEP);
		}

		return connect(10);
	}

	bool read()
	{
		bool rc;
		DWORD bytes_read = 0;
		DWORD last_error;

		if (read_buffer == NULL)
		{
			read_buffer = malloc(READ_MAX_SIZE);
			if (read_buffer == NULL)
			{
				return false;
			}
		}

		rc = WTSVirtualChannelRead(handle, timeout, (PCHAR)read_buffer, READ_MAX_SIZE, &bytes_read);
		if (rc == 0)
		{
			last_error = GetLastError();
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::read] failed to read! last_error = %08x\r\n", last_error);
			logger->log(log_buffer);
		}
		else
		{
			sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::read] read %ld bytes\r\n", bytes_read);
			logger->log(log_buffer);
		}

		return rc;
	}

	bool write(PVOID data, DWORD data_size)
	{
		return write(data, data_size, true);
	}

	bool write(PVOID data, DWORD data_size, bool reconnect_required = true)
	{
		DWORD bytes_written = 0;

		if (!handle)
		{
			connect();
		}

		if (data)
		{
			count++;
			if (WTSVirtualChannelWrite(handle, (PCHAR)data, data_size, &bytes_written) == 0)
			{
				sprintf_s(log_buffer, sizeof(log_buffer), "[CVirtualChannel::write] WTSVirtualChannelWrite failed! last_error=%08x\r\n", GetLastError());
				logger->log(log_buffer);

				return false;
			}
		}
		return true;
	}

private:

	HANDLE handle;
	DWORD session_id;
	std::string channel_name;
	ULONG timeout;

	bool dynamic;

	PVOID read_buffer;

	CLogger *logger;
	std::string log_message;
	CHAR log_buffer[1024];

	DWORD count;

	CRITICAL_SECTION send_critical_section;
};

class CClientDynamicVirtualChannel : public CSender
{
public:
	CClientDynamicVirtualChannel(std::string _channel_name)
	{
		CHAR log_file_name[MAX_PATH];
		channel_data channel;

		channel.channel_name = _channel_name;
		channel.timeout = 0;
		channel.dynamic = true;
		channel.read_buffer = NULL;
		channel.handle = NULL;

		channels.push_back(channel);

		count = 0;

		sprintf_s(log_file_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\ClientDynamicVirtualChannel-%ld.txt", GetCurrentProcessId());
		logger = new CLogger(log_file_name, false, false);

		logger->log("starting\r\n");

		InitializeCriticalSection(&send_critical_section);
	}

	~CClientDynamicVirtualChannel()
	{
		if (channels[0].read_buffer)
		{
			free(channels[0].read_buffer);
		}

		delete logger;
	}

	void enumerate()
	{

	}

	bool connect()
	{
		return connect(1);
	}
	bool connect(DWORD num_retries)
	{
		DWORD last_error;
		DWORD retry;
		DWORD flags = 0;

		if (channels[0].dynamic)
		{
			flags = WTS_CHANNEL_OPTION_DYNAMIC | WTS_CHANNEL_OPTION_DYNAMIC_PRI_HIGH;
		}

		for (retry = 0; retry < num_retries; retry++)
		{
			channels[0].handle = WTSVirtualChannelOpenEx(WTS_CURRENT_SESSION, (LPSTR)channels[0].channel_name.c_str(), flags);
			if (channels[0].handle == NULL)
			{
				last_error = GetLastError();
				sprintf_s(log_buffer, sizeof(log_buffer), "[connect] failed to connect! last_error = %08x\r\n", last_error);
				logger->log(log_buffer);

				Sleep(100);
			}
			else
			{
				sprintf_s(log_buffer, sizeof(log_buffer), "[connect] connected!\r\n");
				logger->log(log_buffer);

				break;
			}
		}
		if (channels[0].handle == NULL)
		{
			last_error = GetLastError();
			sprintf_s(log_buffer, sizeof(log_buffer), "[connect] failed to connect in all %ld retries\r\n", num_retries);
			logger->log(log_buffer);

			return false;
		}

		return true;
	}

	void disconnect()
	{
		WTSVirtualChannelClose(channels[0].handle);
		channels[0].handle = NULL;
	}

	bool reconnect(bool sleep = true)
	{
		disconnect();

		if (sleep)
		{
			// give the client time to restart
			Sleep(STARTUP_SLEEP);
		}

		return connect(10);
	}

	bool read()
	{
		bool rc;
		DWORD bytes_read = 0;
		DWORD last_error;

		if (channels[0].read_buffer == NULL)
		{
			channels[0].read_buffer = malloc(READ_MAX_SIZE);
			if (channels[0].read_buffer == NULL)
			{
				return false;
			}
		}

		rc = WTSVirtualChannelRead(channels[0].handle, channels[0].timeout, (PCHAR)channels[0].read_buffer, READ_MAX_SIZE, &bytes_read);
		if (rc == 0)
		{
			last_error = GetLastError();
			sprintf_s(log_buffer, sizeof(log_buffer), "[read] failed to read! last_error = %08x\r\n", last_error);
			logger->log(log_buffer);
		}
		else
		{
			sprintf_s(log_buffer, sizeof(log_buffer), "[read] read %ld bytes\r\n", bytes_read);
			logger->log(log_buffer);
		}

		return rc;
	}

	bool write(PVOID data, DWORD data_size)
	{
		return write(data, data_size, true, false);
	}

	bool write(PVOID data, DWORD data_size, bool reconnect_required = true, bool lock = false)
	{
		bool rc = false;
		DWORD bytes_written = 0;
		DWORD last_error;

		if (!channels[0].handle)
		{
			connect();
		}

		if (lock)
		{
			EnterCriticalSection(&send_critical_section);
		}

		if (data)
		{
			count++;

			if (WTSVirtualChannelWrite(channels[0].handle, (PCHAR)data, data_size, &bytes_written) == 0)				// first write failed - if this is a disconnection try to reconnect
			{
				last_error = GetLastError();

				sprintf_s(log_buffer, sizeof(log_buffer), "[write] write failed last_error=%ld\r\n", last_error);
				logger->log(log_buffer);

				if (last_error == ERROR_PIPE_NOT_CONNECTED)
				{
					if (reconnect(false))
					{
						if (WTSVirtualChannelWrite(channels[0].handle, (PCHAR)data, data_size, &bytes_written) == 0)	// second write failed - too bad
						{
							sprintf_s(log_buffer, sizeof(log_buffer), "[write] write failed after reconnect last_error=%ld\r\n", GetLastError());
							logger->log(log_buffer);
						}
						else
						{
							rc = true;
						}
					}
				}
			}
			else
			{
				rc = true;
			}
		}

		if (lock)
		{
			LeaveCriticalSection(&send_critical_section);
		}

		return rc;
	}

	bool reset()
	{
		bool rc;

		rc = reconnect(false);

		if (!rc)
		{
			rc = reconnect(true);
		}

		return rc;
	}

private:

	std::vector<channel_data> channels;

	CLogger *logger;
	std::string log_message;
	CHAR log_buffer[1024];

	DWORD count;

	CRITICAL_SECTION send_critical_section;
};

class CServerVirtualChannel : public CSender // sending messeages ***TO*** the server
{
public:
	CServerVirtualChannel(std::string msg_file_name)
	{
		CHAR log_file_name[MAX_PATH];
		channel_data data;

		data.channel_name = "SERVER";
		data.timeout = 0;
		data.dynamic = true;
		data.read_buffer = NULL;
		data.handle = NULL;

		channels.push_back(data);

		count = 0;
		consecutive_failures = 0;

		server_msg_file_name = msg_file_name;

		sprintf_s(log_file_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\ServerVirtualChannel-%ld.txt", GetCurrentProcessId());
		logger = new CLogger(log_file_name, false, false);

		InitializeCriticalSection(&send_critical_section);
	}

	~CServerVirtualChannel()
	{
		if (channels[0].read_buffer)
		{
			free(channels[0].read_buffer);
		}

		delete logger;
	}

	void enumerate()
	{

	}

	bool connect()
	{
		return true;
	}

	void disconnect()
	{

	}

	bool reconnect(bool sleep = true)
	{
		return true;
	}

	bool read()
	{
		return true;
	}

	bool write(PVOID data, DWORD data_size)
	{
		// simply write the file to the predefined shared folder
		// a process on the other side must query this folder and send messages when a file exists

		bool success = false;
		DWORD i;
		HANDLE server_msg_file_handle;
		DWORD bytes_written = 0;

		for (i = 0; i < MAX_NUM_SERVER_WRITE_TRIES; i++)
		{
			server_msg_file_handle = CreateFileA(server_msg_file_name.c_str(), FILE_ALL_ACCESS, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

			if (server_msg_file_handle != INVALID_HANDLE_VALUE)
			{
				if (WriteFile(server_msg_file_handle, data, data_size, &bytes_written, NULL))
				{
					success = true;
					CloseHandle(server_msg_file_handle);
					break;
				}
			}
			Sleep(10);
		}

		if (success)
		{
			consecutive_failures = 0;
		}
		else
		{
			consecutive_failures++;
			if (consecutive_failures > MAX_CONSECUTIVE_FAILS)
			{
				if (DeleteFileA(server_msg_file_name.c_str()))
				{
					consecutive_failures = 0;
				}
			}
		}

		return success;
	}

	bool reset()
	{
		return true;
	}

private:

	std::vector<channel_data> channels;

	std::string server_msg_file_name;

	CLogger *logger;
	std::string log_message;
	CHAR log_buffer[1024];

	DWORD count;

	CRITICAL_SECTION send_critical_section;

	DWORD consecutive_failures;
};
