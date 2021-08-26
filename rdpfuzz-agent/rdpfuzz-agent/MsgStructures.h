#pragma once
#include "windows.h"
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <boost/property_tree/ptree.hpp>                                        
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional/optional.hpp>
#include "logger.h"

#define MINIMAL_MSG_BUFFER_SIZE				0x100

#define TEST_1 "\x5F\x5F\x5F\x63\x6D\x64\x30\x31\x00\x00\x08\x00\x20\x40\x00\x0A\x01\x54\x00\x21\x01\x57\x01\x00\x00\x01\x51\x1C\x00\x00\x00\x00\x00\x47\x01\x00\x00\x00\x00\x00\x00\x00\x00\x13\x00\x00\x00\x16\x00\x00\x00\x00\x00\x17\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\x04\x84\x00\x17\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\x00\x17\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\x06\x84\x05\x84\x04\x84\x04\x84\x06\x84\x06\x84\x06\x84\x00\x17\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\x07\x84\x07\x84\x07\x84\x07\x84\x07\x84\x07\x84\x07\x84\x07\x84"
#define TEST_2 "\x5F\x5F\x5F\x63\x6D\x64\x30\x31\x00\x01\x09\x00\x18\x40\x00\x0A\x01\x54\x00\x21\x01\x57\x01\x00\x00\x01\x51\x1C\x00\x00\x00\x00\x00\x47\x01\x00\x00\x00\x00\x00\x00\x00\x00\x13\x00\x00\x00\x16\x00\x00\x00\x00\x00\x17\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\x04\x84\x00\x17\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\x00\x17\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\x06\x84\x05\x84\x04\x84\x04\x84\x06\x84\x06\x84\x06\x84\x00\x17\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFA\xAF\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\xFB\xB0\x00\x07\x84\x07\x84\x07\x84\x07\x84\x07\x84\x07\x84\x07\x84\x07\x84"

#define EXAMPLE_JSON "{	\"definition\":[\"that\", \t\"another\"]}"


#define EXAMPLE_JSON_2  \
"{\
	\"definition\" :\
	{\
		\"token\" : \"___cmd01\",\
		\"header_size\" : \"8\",\
		\"body_min_size\" : \"25\"\
	},\
	\"this_component\" :\
	{\
		\"subcomponents\" :\
		[\
			{\
			\"offset\":\"8\",\
			\"definition\" :\
				{\
				\"max\" : \"2000\"\
				}\
			},\
			{\
			\"offset\" : \"10\",\
			\"definition\" :\
				{\
				\"values\" :\
					[\
					\"0000\",\
					\"0300\",\
					\"0800\",\
					\"0a00\",\
					\"0b00\",\
					\"0c00\",\
					\"0e00\",\
					\"0f00\"\
					]\
				}\
			},\
			{\
			\"offset\" : \"12\",\
			\"definition\" :\
				{\
				\"values\" :\
					[\
					\"20\",\
					\"21\"\
					]\
				}\
			},\
			{\
			\"offset\" : \"13\",\
			\"definition\" :\
				{\
				}\
			},\
			{\
			\"offset\" : \"21\",\
			\"definition\" :\
				{\
				}\
			},\
			{\
			\"offset\" : \"25\",\
			\"definition\" :\
				{\
				}\
			}\
		]\
	}\
}"


#define DEFAULT_MARKER_OFFSET			0
#define DEFAULT_MARKER					0xc0u
#define DEFAULT_LAST_MARKER				0x20u
#define DEFAULT_MAX_MSGS				3

#define DEFAULT_CMDID_OFFSET			0
#define DEFAULT_CMDID_SIZE				2
#define DEFAULT_SIZE_OFSSET				2
#define DEFAULT_SIZE_SIZE				4
#define DEFAULT_COUNTER_OFSSET			0
#define DEFAULT_COUNTER_SIZE			0



struct FORMAT_MSG
{
	PUCHAR msg;
	DWORD msg_size;
};



class CHeader
{
public:
	CHeader(boost::property_tree::ptree definition)
	{
		cmd_id_offset = definition.get("cmd_id_offset", DEFAULT_CMDID_OFFSET);
		cmd_id_size = definition.get("cmd_id_size", DEFAULT_CMDID_SIZE);
		size_offset = definition.get("size_offset", DEFAULT_SIZE_OFSSET);
		size_size = definition.get("size_size", DEFAULT_SIZE_SIZE);
		counter_offset = definition.get("counter_offset", DEFAULT_COUNTER_OFSSET);
		counter_size = definition.get("counter_size", DEFAULT_COUNTER_SIZE);

		counter = 0;
	}

	void fix_header(PUCHAR data, DWORD size, UCHAR cmdid)
	{
		if (cmd_id_size)
		{
			data[cmd_id_offset] = cmdid;

			if (cmd_id_size - 1)
			{
				memset(data + cmd_id_offset + 1, 0, cmd_id_size - 1);
			}
		}

		switch (size_size)
		{
		case 1:
			data[size_offset] = (UCHAR)size;
			break;
		case 2:
			*(USHORT *)(data + size_offset) = (USHORT)size;
			break;
		case 4:
			*(DWORD *)(data + size_offset) = size;
			break;
		case 8:
			*(ULONGLONG *)(data + size_offset) = (ULONGLONG)size;
			break;
		case 0:
		default:
			break;
		}

		switch (counter_size)
		{
		case 1:
			data[counter_offset] = (UCHAR)counter;
			break;
		case 2:
			*(USHORT *)(data + counter_offset) = (USHORT)counter;
			break;
		case 4:
			*(DWORD *)(data + counter_offset) = counter;
			break;
		case 8:
			*(ULONGLONG *)(data + counter_offset) = (ULONGLONG)counter;
			break;
		case 0:
		default:
			break;
		}

		counter++;
	}

private:
	DWORD cmd_id_offset;
	DWORD cmd_id_size;
	DWORD size_offset;
	DWORD size_size;
	DWORD counter_offset;
	DWORD counter_size;

	DWORD counter;
};

class CMarker
{
public:

	CMarker(boost::property_tree::ptree definition)
	{
		offset = definition.get("offset", DEFAULT_MARKER_OFFSET);
		marker = (UCHAR)strtoul(definition.get<std::string>("marker").c_str(), NULL, 0);
		last_marker = (UCHAR)strtoul(definition.get<std::string>("last_marker").c_str(), NULL, 0);
	}

	void add_marker(PUCHAR data, DWORD size, bool last)
	{
		if (size > offset)
		{
			data[offset] += marker;

			if (last)
			{
				data[offset] += last_marker;
			}
		}
	}

private:
	DWORD offset;
	UCHAR marker;
	UCHAR last_marker;
};


class CMsgComponentValue
{
public:

	CMsgComponentValue()
	{

	}

	CMsgComponentValue(std::string value)
	{
		DWORD i;
		UCHAR current;
		CHAR temp;

		size = (DWORD)value.length() / 2;

		// values are always written as consecutive XX, where XX is the hex representation of the value
		// we insert it in reverse order - for simpler comparison
		// to do - generailze, simplify, error-protection
		for (i = 0; i < value.length(); i += 2)
		{
			temp = value[i + 2];
			value[i + 2] = 0;
			current = std::stoi(value.substr(i), 0, 16);
			value[i + 2] = temp;
			data.push_back(current);
		}
	}

	CMsgComponentValue(PUCHAR input, DWORD input_size)
	{
		DWORD i;

		size = input_size;
		for (i = 0; i < size; i++)
		{
			data.push_back(input[i]);
		}
	}

	void write(PUCHAR output)
	{
		DWORD i;

		for (i = 0; i < size; i++)
		{
			output[i] = data[i];
		}
	}

	friend inline bool operator<(const CMsgComponentValue& left, const CMsgComponentValue& right)
	{
		DWORD i;
		bool rc = false;

		for (i = 0; i < min(left.size, right.size); i++)
		{
			if (left.data[i] < right.data[i])
			{
				rc = true;
			}
			if (left.data[i] > right.data[i])
			{
				rc = false;
			}
		}

		if (left.size < right.size)
		{
			for (; i < right.size; i++)
			{
				if (right.data[i])
				{
					rc = true;
				}
			}
		}
		else if (right.size < left.size)
		{
			for (; i < left.size; i++)
			{
				if (left.data[i])
				{
					rc = false;
				}
			}
		}

		return rc;
	}

	friend inline bool operator>(const CMsgComponentValue& left, const CMsgComponentValue& right)
	{
		DWORD i;
		bool rc = false;

		for (i = 0; i < min(left.size, right.size); i++)
		{
			if (left.data[i] < right.data[i])
			{
				rc = false;
			}
			if (left.data[i] > right.data[i])
			{
				rc = true;
			}
		}

		if (left.size < right.size)
		{
			for (; i < right.size; i++)
			{
				if (right.data[i])
				{
					rc = false;
				}
			}
		}
		else if (right.size < left.size)
		{
			for (; i < left.size; i++)
			{
				if (left.data[i])
				{
					rc = true;
				}
			}
		}

		return rc;
	}

	friend inline bool operator==(const CMsgComponentValue& left, const CMsgComponentValue& right)
	{
		DWORD i;

		for (i = 0; i < min(left.size, right.size); i++)
		{
			if (left.data[i] != right.data[i])
			{
				return false;
			}
		}

		return left.size == right.size;
	}


private:
	std::vector<UCHAR> data;
	DWORD size;
};


class CMsgComponent
{
public:

	CMsgComponent()
	{

	};


	CMsgComponent(boost::property_tree::ptree component)
	{
		boost::property_tree::ptree::const_assoc_iterator it;
		boost::property_tree::ptree components;

		// check if min exists 
		it = component.find("min");
		if (it == component.not_found())
		{
			has_min = false;
			min_value = NULL;
		}
		else
		{
			has_min = true;
			// read the minimal value
			min_value = new CMsgComponentValue(component.get<std::string>("min"));
		}

		// check if max exists 
		it = component.find("max");
		if (it == component.not_found())
		{
			has_max = false;
			max_value = NULL;
		}
		else
		{
			has_max = true;
			// read the maximal value
			max_value = new CMsgComponentValue(component.get<std::string>("max"));
		}

		// check if values exist
		it = component.find("values");
		if (it != component.not_found())
		{
			// insert all possible values
			BOOST_FOREACH(boost::property_tree::ptree::value_type &v, component.get_child("values."))
			{
				ordered_possible_values.insert(v.second.data());
			}
		}

		// check if subcomponents exist
		it = component.find("subcomponents");
		if (it != component.not_found())
		{
			components = component.get_child("subcomponents");

			// insert all possible values
			BOOST_FOREACH(boost::property_tree::ptree::value_type &subcomponent, components)
			{
				subcomponents.push_back(std::pair<CMsgComponent, DWORD>(CMsgComponent(subcomponent.second.get_child("definition")), subcomponent.second.get("offset", 0)));
			}
		}
	}

	void set_min_value(CMsgComponentValue min)
	{
		*min_value = min;
		has_min = true;
	}

	void set_max_value(CMsgComponentValue max)
	{
		*max_value = max;
		has_max = true;
	}

	void add_value(CMsgComponentValue value)
	{
		ordered_possible_values.insert(value);
	}

	void set_values(std::vector<CMsgComponentValue> values)
	{
		for (DWORD i = 0; i != values.size(); i++)
		{
			ordered_possible_values.insert(values[i]);
		}
	}

	void add_component(std::pair<CMsgComponent, DWORD> subcomponent)
	{
		subcomponents.push_back(subcomponent);
	}

	void set_components(std::vector<std::pair<CMsgComponent, DWORD>> _subcomponents)
	{
		subcomponents = _subcomponents;
	}

	// we assume the input_size is always valid
	// we fix the data of the component inplace
	// variable length leaf components MUST override this function to extract the real size from the remaining size
	virtual void fix(PVOID input, DWORD remaining_size)
	{
		CMsgComponentValue value((PUCHAR)input, remaining_size);

		std::vector<std::pair<CMsgComponent, DWORD>>::iterator current;
		std::vector<std::pair<CMsgComponent, DWORD>>::iterator next;

		if (subcomponents.size())
		{
			for (next = subcomponents.begin(); next != subcomponents.end(); current = next++)
			{
				if (next == subcomponents.begin())
				{
					continue;
				}
				else
				{
					current->first.fix((PCHAR)input + current->second, next->second - current->second);
				}
			}
			current->first.fix((PCHAR)input + current->second, remaining_size - current->second);
		}
		else
		{
			// this is a leaf, it can just fix the data in place
			// if this is an array of subcomponents
			// it must override this section to change each entry

			if (has_min)
			{
				value = max(value, *min_value);
			}

			if (has_max)
			{
				value = min(value, *max_value);
			}

			if (!ordered_possible_values.empty())
			{
				auto closest_above = ordered_possible_values.lower_bound(value);
				if (closest_above != ordered_possible_values.end())
				{
					value = *closest_above;
				}
				else
				{
					value = *ordered_possible_values.rbegin();
				}
			}

			value.write((PUCHAR)input);
		}
	}

private:

	bool has_min = false;
	CMsgComponentValue * min_value;
	bool has_max = false;
	CMsgComponentValue * max_value;
	std::set<CMsgComponentValue> ordered_possible_values;
	std::vector<std::pair<CMsgComponent, DWORD>> subcomponents;
};

struct MsgDefinition
{
	std::string token;
	DWORD header_size;
	DWORD body_min_size;
	DWORD body_max_size;
	DWORD body_size_modulus;
	DWORD body_size_remainder;
	DWORD cmd_id;

	MsgDefinition& operator =(const MsgDefinition& other)
	{
		token = other.token;
		header_size = other.header_size;
		body_min_size = other.body_min_size;
		body_max_size = other.body_max_size;
		body_size_modulus = other.body_size_modulus;
		body_size_remainder = other.body_size_remainder;
		cmd_id = other.cmd_id;
		return *this;
	}
};

// CMsgStructure defines a single message type

class CMsgStructure
{
public:

	CMsgStructure(std::string json_definition)
	{
		boost::property_tree::ptree msg_tree;
		std::stringstream msg_stream;

		msg_stream << json_definition;
		boost::property_tree::read_json(msg_stream, msg_tree);

		read_definition(msg_tree.get_child("definition"));
		this_component = new CMsgComponent(msg_tree.get_child("this_component"));
	}

	CMsgStructure(boost::property_tree::ptree msg_tree)
	{
		read_definition(msg_tree.get_child("definition"));
		this_component = new CMsgComponent(msg_tree.get_child("this_component"));

		logger = new CLogger(std::string("C:\\RDPFuzz\\Logs\\msg_log").append(msg_definition.token.substr(6)).append(".txt"), false, true);
	}

	void read_definition(boost::property_tree::ptree definition)
	{
		msg_definition.token = definition.get<std::string>("token");
		msg_definition.header_size = definition.get("header_size", 8);
		msg_definition.body_min_size = definition.get("body_min_size", 0);
		msg_definition.body_max_size = definition.get("body_max_size", INFINITE);
		msg_definition.body_size_modulus = definition.get("body_size_modulus", 1);
		msg_definition.body_size_remainder = definition.get("body_size_remainder", 0);
		msg_definition.cmd_id = std::stoi(msg_definition.token.substr(6), 0, 16);
	}

	CMsgStructure(MsgDefinition _msg_definition)
	{
		msg_definition = _msg_definition;
	}

	// input points to the token in the input
	// body_size is the size of the body immediately after the token in the input
	// if header exists - it is written
	// if not - the data is zeroed out, and the caller is responsible for filling it in
	// writes the structure to the output
	// returns the size written to output
	DWORD fix(PCHAR input, DWORD body_size, PCHAR output, PCHAR header = NULL)
	{
		DWORD copy_size;
		DWORD new_body_size;
		bool pad;
		PCHAR current = output;

		new_body_size = fix_body_size(body_size);
		copy_size = min(new_body_size, body_size);
		pad = copy_size < new_body_size;

		if (header)
		{
			memcpy(current, header, msg_definition.header_size);
		}
		else
		{
			write_header(input, current);
		}
		current += msg_definition.header_size;

		memcpy(current, input + msg_definition.token.length(), copy_size);
		current += copy_size;

		if (pad)
		{
			memset(current, 0, new_body_size - copy_size);
			current += new_body_size - copy_size;
		}

		// size is fixed - now fix the data
		this_component->fix(output, (DWORD)(current - output));

		return (DWORD)(current - output);
	}

	// the default is to just zero out the header
	// inheritors may use the input in a smart(?) way
	virtual void write_header(PCHAR input, PCHAR output)
	{
		memset(output, 0, msg_definition.header_size);
	}

	bool is_token(PCHAR token_address, DWORD size)
	{
		if (size < (DWORD)msg_definition.token.length())
		{
			return false;
		}

		return memcmp(msg_definition.token.c_str(), token_address, msg_definition.token.length()) == 0;
	}

	void print()
	{
		sprintf_s(log_buffer, 0x1000, "msg\r\ndefinition:\r\ntoken=%s\r\nheader_size=%ld\r\nbody_min_size=%ld\r\nbody_max_size=%ld\r\nbody_size_modulus=%ld\r\nbody_size_remainder=%ld\r\ncmd_id=%ld\r\n",
			msg_definition.token.c_str(), msg_definition.header_size, msg_definition.body_min_size, msg_definition.body_max_size, msg_definition.body_size_modulus, msg_definition.body_size_remainder, msg_definition.cmd_id);
		logger->log(log_buffer);
	}

	MsgDefinition msg_definition;

private:

	DWORD fix_body_size(DWORD body_size)
	{
		DWORD new_body_size;
		DWORD remainder;

		// get the current size
		new_body_size = body_size;

		// find the best fit for the input size and the restrictions

		// if it is smaller than the minimum - change to the minimum
		new_body_size = max(new_body_size, msg_definition.body_min_size);

		// if it is larger than the maximum - change to the maximum
		new_body_size = min(new_body_size, msg_definition.body_max_size);

		// check the modulus
		remainder = new_body_size % msg_definition.body_size_modulus;
		if (remainder != msg_definition.body_size_remainder)
		{
			// we take smallest possible size larger than new_size
			if (remainder < msg_definition.body_size_remainder)
			{
				new_body_size = new_body_size + msg_definition.body_size_remainder - remainder;
			}
			else
			{
				new_body_size = new_body_size + msg_definition.body_size_modulus + msg_definition.body_size_remainder - remainder;
			}
		}

		return new_body_size;
	}

	CMsgComponent * this_component;	// a wrapper component for entire msg
	CLogger * logger;
	char log_buffer[0x1000];
};


// CMsgs defines the collection of supported message types and some global definitions

class CMsgs
{
public:
	CMsgs(std::string json_definition)
	{
		char log_name[MAX_PATH];

		sprintf_s(log_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\msgs_log-%ld.txt", GetCurrentProcessId());

		logger = new CLogger(log_name, false, false);

		logger->log("starting\r\n");

		boost::property_tree::ptree msgs_tree;
		std::stringstream msgs_stream;

		logger->log("json_definition:\r\n\r\n");

		logger->log(json_definition);
		logger->log("\r\n\r\n");

		msgs_stream << json_definition;
		boost::property_tree::read_json(msgs_stream, msgs_tree);

		max_msgs = msgs_tree.get("max_msgs", DEFAULT_MAX_MSGS);

		logger->log("before defining marker\r\n");

		marker = new CMarker(msgs_tree.get_child("marker"));

		logger->log("defined marker\r\n");

		header = new CHeader(msgs_tree.get_child("header"));

		logger->log("defined header\r\n");

		BOOST_FOREACH(boost::property_tree::ptree::value_type &v, msgs_tree.get_child("msg_definitions."))
		{
			msgs.push_back(v.second);
		}

		logger->log("defined everything\r\n");
	}

	DWORD fix(PCHAR input, DWORD size, PCHAR output)
	{
		DWORD input_offset;
		DWORD msg_index;
		DWORD msg_total_size;
		DWORD msg_body_size;
		DWORD input_size = size;
		DWORD output_offset = 0;
		DWORD msg_output_size;

		bool begins_with_header = false;

		sprintf_s(log_buffer, 0x1000, "[msgs::fix] size=%ld\r\n", size);
		logger->log(log_buffer);

		logger->log((PUCHAR)input, size);

		sprintf_s(log_buffer, 0x1000, "[msgs::fix] token_length=%lld\r\n", msgs[0].msg_definition.token.length());
		logger->log(log_buffer);

		// first make sure we start with a msg header
		for (auto msg = msgs.begin(); msg != msgs.end(); msg++)
		{
			if (msg->is_token(input, size))
			{
				begins_with_header = true;
			}
		}

		sprintf_s(log_buffer, 0x1000, "[msgs::fix] begins_with_header=%ld\r\n", begins_with_header);
		logger->log(log_buffer);

		if (!begins_with_header)
		{
			memmove(input + msgs[0].msg_definition.token.length(), input, input_size);
			memmove(input, msgs[0].msg_definition.token.c_str(), msgs[0].msg_definition.token.length());

			input_size += (DWORD)msgs[0].msg_definition.token.length();

			logger->log((PUCHAR)input, input_size);
		}

		sprintf_s(log_buffer, 0x1000, "[msgs::fix] input_size=%ld\r\n", input_size);
		logger->log(log_buffer);

		input_offset = 0;

		while (input_offset < input_size)
		{
			sprintf_s(log_buffer, 0x1000, "[msgs::fix] input_offset=%ld\r\n", input_offset);
			logger->log(log_buffer);

			// get the current msg index
			msg_index = get_msg_index(input + input_offset);
			if (msg_index == -1)
			{
				// this is very bad - log?
			}

			sprintf_s(log_buffer, 0x1000, "[msgs::fix] msg_index=%ld\r\n", msg_index);
			logger->log(log_buffer);

			// get the actual (provided) size of the msg
			msg_total_size = get_msg_size(input + input_offset, input_size - input_offset);
			if (msg_total_size == -1)
			{
				// this is very bad - log?
			}

			sprintf_s(log_buffer, 0x1000, "[msgs::fix] msg_total_size=%ld\r\n", msg_total_size);
			logger->log(log_buffer);

			msg_body_size = msg_total_size - (DWORD)msgs[msg_index].msg_definition.token.length();

			// write to output
			// advance the output_offset accordingly
			msg_output_size = msgs[msg_index].fix(input + input_offset, msg_body_size, output + output_offset);

			logger->log("[msgs::fix] wrote msg\r\n");

			fix_header(output + output_offset, msg_output_size, msg_index);

			logger->log((PUCHAR)output + output_offset, msg_output_size);

			output_offset += msg_output_size;

			logger->log("[msgs::fix] fixed header\r\n");

			// advance the input_offset
			input_offset += msg_total_size;
		}

		return output_offset;
	}

	DWORD get_msg_index(PCHAR start)
	{
		DWORD index = -1;
		DWORD i;

		for (i = 0; i < msgs.size(); i++)
		{
			if (msgs[i].is_token(start, INFINITE))
			{
				index = i;
				break;
			}
		}

		return index;
	}

	DWORD get_msg_size(PCHAR start, DWORD remaining_size)
	{
		DWORD i;

		for (i = 1; i < remaining_size; i++)
		{
			for (auto msg = msgs.begin(); msg != msgs.end(); msg++)
			{
				if (msg->is_token(start + i, remaining_size - i))
				{
					return i;
				}
			}
		}
		return remaining_size;
	}

	virtual DWORD split(PCHAR start, DWORD size, std::vector<FORMAT_MSG> *split_msgs)
	{
		DWORD offset = 0;
		FORMAT_MSG current_msg;
		DWORD num_msgs = 0;

		while (offset < size && num_msgs < max_msgs)
		{
			current_msg.msg_size = get_msg_size(start + offset, size - offset);

			current_msg.msg = (PUCHAR)malloc(max(size * 2, MINIMAL_MSG_BUFFER_SIZE));
			memcpy(current_msg.msg, start + offset, current_msg.msg_size);

			split_msgs->push_back(current_msg);

			offset += current_msg.msg_size;

			num_msgs++;
		}

		return (DWORD)split_msgs->size();
	}

	virtual void fix_header(PCHAR start, DWORD size, DWORD msg_index)
	{
		header->fix_header(PUCHAR(start), size, (UCHAR)msgs[msg_index].msg_definition.cmd_id);
	}

	virtual DWORD wrap(PCHAR start, DWORD size)
	{
		return size;
	}

	virtual void add_marker(PCHAR start, DWORD size, bool last)
	{
		marker->add_marker((PUCHAR)start, size, last);
	}

	std::vector<CMsgStructure> msgs;
	CMarker * marker;
	CHeader * header;

	CLogger * logger;
	char log_buffer[0x1000];
	DWORD max_msgs;

private:

	void print()
	{
		logger->log("msg definitions:\r\n");
		for (auto msg = msgs.begin(); msg != msgs.end(); msg++)
		{
			msg->print();
		}
	}


};


class CGfxMsgs : public CMsgs
{
public:

	CGfxMsgs(std::string json_definition) : CMsgs(json_definition) {}

	void fix_header(PCHAR start, DWORD size, DWORD msg_index)
	{
		*(USHORT *)start = (USHORT)msgs[msg_index].msg_definition.cmd_id;
		*(USHORT *)(start + 2) = 0;
		*(DWORD *)(start + 4) = size;
	}

	DWORD wrap(PCHAR start, DWORD size)
	{
		memmove(start + 2, start, size);
		*(UCHAR *)start = 0xE0;
		start[1] = 4;

		return size + 2;
	}

	void add_marker(PCHAR start, DWORD size, bool last)
	{
		start[0] -= 0xC0;
		if (last)
		{
			start[0] -= 0x20;
		}
	}
};


class CAudioMsgs : public CMsgs
{
public:

	CAudioMsgs(std::string json_definition) : CMsgs(json_definition) {}

	DWORD split(PCHAR start, DWORD size, std::vector<FORMAT_MSG> *split_msgs)
	{
		DWORD offset = 0;
		FORMAT_MSG current_msg;
		DWORD count = 0;
		DWORD max_split_msgs = 5;

		while (offset < size && count < max_split_msgs)
		{
			current_msg.msg_size = get_msg_size(start + offset, size - offset);
			current_msg.msg = (PUCHAR)malloc(max(size * 2, MINIMAL_MSG_BUFFER_SIZE));
			memcpy(current_msg.msg, start + offset, current_msg.msg_size);

			split_msgs->push_back(current_msg);

			offset += current_msg.msg_size;
			count++;
		}

		return (DWORD)split_msgs->size();
	}

	void fix_header(PCHAR start, DWORD size, DWORD msg_index)
	{
		*(UCHAR *)start = (UCHAR)msgs[msg_index].msg_definition.cmd_id;
	}

	DWORD wrap(PCHAR start, DWORD size)
	{
		return size;
	}

	void add_marker(PCHAR start, DWORD size, bool last)
	{
		start[0] += 0x10;

		if (last)
		{
			start[0] += 0x10;
		}
	}
};

class CGeometryMsgs : public CMsgs
{
public:

	CGeometryMsgs(std::string json_definition) : CMsgs(json_definition) {}

	DWORD split(PCHAR start, DWORD size, std::vector<FORMAT_MSG> *split_msgs)
	{
		DWORD offset = 0;
		FORMAT_MSG current_msg;

		while (offset < size)
		{
			current_msg.msg_size = get_msg_size(start + offset, size - offset);
			current_msg.msg = (PUCHAR)malloc(max(size * 2, MINIMAL_MSG_BUFFER_SIZE));
			memcpy(current_msg.msg, start + offset, current_msg.msg_size);

			split_msgs->push_back(current_msg);

			offset += current_msg.msg_size;
		}

		return (DWORD)split_msgs->size();
	}

	void fix_header(PCHAR start, DWORD size, DWORD msg_index)
	{
		DWORD *header = (DWORD *)start;
		header[0] = size;
		header[1] = 1;
		header[4] = msgs[msg_index].msg_definition.cmd_id;
	}

	DWORD wrap(PCHAR start, DWORD size)
	{
		return size;
	}

	void add_marker(PCHAR start, DWORD size, bool last)
	{
		DWORD *header = (DWORD *)start;

		header[1] += 0x10;

		if (last)
		{
			header[1] += 0x10;
		}
	}
};


class CVideoMsgs : public CMsgs
{
public:

	CVideoMsgs(std::string json_definition) : CMsgs(json_definition) {}

	DWORD split(PCHAR start, DWORD size, std::vector<FORMAT_MSG> *split_msgs)
	{
		DWORD offset = 0;
		FORMAT_MSG current_msg;
		DWORD count = 0;
		DWORD max_split_msgs = 5;

		while (offset < size && count < max_split_msgs)
		{
			current_msg.msg_size = get_msg_size(start + offset, size - offset);

			if (count == 0 && !is_presentation_request(start + offset, current_msg.msg_size))
			{
				add_fixed_start_presentation_request(split_msgs);
				count++;

				logger->log("added fixed msg\r\n");
			}
			else
			{
				if (count == 0 || !is_presentation_request(start + offset, current_msg.msg_size))
				{
					current_msg.msg = (PUCHAR)malloc(max(size * 2, MINIMAL_MSG_BUFFER_SIZE));
					memcpy(current_msg.msg, start + offset, current_msg.msg_size);

					split_msgs->push_back(current_msg);
					count++;

					logger->log("added fuzzer msg\r\n");
				}
				offset += current_msg.msg_size;
			}
		}

		add_fixed_stop_presentation_request(split_msgs);

		return (DWORD)split_msgs->size();
	}

	bool is_presentation_request(PCHAR msg, DWORD size)
	{
		return memcmp(msg, "___cmd01", 8) == 0;
	}


	void add_fixed_start_presentation_request(std::vector<FORMAT_MSG> *split_msgs)
	{
		byte start_presentation_request_msg[] =
		{
			0x5F,0x5F,0x5F,0x63,0x6D,0x64,0x30,0x31,0x10,0x01,0x01,0x00,0x01,0x00,0x00,0x00,0x03,0x01,0x01,0x1D,0xC0,0x12,0x00,0x00,0xE0,0x01,0x00,0x00,0xF4,0x00,0x00,0x00,0xE0,0x01,0x00,0x00,0xF4,0x00,0x00,0x00,0xA4,0x7A,0x3B,0x82,0x0F,0x00,0x00,0x00,0x22,0x02,0x04,0x00,0xBA,0x7A,0x00,0x80,0x48,0x32,0x36,0x34,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x67,0x42,0xC0,0x15,0x95,0xA0,0x78,0x21,0xF9,0xE1,0x00,0x00,0x03,0x00,0x01,0x00,0x00,0x03,0x00,0x3C,0x0D,0xA0,0x88,0x46,0xA0,0x00,0x00,0x00,0x01,0x68,0xCE,0x3C,0x80,0x00
		};

		FORMAT_MSG new_msg;

		new_msg.msg_size = 114;

		new_msg.msg = (PUCHAR)malloc(new_msg.msg_size);

		if (new_msg.msg)
		{
			memcpy(new_msg.msg, start_presentation_request_msg, new_msg.msg_size);
		}

		split_msgs->push_back(new_msg);
	}

	void add_fixed_stop_presentation_request(std::vector<FORMAT_MSG> *split_msgs)
	{
		byte start_presentation_request_msg[] =
		{
			0x5F,0x5F,0x5F,0x63,0x6D,0x64,0x30,0x31,0x10,0x01,0x02,0x00,0x01,0x00,0x00,0x00,0x03,0x01,0x01,0x1D,0xC0,0x12,0x00,0x00,0xE0,0x01,0x00,0x00,0xF4,0x00,0x00,0x00,0xE0,0x01,0x00,0x00,0xF4,0x00,0x00,0x00,0xA4,0x7A,0x3B,0x82,0x0F,0x00,0x00,0x00,0x22,0x02,0x04,0x00,0xBA,0x7A,0x00,0x80,0x48,0x32,0x36,0x34,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x67,0x42,0xC0,0x15,0x95,0xA0,0x78,0x21,0xF9,0xE1,0x00,0x00,0x03,0x00,0x01,0x00,0x00,0x03,0x00,0x3C,0x0D,0xA0,0x88,0x46,0xA0,0x00,0x00,0x00,0x01,0x68,0xCE,0x3C,0x80,0x00
		};

		FORMAT_MSG new_msg;

		new_msg.msg_size = 114;

		new_msg.msg = (PUCHAR)malloc(new_msg.msg_size);

		if (new_msg.msg)
		{
			memcpy(new_msg.msg, start_presentation_request_msg, new_msg.msg_size);
		}

		split_msgs->push_back(new_msg);
	}


	void fix_header(PCHAR start, DWORD size, DWORD msg_index)
	{
		DWORD *header = (DWORD *)start;
		header[0] = size;
		header[1] = msgs[msg_index].msg_definition.cmd_id;
	}

	DWORD wrap(PCHAR start, DWORD size)
	{
		return size;
	}

	void add_marker(PCHAR start, DWORD size, bool last)
	{
		DWORD *header = (DWORD *)start;

		header[1] += 0x10;

		if (last)
		{
			header[1] += 0x10;
		}
	}
};

class CdwmproxMsgs : public CMsgs
{
public:

	CdwmproxMsgs(std::string json_definition) : CMsgs(json_definition) {}

	DWORD split(PCHAR start, DWORD size, std::vector<FORMAT_MSG> *split_msgs)
	{
		DWORD offset = 0;
		FORMAT_MSG current_msg;

		while (offset < size)
		{
			current_msg.msg_size = get_msg_size(start + offset, size - offset);
			current_msg.msg = (PUCHAR)malloc(max(size * 2, MINIMAL_MSG_BUFFER_SIZE));
			memcpy(current_msg.msg, start + offset, current_msg.msg_size);

			split_msgs->push_back(current_msg);

			offset += current_msg.msg_size;
		}

		return (DWORD)split_msgs->size();
	}

	void fix_header(PCHAR start, DWORD size, DWORD msg_index)
	{
		DWORD *header = (DWORD *)start;
		header[0] = size;
		header[1] = msgs[msg_index].msg_definition.cmd_id;
	}

	DWORD wrap(PCHAR start, DWORD size)
	{
		return size;
	}

	void add_marker(PCHAR start, DWORD size, bool last)
	{
		DWORD *header = (DWORD *)start;

		header[1] += 0x10;

		if (last)
		{
			header[1] += 0x10;
		}
	}
};


class CGfxServerMsgs : public CMsgs // messages sent ***TO*** the server
{
public:

	CGfxServerMsgs(std::string json_definition) : CMsgs(json_definition) {}

	DWORD split(PCHAR start, DWORD size, std::vector<FORMAT_MSG> *split_msgs)
	{
		DWORD offset = 0;
		FORMAT_MSG current_msg;

		while (offset < size)
		{
			current_msg.msg_size = get_msg_size(start + offset, size - offset);
			current_msg.msg = (PUCHAR)malloc(max(size * 2, MINIMAL_MSG_BUFFER_SIZE));
			memcpy(current_msg.msg, start + offset, current_msg.msg_size);

			split_msgs->push_back(current_msg);

			offset += current_msg.msg_size;
		}

		return (DWORD)split_msgs->size();
	}

	void fix_header(PCHAR start, DWORD size, DWORD msg_index)
	{
		*(USHORT *)start = (USHORT)msgs[msg_index].msg_definition.cmd_id;
		*(USHORT *)(start + 2) = 0;
		*(DWORD *)(start + 4) = size;
	}

	DWORD wrap(PCHAR start, DWORD size)
	{
		return size;
	}

	void add_marker(PCHAR start, DWORD size, bool last)
	{
		start[0] += 0x80;

		if (last)
		{
			start[0] += 0x40;
		}
	}
};


class CInputServerMsgs : public CMsgs // messages sent ***TO*** the server
{
public:

	CInputServerMsgs(std::string json_definition) : CMsgs(json_definition) {}

	DWORD split(PCHAR start, DWORD size, std::vector<FORMAT_MSG> *split_msgs)
	{
		DWORD offset = 0;
		FORMAT_MSG current_msg;

		while (offset < size)
		{
			current_msg.msg_size = get_msg_size(start + offset, size - offset);
			current_msg.msg = (PUCHAR)malloc(max(size * 2, MINIMAL_MSG_BUFFER_SIZE));
			memcpy(current_msg.msg, start + offset, current_msg.msg_size);

			split_msgs->push_back(current_msg);

			offset += current_msg.msg_size;
		}

		return (DWORD)split_msgs->size();
	}

	void fix_header(PCHAR start, DWORD size, DWORD msg_index)
	{
		*(USHORT *)start = (USHORT)msgs[msg_index].msg_definition.cmd_id;
		*(DWORD *)(start + 2) = size;
	}

	DWORD wrap(PCHAR start, DWORD size)
	{
		return size;
	}

	void add_marker(PCHAR start, DWORD size, bool last)
	{
		start[0] += 0xC0;

		if (last)
		{
			start[0] += 0x20;
		}
	}
};

class CUSBServerMsgs : public CMsgs // messages sent ***TO*** the server
{
public:

	CUSBServerMsgs(std::string json_definition) : CMsgs(json_definition) {}

	DWORD split(PCHAR start, DWORD size, std::vector<FORMAT_MSG> *split_msgs)
	{
		DWORD offset = 0;
		FORMAT_MSG current_msg;

		while (offset < size)
		{
			current_msg.msg_size = get_msg_size(start + offset, size - offset);
			current_msg.msg = (PUCHAR)malloc(max(size * 2, MINIMAL_MSG_BUFFER_SIZE));
			memcpy(current_msg.msg, start + offset, current_msg.msg_size);

			split_msgs->push_back(current_msg);

			offset += current_msg.msg_size;
		}

		return (DWORD)split_msgs->size();
	}

	void fix_header(PCHAR start, DWORD size, DWORD msg_index)
	{
		return;
	}

	DWORD wrap(PCHAR start, DWORD size)
	{
		return size;
	}

	void add_marker(PCHAR start, DWORD size, bool last)
	{
		*(DWORD *)(start + 8) += 0x80000000;

		if (last)
		{
			*(DWORD *)(start + 8) += 0x40000000;
		}
	}
};



class CServerMsgs : public CMsgs // messages sent ***TO*** the server
{
public:

	CServerMsgs(std::string json_definition) : CMsgs(json_definition) {}

};

class CClientMsgs : public CMsgs // messages sent ***TO*** the server
{
public:

	CClientMsgs(std::string json_definition) : CMsgs(json_definition) {}

};


class CServerRdpdrMsgs : public CMsgs // messages sent ***TO*** the server
{
public:

	CServerRdpdrMsgs(std::string json_definition) : CMsgs(json_definition) {}

	DWORD split(PCHAR start, DWORD size, std::vector<FORMAT_MSG> *split_msgs)
	{
		DWORD offset = 0;
		FORMAT_MSG current_msg;
		DWORD count = 0;
		DWORD max_split_msgs = 8;

		std::set<DWORD> device_ids;

		while (offset < size && count < max_split_msgs)
		{
			current_msg.msg_size = get_msg_size(start + offset, size - offset);
			current_msg.msg = (PUCHAR)malloc(max(size * 2, MINIMAL_MSG_BUFFER_SIZE));
			memcpy(current_msg.msg, start + offset, current_msg.msg_size);

			split_msgs->push_back(current_msg);

			offset += current_msg.msg_size;

			count++;
		}

		print_rdpdr_msgs(split_msgs, &device_ids);

		if (!msg_is_device_announce(split_msgs->front()))
		{
			add_fixed_device_announce_msg(split_msgs, &device_ids);
		}

		print_rdpdr_msgs(split_msgs, &device_ids);

		for (auto msg = split_msgs->begin(); msg != split_msgs->end(); msg++)
		{
			if (msg_is_device_announce(*msg))
			{
				//extract_device_ids(split_msgs, msg, &device_ids);
				if (msg != split_msgs->begin())
				{
					change_device_announce_msg(msg);
				}
			}
		}

		print_rdpdr_msgs(split_msgs, &device_ids);

		add_device_remove_msg(split_msgs, &device_ids);

		print_rdpdr_msgs(split_msgs, &device_ids);

		for (auto msg = split_msgs->begin(); msg != split_msgs->end(); msg++)
		{
			if (msg_is_io_response(*msg))
			{
				fix_io_response_msg(*msg, &device_ids);
			}
		}

		print_rdpdr_msgs(split_msgs, &device_ids);

		return (DWORD)split_msgs->size();
	}

	BOOL msg_is_device_announce(FORMAT_MSG msg)
	{
		return memcmp(msg.msg, "___cmd41", 8) == 0;
	}

	BOOL msg_is_io_response(FORMAT_MSG msg)
	{
		return memcmp(msg.msg, "___cmd43", 8) == 0;
	}

	void add_fixed_device_announce_msg(std::vector<FORMAT_MSG> *msgs, std::set<DWORD> *device_ids)
	{
		FORMAT_MSG msg;

		UCHAR device_announce_msg[] =
		{
			'_','_','_','c','m','d','4','1',	// header: core component, announce
			0x01, 0x00, 0x00, 0x00,				// count
			0x20, 0x00, 0x00, 0x00,				// device type: filesystem
			0x20, 0x00, 0x00, 0x00,				// device id
			'S','C','A','R','D',0,0,0,		// device preferred name
			0x00, 0x00, 0x00, 0x00				// device data length
		};

		msg.msg_size = 32;
		msg.msg = (PUCHAR)malloc(msg.msg_size);

		memcpy(msg.msg, device_announce_msg, msg.msg_size);

		msgs->insert(msgs->begin(), msg);

		device_ids->insert(0x20);
	}


	void change_device_announce_msg(std::vector<FORMAT_MSG>::iterator msg)
	{
		msg->msg[7] = '3';
	}

	void add_device_remove_msg(std::vector<FORMAT_MSG> *msgs, std::set<DWORD> *device_ids)
	{
		FORMAT_MSG msg;
		DWORD *d_msg;
		DWORD i = 0;

		UCHAR device_remove_msg_header[] =
		{
			'_','_','_','c','m','d','4','d'		// header: core component, remove
		};

		msg.msg_size = 12 + (4 * (DWORD)device_ids->size());
		msg.msg = (PUCHAR)malloc(msg.msg_size);

		memcpy(msg.msg, device_remove_msg_header, 8);

		d_msg = (DWORD *)msg.msg;

		d_msg[2] = (DWORD)device_ids->size();

		for (auto id = device_ids->begin(); id != device_ids->end(); id++)
		{
			d_msg[3 + i] = *id;
			i++;
		}

		msgs->push_back(msg);
	}

	void extract_device_ids(std::vector<FORMAT_MSG> *msgs, std::vector<FORMAT_MSG>::iterator msg, std::set<DWORD> *device_ids)
	{
		DWORD * d_msg;
		DWORD count = 0;
		DWORD device_start_offset = 12;
		DWORD device_id;
		DWORD device_type;
		DWORD device_size;
		DWORD *device_start;

		if (msg->msg_size < 32)
		{
			PUCHAR new_msg;
			new_msg = (PUCHAR)malloc(32);
			memcpy(new_msg, msg->msg, msg->msg_size);
			memset(new_msg + msg->msg_size, 0, 32 - msg->msg_size);

			free(msg->msg);
			msg->msg = new_msg;
			msg->msg_size = 32;
		}

		// type (4), id (4), name (8), length (4), data (var)

		// for now we only allow one creation in every message
		// to do - treat general case
		device_start = (DWORD *)(msg->msg + device_start_offset);

		// everything after the length is the device data
		device_size = msg->msg_size - device_start_offset - 20;
		device_start[4] = device_size;

		// leave the loop as before, will only run once because we changed size

		while (device_start_offset < msg->msg_size)
		{
			if (msg->msg_size - device_start_offset < 20)
			{
				msg->msg_size = device_start_offset;
				break;
			}

			count++;

			device_start = (DWORD *)(msg->msg + device_start_offset);

			device_type = device_start[0];

			switch (device_type)
			{
			case 1:
				break;
			case 2:
				break;
			case 4:
				break;
				// we do not support filesystem devices
				//case 8:
				//	break;
			default:
				device_start[0] = 0x20;
				// name must be 'SCARD'
				byte device_name_scard[] = { 'S', 'C', 'A', 'R', 'D', 0, 0, 0 };
				memcpy(&device_start[2], device_name_scard, 8);
				break;
			}

			device_id = device_start[1];
			device_ids->insert(device_id);

			device_size = device_start[4];
			if (device_size > msg->msg_size - device_start_offset - 20)
			{
				device_size = msg->msg_size - device_start_offset - 20;
				device_start[4] = device_size;
			}

			device_start_offset += device_size + 20;
		}

		d_msg = (DWORD *)msg->msg;
		d_msg[2] = count;
	}

	void fix_io_response_msg(FORMAT_MSG msg, std::set<DWORD> *device_ids)
	{
		DWORD * d_msg = (DWORD *)msg.msg;
		auto id = device_ids->begin();
		d_msg[2] = *id;
	}

	void print_rdpdr_msgs(std::vector<FORMAT_MSG> *msgs, std::set<DWORD> *device_ids)
	{
		logger->log("\r\n[print_rdpdr_msgs] starting\r\n");

		for (DWORD i = 0; i < msgs->size(); i++)
		{
			char log_msg[1000];
			sprintf_s(log_msg, 1000, "\r\n[print_rdpdr_msgs] i=%i\r\n", i);

			logger->log(log_msg);
			print_rdpdr_msg((*msgs)[i]);
		}

		print_device_ids(device_ids);
	}

	void print_rdpdr_msg(FORMAT_MSG msg)
	{
		char current[4];

		char print_msg[0x10000];
		print_msg[0] = 0;

		logger->log("\r\n[print_rdpdr_msg] starting\r\n");

		for (DWORD i = 0; i < msg.msg_size; i++)
		{
			if ((i % 16) == 0)
			{
				strcat_s(print_msg, sizeof(print_msg), "\r\n");
			}

			else if ((i % 16) == 8)
			{
				strcat_s(print_msg, sizeof(print_msg), " ");
			}

			sprintf_s(current, sizeof(current), "%02x ", msg.msg[i]);
			strcat_s(print_msg, sizeof(print_msg), current);
		}

		logger->log(print_msg);
	}

	void print_device_ids(std::set<DWORD> *device_ids)
	{
		char current[16];
		char print_msg[0x10000];

		sprintf_s(print_msg, 0x10000, "\r\n[print_device_ids] starting device_ids.size()=%lld\r\n", device_ids->size());

		for (auto i = device_ids->begin(); i != device_ids->end(); i++)
		{
			sprintf_s(current, sizeof(current), "%ld ", *i);
			strcat_s(print_msg, sizeof(print_msg), current);
		}

		logger->log(print_msg);
	}
};