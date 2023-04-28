#pragma once

//TODO: FIXME serializer/deserializer with process interface
struct BufferReader
{
	unsigned char* buffer;
	size_t size;
	size_t cursor;
	std::vector<std::string> strings;
	BufferReader(unsigned char* buffer_, size_t n) : buffer(buffer_), size(n), cursor(0)
	{
		strings.push_back("");
		strings.push_back("[defined elsewhere]");
	}

	void hexdump(size_t n, size_t perline, size_t base)
	{
		size_t numlines = n / perline;
		size_t leftover = n % perline;
		size_t index = 0;
		size_t offsz = 0;
		for (size_t i = 0; i < numlines + 1; ++i)
		{
			printf("%02X: ", offsz + base);
			for (size_t k = 0; k < perline; ++k)
			{
				if (index >= n)
					break;
				printf("%02X ", buffer[base + index]);
				++index;
			}
			offsz += perline;
			printf("\n");
		}
		printf("\n");
	}

	template<typename T>
	T read()
	{
		T t;
		if (cursor + sizeof(T) > size)
			return {};
		memcpy(&t, &buffer[cursor], sizeof(T));
		//printf("\tread %d at %d (0x%x)\n", sizeof(T), cursor, cursor);
		cursor += sizeof(T);
		return t;
	}

	template<typename T, typename... Ts>
	bool type(T& t, std::string_view fmt, Ts&&... ts)
	{
#if 0
		auto message_ = std::vformat(fmt, std::make_format_args(std::forward<Ts>(ts)...));
		set_console_color(true);
		printf("[ Reading type %s (%d bytes) Message: %s ]\n", typeid(T).name(), sizeof(T), message_.c_str());
		hexdump(sizeof(T), 16, cursor);
		set_console_color(false);
#endif
		if (cursor + sizeof(T) > size)
			return false;
		//printf("type %d at 0x%x\n", sizeof(T), cursor);
		memcpy(&t, &buffer[cursor], sizeof(T));
		cursor += sizeof(T);
		return true;
	}
	template<typename T>
	bool type(T& t)
	{
#if 0
		set_console_color(true);
		printf("[ Reading type %s (%d bytes) ]\n", typeid(T).name(), sizeof(T));
		hexdump(sizeof(T), 16, cursor);
		set_console_color(false);
#endif
		if (cursor + sizeof(T) > size)
			return false;
		//printf("type %d at 0x%x\n", sizeof(T), cursor);
		memcpy(&t, &buffer[cursor], sizeof(T));
		cursor += sizeof(T);
		return true;
	}

	std::string read_string()
	{
		std::string s;
		while (1)
		{
			char c = read<char>();
			if (!c)
				break;
			s.push_back(c);
		}
		return s;
	}

	std::string* read_null_terminated_string(int32_t* ptr)
	{
		if (*ptr != -1)
		{
			if (*ptr == 0)
				return &strings[0];
			return &strings[1];
		}
		std::string s;
		while (1)
		{
			char c = read<char>();
			if (!c)
				break;
			s.push_back(c);
		}
		strings.push_back(s);
		*ptr = strings.size() - 1;
		return &strings[*ptr];
	}

	template<typename T>
	bool read_pointer(T*& ptr)
	{
		ptr = (T*)read<int32_t>();
		return true; //TODO: FIXME if overflowed
	}

	float read_float(bool big_endian = true)
	{
		union {
			float f;
			u32 i;
		} i2f;
		i2f.i = read_u32();
		return i2f.f;
	}

	vec3 read_vec3(bool big_endian = true)
	{
		vec3 v;
		v.x = read_float(big_endian);
		v.y = read_float(big_endian);
		v.z = read_float(big_endian);
		return v;
	}

	uint32_t read_u32(bool big_endian = true)
	{
		uint32_t value = read<uint32_t>();
		if (big_endian)
		{
			value = (value >> 24) |
				((value << 8) & 0x00FF0000) |
				((value >> 8) & 0x0000FF00) |
				(value << 24);
		}
		return value;
	}
	uint16_t read_u16(bool big_endian = true)
	{
		uint16_t value = read<uint16_t>();
		if (big_endian)
		{
			value = (value >> 8) | (value << 8);
		}
		return value;
	}
};