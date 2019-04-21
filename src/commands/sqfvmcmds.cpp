#include "../commandmap.h"
#include "../value.h"
#include "../cmd.h"
#include "../virtualmachine.h"
#include "../configdata.h"
#include "../arraydata.h"
#include "../innerobj.h"
#include "../objectdata.h"
#include "../parsepreprocessor.h"
#include "../vmstack.h"
#include "../sqfnamespace.h"
#include "../callstack_sqftry.h"
#include "../instruction.h"
#include "../codedata.h"
#include "filesystem.h"
#include <sstream>
#include <array>
#include <algorithm> 
#include <cctype>
#include <cmath>
#include <locale>

using namespace sqf;
namespace
{
	std::shared_ptr<value> assembly___code(virtualmachine* vm, std::shared_ptr<value> right)
	{
		auto codedata = right->data<sqf::codedata>();
		std::vector<std::shared_ptr<sqf::value>> outarr;
		for (auto it = codedata->instructions_rbegin(); it != codedata->instructions_rend(); it++)
		{
			outarr.push_back(std::make_shared<sqf::value>((*it)->to_string()));
		}
		return std::make_shared<sqf::value>(outarr);
	}
	std::shared_ptr<value> cmds___(virtualmachine* vm)
	{
		std::vector<std::shared_ptr<sqf::value>> outarr;
		auto str = std::make_shared<sqf::value>("n");
		for (auto& pair : commandmap::get().all_n())
		{
			outarr.push_back(std::make_shared<sqf::value>(std::vector<std::shared_ptr<sqf::value>> { str,
				std::make_shared<sqf::value>(pair.first)
			}));
		}
		str = std::make_shared<sqf::value>("u");
		for (auto& pair : commandmap::get().all_u())
		{
			for (auto& it : *pair.second.get())
			{
				outarr.push_back(std::make_shared<sqf::value>(std::vector<std::shared_ptr<sqf::value>> { str,
					std::make_shared<sqf::value>(pair.first),
					std::make_shared<sqf::value>(sqf::type_str(it->rtype()))
				}));
			}
		}
		str = std::make_shared<sqf::value>("b");
		for (auto& pair : commandmap::get().all_b())
		{
			for (auto& it : *pair.second.get())
			{
				outarr.push_back(std::make_shared<sqf::value>(std::vector<std::shared_ptr<sqf::value>> { str,
					std::make_shared<sqf::value>(sqf::type_str(it->ltype())),
					std::make_shared<sqf::value>(pair.first),
					std::make_shared<sqf::value>(sqf::type_str(it->rtype()))
				}));
			}
		}
		return std::make_shared<sqf::value>(outarr);
	}
	std::shared_ptr<value> cmdsimplemented___(virtualmachine* vm)
	{
		std::vector<std::shared_ptr<sqf::value>> outarr;
		auto str = std::make_shared<sqf::value>("n");
		for (auto& pair : commandmap::get().all_n())
		{
			if (pair.second->desc().length() == 0)
				continue;
			outarr.push_back(std::make_shared<sqf::value>(std::vector<std::shared_ptr<sqf::value>> { str,
				std::make_shared<sqf::value>(pair.first)
			}));
		}
		str = std::make_shared<sqf::value>("u");
		for (auto& pair : commandmap::get().all_u())
		{
			for (auto& it : *pair.second.get())
			{
				if (it->desc().length() == 0)
					continue;
				outarr.push_back(std::make_shared<sqf::value>(std::vector<std::shared_ptr<sqf::value>> { str,
					std::make_shared<sqf::value>(pair.first),
					std::make_shared<sqf::value>(sqf::type_str(it->rtype()))
				}));
			}
		}
		str = std::make_shared<sqf::value>("b");
		for (auto& pair : commandmap::get().all_b())
		{
			for (auto& it : *pair.second.get())
			{
				if (it->desc().length() == 0)
					continue;
				outarr.push_back(std::make_shared<sqf::value>(std::vector<std::shared_ptr<sqf::value>> { str,
					std::make_shared<sqf::value>(sqf::type_str(it->ltype())),
					std::make_shared<sqf::value>(pair.first),
					std::make_shared<sqf::value>(sqf::type_str(it->rtype()))
				}));
			}
		}
		return std::make_shared<sqf::value>(outarr);
	}
	std::shared_ptr<value> vm___(virtualmachine* vm)
	{
		std::vector<std::shared_ptr<sqf::value>> outarr;
		auto str = std::make_shared<sqf::value>("n");
		for (auto& pair : commandmap::get().all_n())
		{
			if (str_ew(pair.first.c_str(), "__") == 0)
				continue;
			outarr.push_back(std::make_shared<sqf::value>(std::vector<std::shared_ptr<sqf::value>> { str,
				std::make_shared<sqf::value>(pair.first)
			}));
		}
		str = std::make_shared<sqf::value>("u");
		for (auto& pair : commandmap::get().all_u())
		{
			if (str_ew(pair.first.c_str(), "__") == 0)
				continue;
			for (auto& it : *pair.second.get())
			{
				outarr.push_back(std::make_shared<sqf::value>(std::vector<std::shared_ptr<sqf::value>> { str,
					std::make_shared<sqf::value>(pair.first),
					std::make_shared<sqf::value>(sqf::type_str(it->rtype()))
				}));
			}
		}
		str = std::make_shared<sqf::value>("b");
		for (auto& pair : commandmap::get().all_b())
		{
			if (str_ew(pair.first.c_str(), "__") == 0)
				continue;
			for (auto& it : *pair.second.get())
			{
				outarr.push_back(std::make_shared<sqf::value>(std::vector<std::shared_ptr<sqf::value>> { str,
					std::make_shared<sqf::value>(sqf::type_str(it->ltype())),
					std::make_shared<sqf::value>(pair.first),
					std::make_shared<sqf::value>(sqf::type_str(it->rtype()))
				}));
			}
		}
		return std::make_shared<sqf::value>(outarr);
	}
	std::shared_ptr<value> tree___string(virtualmachine* vm, std::shared_ptr<value> right)
	{
		auto str = right->as_string();
		std::stringstream sstream;
		vm->parse_sqf_tree(str, &sstream);
		return std::make_shared<value>(sstream.str());
	}
	std::shared_ptr<value> help___string(virtualmachine* vm, std::shared_ptr<value> right)
	{
		std::stringstream sstream;
		auto str = right->as_string();
		bool wasfound = false;
		for (auto& pair : commandmap::get().all_n())
		{
			if (str_cmpi(pair.first.c_str(), static_cast<int>(pair.first.length()), str.c_str(), static_cast<int>(str.length())) != 0)
				continue;
			auto cmd = pair.second;
			if (cmd->desc().empty())
				vm->out() << "NULAR '" << pair.first << "'\t<" << cmd->name() << ">" << std::endl;
			else
				vm->out() << "NULAR '" << pair.first << "'\t<" << cmd->name() << ">\t" << cmd->desc() << std::endl;
			wasfound = true;
		}
		for (auto& pair : commandmap::get().all_u())
		{
			if (str_cmpi(pair.first.c_str(), static_cast<int>(pair.first.length()), str.c_str(), static_cast<int>(str.length())) != 0)
				continue;
			auto cmds = pair.second;
			for (auto& cmd : *cmds)
			{
				if (!cmd->desc().empty())
					vm->out() << "UNARY '" << pair.first << "'\t<" << cmd->name() << " " << sqf::type_str(cmd->rtype()) << ">\t" << cmd->desc() << std::endl;
				else
					vm->out() << "UNARY '" << pair.first << "'\t<" << cmd->name() << " " << sqf::type_str(cmd->rtype()) << ">" << std::endl;
				wasfound = true;
			}
		}
		for (auto& pair : commandmap::get().all_b())
		{
			if (str_cmpi(pair.first.c_str(), static_cast<int>(pair.first.length()), str.c_str(), static_cast<int>(str.length())) != 0)
				continue;
			auto cmds = pair.second;
			for (auto& cmd : *cmds)
			{
				if (!cmd->desc().empty())
					vm->out() << "BINARY '" << pair.first << "'\t<" << sqf::type_str(cmd->ltype()) << " " << cmd->name() << " " << sqf::type_str(cmd->rtype()) << ">\t" << cmd->desc() << std::endl;
				else
					vm->out() << "BINARY '" << pair.first << "'\t<" << sqf::type_str(cmd->ltype()) << " " << cmd->name() << " " << sqf::type_str(cmd->rtype()) << ">" << std::endl;
			}
			wasfound = true;
		}
		if (!wasfound)
		{
			vm->out() << "Could not find any command with that name." << std::endl;
		}
		return std::make_shared<value>();
	}
	std::shared_ptr<value> configparse___string(virtualmachine* vm, std::shared_ptr<value> right)
	{
		auto str = right->as_string();
		auto cd = std::make_shared<sqf::configdata>();
		vm->parse_config(str, cd);
		return std::make_shared<value>(cd, type::CONFIG);
	}
	std::shared_ptr<value> merge___config_config(virtualmachine* vm, std::shared_ptr<value> left, std::shared_ptr<value> right)
	{
		auto target = left->data<configdata>();
		auto source = right->data<configdata>();
		source->mergeinto(target);
		return std::make_shared<value>();
	}
	std::shared_ptr<value> allObjects__(virtualmachine* vm)
	{
		auto arr = std::make_shared<arraydata>();
		for (auto& object : vm->get_objlist())
		{
			arr->push_back(std::make_shared<value>(std::make_shared<objectdata>(object), OBJECT));
		}
		return std::make_shared<value>(arr, ARRAY);
	}
	std::shared_ptr<value> prettyprintsqf___string(virtualmachine* vm, std::shared_ptr<value> right)
	{
		auto str = right->as_string();
		vm->pretty_print_sqf(str);
		return std::make_shared<value>();
	}
	std::shared_ptr<value> exit___(virtualmachine* vm)
	{
		vm->exitflag(true);
		return std::make_shared<value>();
	}
	std::shared_ptr<value> exit___scalar(virtualmachine* vm, std::shared_ptr<value> right)
	{
		vm->exitflag(true, static_cast<int>(std::round(right->as_float())));
		return std::make_shared<value>();
	}
	std::shared_ptr<value> respawn___(virtualmachine* vm)
	{
		vm->player_obj(innerobj::create(vm, "CAManBase", false));
		return std::make_shared<value>(std::make_shared<objectdata>(vm->player_obj()), type::OBJECT);
	}
	std::shared_ptr<value> preprocess___string(virtualmachine* vm, std::shared_ptr<value> right)
	{
		auto content = right->as_string();
		bool errflag = false;
		auto ppres = sqf::parse::preprocessor::parse(vm, content, errflag, "__preprocess__.sqf");
		if (errflag)
		{
			return std::make_shared<value>();
		}
		else
		{
			return std::make_shared<value>(ppres);
		}
	}
	std::shared_ptr<value> except___code_code(virtualmachine* vm, std::shared_ptr<value> left, std::shared_ptr<value> right)
	{
		auto cs = std::make_shared<callstack_sqftry>(vm->active_vmstack()->stacks_top()->get_namespace(), right->data<codedata>());
		vm->active_vmstack()->pushcallstack(cs);
		left->data<codedata>()->loadinto(vm->active_vmstack(), cs);
		return std::shared_ptr<value>();
	}
	std::shared_ptr<value> callstack___(virtualmachine* vm)
	{
		auto stackdump = vm->active_vmstack()->dump_callstack_diff({});
		auto sqfarr = std::make_shared<arraydata>();
		for (auto& it : stackdump)
		{
			std::vector<std::shared_ptr<sqf::value>> vec = {
					std::make_shared<sqf::value>(it.namespace_used->get_name()),
					std::make_shared<sqf::value>(it.scope_name),
					std::make_shared<sqf::value>(it.callstack_name),
					std::make_shared<sqf::value>(it.line),
					std::make_shared<sqf::value>(it.column),
					std::make_shared<sqf::value>(it.file),
					std::make_shared<sqf::value>(it.dbginf)
			};
			sqfarr->push_back(std::make_shared<sqf::value>(std::make_shared<arraydata>(vec), sqf::type::ARRAY));
		}
		return std::make_shared<value>(sqfarr, sqf::type::ARRAY);
	}
	std::shared_ptr<value> allfiles___(virtualmachine* vm, std::shared_ptr<sqf::value> right)
	{
#if !defined(FILESYSTEM_DISABLE_DISALLOW)
		if (vm->get_filesystem().disallow())
		{
			vm->wrn() << "FILE SYSTEM IS DISABLED" << std::endl;
			return std::make_shared<sqf::value>(std::make_shared<arraydata>(), sqf::type::ARRAY);
		}
#endif
		auto arr = right->data<arraydata>();
		if (!arr->check_type(vm, sqf::type::STRING, 0, arr->size()))
		{
			return {};
		}
		auto files = std::vector<std::shared_ptr<sqf::value>>();
		//recursively search for pboprefix
		for (auto phys : vm->get_filesystem().m_physicalboundaries)
		{
			for (auto i = std::filesystem::recursive_directory_iterator(phys, std::filesystem::directory_options::follow_directory_symlink);
				i != std::filesystem::recursive_directory_iterator();
				++i)
			{
				bool skip = false;
				for (auto& ext : *arr) {
					if (i->is_directory() || i->path().extension().compare(ext->as_string()))
					{
						skip = true;
						break;
					}
				}
				if (!skip)
				{
					files.push_back(std::make_shared<sqf::value>(i->path().string()));
				}
			}
		}
		return std::make_shared<sqf::value>(std::make_shared<arraydata>(files), sqf::type::ARRAY);
	}
	std::shared_ptr<value> pwd___(virtualmachine* vm)
	{
		auto path = std::filesystem::path(vm->active_vmstack()->stacks_top()->current_instruction()->file());
		return std::make_shared<sqf::value>(std::filesystem::absolute(path).string());
	}
	std::shared_ptr<value> currentDirectory___(virtualmachine* vm)
	{
		auto path = std::filesystem::path(vm->active_vmstack()->stacks_top()->current_instruction()->file());
		
		return std::make_shared<sqf::value>(std::filesystem::absolute(path.parent_path()).string());
	}
	std::shared_ptr<value> trim___(virtualmachine* vm, std::shared_ptr<sqf::value> right)
	{
		auto str = right->as_string();
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
			return !std::isspace(ch);
			}));
		str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) {
			return !std::isspace(ch);
			}).base(), str.end());
		return std::make_shared<sqf::value>(str);
	}
}
void sqf::commandmap::initsqfvmcmds()
{
	add(unary("tree__", sqf::type::STRING, "Returns a string containing the abstract syntax tree for the provided SQF expression.", tree___string));
	add(nular("cmds__", "Returns an array containing all commands available.", cmds___));
	add(nular("cmdsimplemented__", "Returns an array containing all commands that are actually implemented.", cmdsimplemented___));
	add(unary("help__", sqf::type::STRING, "Displays all available information for a single command.", help___string));
	add(unary("configparse__", sqf::type::STRING, "Parses provided string as config into a new config object.", configparse___string));
	add(binary(4, "merge__", sqf::type::CONFIG, sqf::type::CONFIG, "Merges contents from the right config into the left config. Duplicate entries will be overriden. Contents will not be copied but referenced.", merge___config_config));
	add(nular("allObjects__", "Returns an array containing all objects created.", allObjects__));
	add(unary("prettyprintsqf__", sqf::type::STRING, "Takes provided SQF code and pretty-prints it to output.", prettyprintsqf___string));
	add(nular("exit__", "Exits the VM execution immediately. Will not notify debug interface when used.", exit___));
	add(unary("exit__", sqf::type::SCALAR, "Exits the VM execution immediately. Will not notify debug interface when used. Allows to pass an exit code to the VM.", exit___scalar));
	add(nular("vm__", "Provides a list of all SQF-VM only commands.", vm___));
	add(nular("respawn__", "'Respawns' the player object.", respawn___));
	add(unary("preprocess__", sqf::type::STRING, "Runs the PreProcessor on provided string.", preprocess___string));
	add(unary("assembly__", sqf::type::CODE, "returns an array, containing the assembly instructions as string.", assembly___code));
	add(binary(4, "except__", sqf::type::CODE, sqf::type::CODE, "Allows to define a block that catches VM exceptions. It is to note, that this will also catch exceptions in spawn! Exception will be put into the magic variable '_exception'. A callstack is available in '_callstack'.", except___code_code));
	add(nular("callstack__", "Returns an array containing the whole callstack.", callstack___));
	add(unary("allFiles__", sqf::type::ARRAY, "Returns all files available in currently loaded paths with the given file extensions.", allfiles___));
	add(nular("pwd__", "Current path determined by current instruction.", pwd___));
	add(nular("currentDirectory__", "Current directory determined by current instruction.", currentDirectory___));
	add(unary("trim__", sqf::type::STRING, "Trims provided strings start and end.", trim___));
}
