#include "dllexports.h"
#include "virtualmachine.h"
#include "commandmap.h"
#include "value.h"
#include "vmstack.h"
#include "configdata.h"
#include "Entry.h"
#include <iostream>
#include <sstream>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <cstring>
#else
#include <limits.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <execinfo.h>
#endif

std::string get_working_dir()
{
#if defined(_WIN32) || defined(_WIN64)
	char buffer[MAX_PATH];
	_getcwd(buffer, MAX_PATH);
	return std::string(buffer);
#elif defined(__GNUC__)
	char buffer[PATH_MAX];
	getcwd(buffer, PATH_MAX);
	return std::string(buffer);
#else
#error "NO IMPLEMENTATION AVAILABLE"
#endif
}
extern "C" {
	DLLEXPORT_PREFIX void sqfvm_init(unsigned long long limit)
	{
		sqfvm_virtualmachine = std::make_shared<sqf::virtualmachine>(sqfvm_exportstarget, limit);
		sqfvm_virtualmachine->allow_suspension(false);
#if !defined(FILESYSTEM_DISABLE_DISALLOW)
		sqfvm_virtualmachine->get_filesystem().disallow(true);
#endif
		sqfvm_virtualmachine->disable_networking();
		sqf::commandmap::get().init();
	}
	DLLEXPORT_PREFIX void sqfvm_exec(const char* code, char* buffer, unsigned int bufferlen)
	{
		std::stringstream sstream;
		bool err;
		std::string executable_path;
		{
#if defined(_WIN32) || defined(_WIN64)
			char buffer[MAX_PATH];
			_getcwd(buffer, MAX_PATH);
			executable_path = sqf::filesystem::sanitize(buffer);
#elif defined(__GNUC__)
			char buffer[PATH_MAX];
			getcwd(buffer, PATH_MAX);
			executable_path = sqf::filesystem::sanitize(buffer);
#else
#error "NO IMPLEMENTATION AVAILABLE"
#endif
		}
		auto inputAfterPP = sqfvm_virtualmachine->preprocess(code, err, "__libraryfeed.sqf");
		if (!err)
		{
			sqfvm_virtualmachine->parse_sqf(inputAfterPP, "__libraryfeed.sqf");
			auto result = sqfvm_virtualmachine->execute(sqf::virtualmachine::execaction::start);
			if (result != sqf::virtualmachine::execresult::OK)
			{
				sqfvm_virtualmachine->execute(sqf::virtualmachine::execaction::abort);
			}
			sstream << sqfvm_exportstarget.str();
			sqfvm_exportstarget.clear();
			auto val = sqfvm_virtualmachine->active_vmstack()->last_value();
			if (val.data() != nullptr)
			{
				sstream << "[WORK]\t<" << sqf::type_str(val.dtype()) << ">\t" << val.as_string() << std::endl;
			}
			else
			{
				sstream << "[WORK]\t<" << "EMPTY" << ">\t" << std::endl;
			}
		}
		else
		{
			sstream << sqfvm_exportstarget.str();
			sqfvm_exportstarget.clear();
		}
		auto str = sstream.str();
		memset(buffer, 0, sizeof(char) * bufferlen);
#ifdef _WIN32
		strncpy_s(buffer, str.length() + 1, str.c_str(), bufferlen);
#else
		std::strncpy(buffer, str.c_str(), bufferlen);
#endif
	}

	DLLEXPORT_PREFIX void sqfvm_loadconfig(const char* cfg)
	{
		sqfvm_virtualmachine->parse_config(cfg);
	}

	DLLEXPORT_PREFIX void sqfvm_uninit()
	{
		sqf::commandmap::get().uninit();
		sqfvm_virtualmachine = std::shared_ptr<sqf::virtualmachine>();
	}
}

void exportstarget::log(loglevel level, std::string_view message)
{
	m_sstream << Logger::loglevelstring(level) << " " << message << std::endl;
}
