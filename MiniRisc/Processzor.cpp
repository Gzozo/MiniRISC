#include "Processzor.h"

int StringToReg(std::string s)
{
	if (s[0] == 'r')
	{
		if (s[2] >= '0' && s[2] <= '9')
			return (s[1] - '0') * 10 + s[2] - '0';
		else
			return s[1] - '0';
	}
	return -1;
}

void Commands::operator()(int r1, int r2)
{
	(*this)(r1, cpu->Reg(r2));
}

void Commands::Flags(int regiszter)
{
	cpu->Z = cpu->Reg(regiszter) == 0;
	cpu->N = cpu->Reg(regiszter) >> 7 == 1;
}

Cim Processzor::ReadAdat(Cim addr)
{
	for (size_t i = 0; i < modulok.size(); i++)
	{
		if (modulok[i]->modul->kezdo == addr || modulok[i]->modul->kezdo < addr && modulok[i]->modul->kezdo + modulok[i]->modul->hossz > addr)
		{
			return modulok[i]->modul->Read(addr);
		}
	}
}

void Processzor::WriteAdat(Cim addr, Cim ertek)
{
	for (size_t i = 0; i < modulok.size(); i++)
	{
		if (modulok[i]->modul->kezdo == addr || modulok[i]->modul->kezdo < addr && modulok[i]->modul->kezdo + modulok[i]->modul->hossz > addr)
		{
			modulok[i]->modul->Write(addr, ertek);
			return;
		}
	}
}
