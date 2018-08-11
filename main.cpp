#include "stdafx.h"
#include "windows.h"

#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <io.h>
#include <fcntl.h>

int main()
{
	namespace fs = std::filesystem;
	using std::endl;

	auto& out = std::wcout;
	auto& err = std::wcerr;

	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stdin), _O_U16TEXT);
	_setmode(_fileno(stderr), _O_U16TEXT);

	try {
		using string = std::wstring;

		fs::path program_path;
		{
			const string program_dirname = L"program";
			program_path = fs::current_path() / program_dirname;
		}
		fs::path target_path;
		{
			char* get_env(const char*);
			const fs::path programFiles_path = get_env("ProgramFiles");
			const string program_name = L"ITR-Remote";

			target_path = programFiles_path / program_name;
		}

		if (fs::exists(program_path))
		{
			out << L"File exists. Copy program to ProgramFiles." << endl;
			fs::copy(program_path, target_path, fs::copy_options::recursive);
			{
				out << L"Add program to autostart." << endl;

				const string program_exec = L"client.exe";
				const string prog_path_str = target_path / program_exec;

				HKEY hkey = NULL;
				LONG createStatus = RegCreateKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
				LONG status = RegSetValueEx(hkey, L"ITR-Remote", 0, REG_SZ, (BYTE *)prog_path_str.c_str(), (prog_path_str.size() + 1) * sizeof(wchar_t));
			}
			out << "Done!" << endl;
		}
		else
		{
			out << L"File doesn't exists!" << endl;
		}
	}
	catch (const fs::filesystem_error& e)
	{
		err << L"Runtime error:" << e.what() << endl;
		out << L"Run the program with administrator privileges." << endl;
	}
	catch (const std::runtime_error e)
	{
		err << L"Runtime error:" << e.what() << endl;
	}
	catch (const std::exception e)
	{
		err << e.what() << endl;
	}

	system("pause");

	return 0;
}

char* get_env(const char* env_name)
{
	char* var_buf;
	size_t requiredSize;

	getenv_s(&requiredSize, NULL, 0, env_name);
	if (requiredSize == 0)
	{
		throw std::runtime_error("Variable doesn't exists!");
	}

	var_buf = (char*)malloc(requiredSize * sizeof(char));
	if (!var_buf)
	{
		throw std::runtime_error("Failed to allocate memory!");
	}

	getenv_s(&requiredSize, var_buf, requiredSize, env_name);

	return var_buf;
}
