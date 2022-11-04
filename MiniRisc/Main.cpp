#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <thread>
#include "Processzor.h"
#include "Grafika.h"

using namespace std;




void main(int argc, char** argv)
{
	cout << "Fajl nev: ";
	string filename;
	//cin >> filename;
	filename = "test.s";
	ifstream is(filename);
	string input;
	bool code = true;

	Processzor proc;

	while (getline(is >> std::ws, input))
	{
		std::istringstream f(input);
		getline(f >> std::ws, input, ';');
		f = istringstream(input);
		std::string command;
		std::getline(f >> std::ws, command, ' ');

		//Defek vizsgálata
		string lower = command;
		std::for_each(lower.begin(), lower.end(), [](char& c)
			{
				c = ::tolower(c);
			});
		if (lower.compare("def") == 0)
		{
			string in;
			getline(f >> std::ws, in, ' ');
			string tag = in;
			getline(f >> std::ws, in);
			Cim ertek = stoul(in, nullptr, 16);
			proc.AddDef(tag, ertek);
			continue;
		}
		if (!code)
		{

			bool b = true;
			while (command.back() == ':')
			{
				command.pop_back();
				proc.AddAdatDef(command);
				b = (bool)std::getline(f >> std::ws, command, ' ');
				if (!b)
					break;
			}
			if (!b)
				continue;
			command.erase(remove_if(command.begin(), command.end(), isspace), command.end());
			std::for_each(command.begin(), command.end(), [](char& c)
				{
					c = ::tolower(c);
				});
			if (command.compare("code") == 0)
			{
				code = true;
				continue;
			}
			string in;
			Cim r1;

			if (command.compare("org") == 0)
			{
				getline(f >> std::ws, in);
				in.erase(remove_if(in.begin(), in.end(), isspace), in.end());
				r1 = stoul(in, nullptr, 16);
				if (r1 < 128)
					proc.SetDataCnt(r1);
			}
			else if (command.compare("db") == 0)
			{
				while (getline(f >> std::ws, in, ','))
				{
					in.erase(remove_if(in.begin(), in.end(), isspace), in.end());
					r1 = stoul(in, nullptr, 16);
					proc.AddData(r1);
				}
			}

			continue;
		}

		bool b = true;
		while (command.back() == ':')
		{
			command.pop_back();
			proc.AddUgras(command);
			b = (bool)std::getline(f >> std::ws, command, ' ');
			if (!b)
				break;
		}
		if (!b)
			continue;
		command.erase(remove_if(command.begin(), command.end(), isspace), command.end());
		std::for_each(command.begin(), command.end(), [](char& c)
			{
				c = ::tolower(c);
			});

		if (command.compare("data") == 0)
		{
			code = false;
			continue;
		}


		//Parancsok, code szekció
		Cim r1, r2;
		string in;

		int param = 0;
		Commands* com = proc.Command(command, param);
		if (param == 0)
		{
			proc.program.push_back(new Processzor::Utasitas(0, 0x00, false, com));
			continue;
		}

		getline(f >> std::ws, in, ',');
		in.erase(remove_if(in.begin(), in.end(), isspace), in.end());
		if (command.front() == 'j')
		{
			if (in.back() == ')' && in.front() == '(')
			{
				//Jump utasításoknál lehet csak
				in.erase(0, 1);
				in.pop_back();
				r1 = StringToReg(in);
				proc.program.push_back(new Processzor::UgroUtasitas(r1, com));
			}
			else if (in.front() < '0' || in.front() > '9')
			{
				proc.program.push_back(new Processzor::UgroUtasitas(in, com));
			}
			else
			{
				in.erase(in.begin(), in.begin() + 1);//# törése
				r2 = std::stoul(in, nullptr, 16);
				proc.program.push_back(new Processzor::Utasitas(0, r2, false, com));
			}
			continue;
		}

		if (command.compare("mov") == 0)
		{
			//r1 is lehet def vagy konstans

			//cimke vagy memóriacím
			if (in.front() == '#')
			{
				//mov #DIGIT_CODE, r8
				in.erase(in.begin(), in.begin() + 1);
				string tag = in;
				getline(f, in);
				in.erase(remove_if(in.begin(), in.end(), isspace), in.end());
				r2 = StringToReg(in);
				com = proc.Command("movm", param);
				proc.program.push_back(new Processzor::AdatUtasitas(tag, 0x00, r2, false, com));

				continue;
			}
			else if (in.front() >= '0' && in.front() <= '9')
			{
				//mov 0x00, r1
				r1 = std::stoul(in, nullptr, 16);
				getline(f, in);
				in.erase(remove_if(in.begin(), in.end(), isspace), in.end());
				r2 = StringToReg(in);
				com = proc.Command("movm", param);
				proc.program.push_back(new Processzor::Utasitas(r2, r1, false, com));
				continue;
			}
			else if (in.back() == ')' && in.front() == '(')
			{
				//mov (r0), r1
				in.erase(0, 1);
				in.pop_back();
				r1 = StringToReg(in);
				getline(f, in);
				in.erase(remove_if(in.begin(), in.end(), isspace), in.end());
				r2 = StringToReg(in);
				com = proc.Command("movm", param);
				proc.program.push_back(new Processzor::AdatUtasitas("", r1, r2, true, com));
				continue;
			}
			else if (in.front() != 'r')
			{
				//mov LD, r1
				r1 = proc.def.at(in);
				getline(f, in);
				in.erase(remove_if(in.begin(), in.end(), isspace), in.end());
				r2 = StringToReg(in);
				com = proc.Command("movm", param);
				proc.program.push_back(new Processzor::Utasitas(r2, r1, false, com));
				continue;
			}
			//mov r1, #0x00
			r1 = StringToReg(in);
			getline(f, in);
			in.erase(remove_if(in.begin(), in.end(), isspace), in.end());
			//TODO: Memóriából olvasás
			if (in.back() == ')' && in.front() == '(')
			{
				//mov r0, (r1)
				in.erase(0, 1);
				in.pop_back();
				r2 = StringToReg(in);
				proc.program.push_back(new Processzor::AdatOlvasUtasitas("", r1, r2, true, com));
				continue;
			}
			else if (in.front() == '#')
			{
				//mov r8, #DIGIT_CODE
				in.erase(in.begin(), in.begin() + 1);
				if (in.front() >= '0' && in.front() <= '9')
				{
					//mov r8, #0x00
					r2 = std::stoul(in, nullptr, 16);
					proc.program.push_back(new Processzor::Utasitas(r1, r2, false, com));
					continue;
				}
				string tag = in;
				proc.program.push_back(new Processzor::AdatOlvasUtasitas(tag, r1, 0x00, false, com));

				continue;
			}
			else if (in.front() >= '0' && in.front() <= '9')
			{
				//mov r1, 0x00
				r2 = std::stoul(in, nullptr, 16);
				proc.program.push_back(new Processzor::AdatOlvasUtasitas("", r1, r2, false, com));
				continue;
			}
			else
			{
				r2 = StringToReg(in);
				if (r2 == 255)
					proc.program.push_back(new Processzor::AdatOlvasUtasitas("", r1, proc.def.at(in), false, com));//mov r1, LD
				else
					proc.program.push_back(new Processzor::Utasitas(r1, r2, true, com));//mov r1, r2
			}

			continue;

		}
		r1 = StringToReg(in);

		if (com->param == 1)
		{
			proc.program.push_back(new Processzor::Utasitas(r1, 0x00, false, com));
			continue;
			//(*com)(r1, 0x00);
		}

		getline(f, in);
		in.erase(remove_if(in.begin(), in.end(), isspace), in.end());
		r2 = StringToReg(in);
		bool reg = r2 != 255;
		if (r2 == 255)
		{
			//Nem lehet pl add r1, LD alakú
			//Feltételezzük hogy 0xHH alakú, két hexadecimális számmal
			in.erase(in.begin(), in.begin() + 1);//# törése
			if (in.front() >= '0' && in.front() <= '9')
			{
				r2 = std::stoul(in, nullptr, 16);
				proc.program.push_back(new Processzor::Utasitas(r1, r2, false, com));
			}
			else
			{
				proc.program.push_back(new Processzor::Utasitas(r1, proc.def.at(in), false, com));
			}
			//(*com)(r1, r2);
		}
		else
		{
			proc.program.push_back(new Processzor::Utasitas(r1, r2, true, com));
			//(*com)(r1, (int)r2);
		}

	}
	thread t1(Init, argc, argv, &proc);
	//Init(argc, argv, &proc);

	while (!proc.End())
	{
		proc();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	t1.join();
}
