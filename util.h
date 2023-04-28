#pragma once

static void print_hex(unsigned char* bytes, size_t n)
{
	for (size_t i = 0; i < n; ++i)
	{
		printf("%02X ", bytes[i]);
	}
	printf("\n");
}

static void print_hex2(unsigned char* bytes, size_t n, size_t perline)
{
	size_t numlines = n / perline;
	size_t leftover = n % perline;
	size_t index = 0;
	size_t offsz = 0;
	for (size_t i = 0; i < numlines + 1; ++i)
	{
		printf("%02X: ", offsz);
		for (size_t k = 0; k < perline; ++k)
		{
			if (index >= n)
				break;
			printf("%02X ", bytes[index]);
			++index;
		}
		offsz += perline;
		printf("\n");
	}
	printf("\n");
}

void set_console_color(bool b)
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!b)
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
	else
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
}

std::vector<unsigned char> read_file(const std::string path)
{
	std::vector<unsigned char> v;
	std::ifstream in(path, std::ios::binary | std::ios::ate);
	if (!in.is_open())
		return {};
	size_t fs = in.tellg();
	v.resize(fs);
	in.seekg(std::ios::beg);
	in.read((char*)v.data(), fs);
	in.close();
	return v;
}

void* myalloc(void* q, unsigned n, unsigned m)
{
	(void)q;
	return calloc(n, m);
}

void myfree(void* q, void* p)
{
	(void)q;
	free(p);
}

static alloc_func zalloc = myalloc;
static free_func zfree = myfree;

void test_large_inflate(unsigned char* compr, size_t comprLen, std::vector<unsigned char>& uncompressed)
{
	int err;
	z_stream d_stream; /* decompression stream */

	d_stream.zalloc = zalloc;
	d_stream.zfree = zfree;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in = compr;
	d_stream.avail_in = (uInt)comprLen;

	err = inflateInit(&d_stream);
	//CHECK_ERR(err, "inflateInit");
	unsigned char buf[4096];
	for (;;) {
		d_stream.next_out = buf;            /* discard the output */
		d_stream.avail_out = sizeof(buf);
		err = inflate(&d_stream, Z_NO_FLUSH);

		size_t nr = sizeof(buf) - d_stream.avail_out;

		uncompressed.insert(uncompressed.end(), buf, buf + nr);

		if (err == Z_STREAM_END) break;
		//CHECK_ERR(err, "large inflate");
	}

	err = inflateEnd(&d_stream);
	//CHECK_ERR(err, "inflateEnd");
#if 0
	if (d_stream.total_out != 2 * uncomprLen + comprLen / 2) {
		fprintf(stderr, "bad large inflate: %ld\n", d_stream.total_out);
		exit(1);
	}
	else {
		printf("large_inflate(): OK\n");
	}
#endif
}

void write_file(const std::string path, std::vector<unsigned char>& v)
{
	if (v.empty())
	{
		printf("data is empty, can't write to '%s'\n", path.c_str());
		return;
	}
	FILE* fp = 0;
	fopen_s(&fp, path.c_str(), "wb");
	if (!fp)
	{
		printf("can't write to '%s'\n", path.c_str());
		return;
	}
	fwrite(v.data(), 1, v.size(), fp);
	fclose(fp);
}