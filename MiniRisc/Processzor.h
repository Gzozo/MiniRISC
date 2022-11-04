#include <string>
#include <map>
#include <stdexcept>
#include <vector>
#include <stack>
#include "Const.h"
#include "Moduls.h"

#ifndef PROCI
#define PROCI




using namespace std;


struct Commands
{
	int param;
	Processzor* cpu;
	Commands(int p, Processzor* cpu) :param(p), cpu(cpu) {}
	virtual void operator()(int, Cim) = 0;
	virtual void operator()(int r1, int r2);
	virtual void Flags(int regiszter);
};
class Processzor
{
	public:
	struct FPGAModul
	{
		Moduls* modul;
		size_t x, y;
		FPGAModul(Moduls* modul, size_t x, size_t y) :modul(modul), x(x), y(y) {}
		void Draw()
		{
			modul->Draw(x, y);
		}
		~FPGAModul()
		{
			delete modul;
		}
	};
	bool IF, IE, V, N, C, Z;
	vector <FPGAModul*> modulok;
	vector <Clickable*> hitbox;
	private:
	Cim reg[16];
	Cim adat[256];
	Cim adat_cnt = 0x00;
	int utasitas = 0;

	private:
	#pragma region Parancsok


	struct Add : Commands
	{
		bool carry = false;
		Add(Processzor* cpu) :Commands(2, cpu) {}
		virtual void operator()(int regiszter, Cim ertek)
		{
			Cim temp = cpu->reg[regiszter];
			cpu->reg[regiszter] += ertek;
			if (carry && cpu->C)
			{
				cpu->reg[regiszter]++;
				cpu->C = (int)temp + (int)ertek + 1 >= 256;
			}
			else
				cpu->C = (int)temp + (int)ertek >= 256;
			Flags(regiszter);
			cpu->V = temp >> 7 == ertek >> 7 && temp >> 7 != cpu->reg[regiszter] >> 7;
		}
	};
	struct Adc : Add
	{
		Adc(Processzor* cpu) :Add(cpu) { carry = true; }
	};
	struct Mov : Commands
	{
		Mov(Processzor* cpu) :Commands(2, cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			cpu->Reg(regiszter) = ertek;
		}
	};
	struct MovM : Commands
	{
		MovM(Processzor* cpu) :Commands(2, cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			//Itt fordítva!!!
			//cpu->adat[ertek] = cpu->Reg(regiszter);
			cpu->WriteAdat(ertek, cpu->Reg(regiszter));
		}
	};
	struct Sub : Add
	{
		Sub(Processzor* cpu) :Add(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			Cim temp = cpu->Reg(regiszter);
			Add::operator()(regiszter, -ertek - 2 * (int)carry);
			cpu->C = (int)temp - (int)ertek < 0;
		}
	};
	struct Sbc : Sub
	{
		Sbc(Processzor* cpu) :Sub(cpu) { carry = true; }
	};
	struct Cmp : Sub
	{
		Cmp(Processzor* cpu) :Sub(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			Cim temp = cpu->Reg(regiszter);
			Sub::operator()(regiszter, ertek);
			cpu->Reg(regiszter) = temp;
		}
	};
	struct And : Commands
	{
		And(Processzor* cpu) :Commands(2, cpu) {}
		virtual void operator()(int regiszter, Cim ertek)
		{
			cpu->Reg(regiszter) &= ertek;
			Flags(regiszter);
		}
	};
	struct Or : Commands
	{
		Or(Processzor* cpu) :Commands(2, cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			cpu->Reg(regiszter) |= ertek;
			Flags(regiszter);
		}
	};
	struct Xor : Commands
	{
		Xor(Processzor* cpu) :Commands(2, cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			cpu->Reg(regiszter) ^= ertek;
			Flags(regiszter);
		}
	};
	struct Tst : Commands
	{
		Tst(Processzor* cpu) :Commands(2, cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			Cim temp = cpu->Reg(regiszter);
			cpu->Reg(regiszter) &= ertek;
			Flags(regiszter);
			cpu->Reg(regiszter) = temp;
		}
	};
	struct Swp : Commands
	{
		Swp(Processzor* cpu) :Commands(2, cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			Cim felso = cpu->Reg(regiszter) & 0xf0;
			felso = felso >> 4;
			cpu->Reg(regiszter) = ((cpu->Reg(regiszter) & 0x0f) << 4) | felso;
			Flags(regiszter);
		}
	};
	struct Sl0 : Commands
	{
		Cim bejon = 0x00;
		Sl0(Processzor* cpu) :Commands(1, cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			cpu->C = (cpu->Reg(regiszter) >> 7) == 1;
			cpu->Reg(regiszter) = cpu->Reg(regiszter) << 1;
			cpu->Reg(regiszter) |= bejon;
			Flags(regiszter);
		}
	};
	struct Sl1 :Sl0
	{
		Sl1(Processzor* cpu) :Sl0(cpu) { bejon = 0x01; }

	};
	struct Sr0 : Commands
	{
		Cim bejon = 0x00;
		Sr0(Processzor* cpu) :Commands(1, cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			cpu->C = (cpu->Reg(regiszter) & 0x01) == 1;
			cpu->Reg(regiszter) = cpu->Reg(regiszter) >> 1;
			cpu->Reg(regiszter) |= (bejon << 7);
			Flags(regiszter);
		}
	};
	struct Sr1 :Sr0
	{
		Sr1(Processzor* cpu) :Sr0(cpu) { bejon = 0x01; }

	};
	struct Asr : Sr0
	{
		Asr(Processzor* cpu) :Sr0(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			bejon = cpu->Reg(regiszter) >> 7;
			Sr0::operator()(regiszter, ertek);
		}
	};
	struct Rol : Sl0
	{
		Rol(Processzor* cpu) :Sl0(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			bejon = cpu->Reg(regiszter) >> 7;
			Sl0::operator()(regiszter, ertek);
		}
	};
	struct Ror : Sr0
	{
		Ror(Processzor* cpu) :Sr0(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			bejon = cpu->Reg(regiszter) & 0x01;
			Sr0::operator()(regiszter, ertek);
		}
	};
	struct Rlc : Sl0
	{
		Rlc(Processzor* cpu) :Sl0(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			bejon = (int)cpu->C;
			Sl0::operator()(regiszter, ertek);
		}
	};
	struct Rrc : Sr0
	{
		Rrc(Processzor* cpu) :Sr0(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			bejon = (int)cpu->C;
			Sr0::operator()(regiszter, ertek);
		}
	};
	struct Jmp :Commands
	{
		Jmp(Processzor* cpu) :Commands(1, cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			cpu->cnt = ertek - 1;
		}
	};
	struct Jz :Jmp
	{
		Jz(Processzor* cpu) :Jmp(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			if (cpu->Z)
				Jmp::operator()(regiszter, ertek);
		}
	};
	struct Jnz :Jmp
	{
		Jnz(Processzor* cpu) :Jmp(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			if (!cpu->Z)
				Jmp::operator()(regiszter, ertek);
		}
	};
	struct Jc :Jmp
	{
		Jc(Processzor* cpu) :Jmp(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			if (cpu->C)
				Jmp::operator()(regiszter, ertek);
		}
	};
	struct Jnc :Jmp
	{
		Jnc(Processzor* cpu) :Jmp(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			if (!cpu->C)
				Jmp::operator()(regiszter, ertek);
		}
	};
	struct Jn :Jmp
	{
		Jn(Processzor* cpu) :Jmp(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			if (cpu->N)
				Jmp::operator()(regiszter, ertek);
		}
	};
	struct Jnn :Jmp
	{
		Jnn(Processzor* cpu) :Jmp(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			if (!cpu->N)
				Jmp::operator()(regiszter, ertek);
		}
	};
	struct Jv :Jmp
	{
		Jv(Processzor* cpu) :Jmp(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			if (cpu->V)
				Jmp::operator()(regiszter, ertek);
		}
	};
	struct Jnv :Jmp
	{
		Jnv(Processzor* cpu) :Jmp(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			if (!cpu->V)
				Jmp::operator()(regiszter, ertek);
		}
	};
	struct Jsr : Jmp
	{
		Jsr(Processzor* cpu) :Jmp(cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			cpu->SaveStack();
			Jmp::operator()(regiszter, ertek);
		}
	};
	struct Rts :Commands
	{
		Rts(Processzor* cpu) :Commands(0, cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			cpu->LoadStack();
		}
	};
	struct Sti :Commands
	{
		Sti(Processzor* cpu) :Commands(0, cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			cpu->IE = true;
		}
	};
	struct Cli :Commands
	{
		Cli(Processzor* cpu) :Commands(0, cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			cpu->IE = false;
		}
	};
	struct Rti :Commands
	{
		Rti(Processzor* cpu) :Commands(0, cpu) {}
		void operator()(int regiszter, Cim ertek)
		{
			cpu->LoadStack(true);
		}
	};

	#pragma endregion



	public:

	struct CpuState
	{
		Cim cnt;
		bool IF, IE, V, N, C, Z;
		CpuState(Cim cnt, bool IF, bool IE, bool V, bool N, bool C, bool Z) :cnt(cnt), IF(IF), IE(IE), V(V), N(N), C(C), Z(Z) {}
	};
	CpuState getState()
	{
		return CpuState(cnt, IF, IE, V, N, C, Z);
	}

	stack<CpuState> verem;

	Cim* AdatP(Cim eltol = 0x00)
	{
		return adat + eltol;
	}

	void SaveStack()
	{
		verem.push(getState());
	}
	void LoadStack(bool flags = false)
	{
		CpuState load = verem.top();
		verem.pop();
		cnt = load.cnt;
		if (flags)
		{
			IF = load.IF;
			IE = load.IE;
			V = load.V;
			N = load.N;
			C = load.C;
			Z = load.Z;
		}
	}

	struct Utasitas
	{
		int reg1;
		Cim param2;
		Commands* com;
		bool reg;
		Utasitas(int r1, Cim p2, bool reg, Commands* com) :reg1(r1), param2(p2), reg(reg), com(com) {}
		virtual void Execute()
		{
			if (reg)
				(*com)(reg1, (int)param2);
			else
				(*com)(reg1, param2);
		}
		virtual void operator()()
		{
			Execute();
		}
	};
	struct UgroUtasitas : Utasitas
	{
		string cim = "";
		UgroUtasitas(string cim, Commands* com) :cim(cim), Utasitas(0, 0x00, false, com) {}
		UgroUtasitas(Cim cim, Commands* com, bool reg = true) :Utasitas(0, cim, reg, com) {}
		void Execute()
		{
			if (!reg && cim != "")
				param2 = com->cpu->def.at(cim);
			Utasitas::Execute();
		}
		void operator()()
		{
			Execute();
		}
	};
	struct AdatUtasitas :Utasitas
	{
		string cim;
		bool regisz;
		AdatUtasitas(string cim, Cim addr, int param2, bool reg, Commands* com) :cim(cim), regisz(reg), Utasitas(param2, addr, false, com) {}
		virtual void Execute()
		{
			Cim param = param2;
			if (regisz)
				param2 = com->cpu->Reg(param2);
			else
				param2 = com->cpu->def.at(cim);
			Utasitas::Execute();
			param2 = param;
		}
	};
	struct AdatOlvasUtasitas :AdatUtasitas
	{
		AdatOlvasUtasitas(string cim, Cim addr, int param2, bool reg, Commands* com) :AdatUtasitas(cim, param2, addr, reg, com) {}
		virtual void Execute()
		{
			Cim param = param2;
			if (regisz)
				param2 = com->cpu->Reg(param2);
			else if (!cim.empty())
			{
				param2 = com->cpu->def.at(cim);
				Utasitas::Execute();
				return;
			}
			param2 = com->cpu->ReadAdat(param2);
			Utasitas::Execute();
			param2 = param;
		}
	};

	vector<Utasitas*> program;
	int cnt = 0;

	map<string, size_t>def;
	void AddUgras(string name)
	{
		def[name] = program.size();
	}
	void AddAdatDef(string name)
	{
		def[name] = (size_t)adat_cnt;
	}
	void AddDef(string name, Cim ertek)
	{
		def[name] = ertek;
	}
	void AddData(Cim data)
	{
		adat[adat_cnt++] = data;
	}
	void SetDataCnt(Cim set)
	{
		adat_cnt = set;
	}

	void FPGASetup()
	{
		modulok.push_back(new FPGAModul(new Regiszter(this), 20, 200));
		modulok.push_back(new FPGAModul(new Memoria(this, 0x00, 0x80), 400, 200));
		modulok.push_back(new FPGAModul(new Led(this, 0x80), 550, 150));
		modulok.push_back(new FPGAModul(new Seg7(this, 0x90, 4), 130, 80));
		modulok.push_back(new FPGAModul(new Matrix(this, 0x94, 5, 7), 20, 80));
		Buttons* bt = new Buttons(this, 0x84, 4);
		modulok.push_back(new FPGAModul(bt, 80, 40));
		hitbox.push_back(bt);
		Switch* sw = new Switch(this, 0x81, 8);
		modulok.push_back(new FPGAModul(sw, 400, 20));
		hitbox.push_back(sw);
	}

	Processzor()
	{
		for (int i = 0; i < 16; i++)
			reg[i] = 0;
		for (int i = 0; i < 256; i++)
			adat[i] = 0;
		Z = C = N = V = IE = IF = false;
		FPGASetup();
	}


	unsigned char& Reg(int i)
	{
		if (i < 16)
			return reg[i];
		throw "error";
	}
	Cim ReadAdat(Cim addr);
	void WriteAdat(Cim addr, Cim ertek);



	map<string, Commands*> parancs = {
		{"add", new Add(this)},
		{"adc", new Adc(this)},
		{"sub", new Sub(this)},
		{"sbc", new Sbc(this)},
		{"mov", new Mov(this)},
		{"movm", new MovM(this)},
		{"cmp", new Cmp(this)},
		{"and", new And(this)},
		{"or",  new Or(this)},
		{"xor", new Xor(this)},
		{"tst", new Tst(this)},
		{"swp", new Swp(this)},
		{"sl0", new Sl0(this)},
		{"sl1", new Sl1(this)},
		{"sr0", new Sr0(this)},
		{"sr1", new Sr1(this)},
		{"asr", new Asr(this)},
		{"rol", new Rol(this)},
		{"ror", new Ror(this)},
		{"rlc", new Rlc(this)},
		{"rrc", new Rrc(this)},
		{"jmp", new Jmp(this)},
		{"jz", new Jz(this)},
		{"jnz", new Jnz(this)},
		{"jc", new Jc(this)},
		{"jnc", new Jnc(this)},
		{"jn", new Jn(this)},
		{"jnn", new Jnn(this)},
		{"jv", new Jv(this)},
		{"jnv", new Jnv(this)},
		{"jsr", new Jsr(this)},
		{"rts", new Rts(this)},
		{"sti", new Sti(this)},
		{"cli", new Cli(this)},
		{"rti", new Rti(this)},
	};
	Commands* Command(string in, int& param)
	{
		param = parancs.at(in)->param;
		return parancs.at(in);
	}
	bool operator()()
	{
		if (End())
			return false;
		(*program[cnt])();
		cnt++;
		utasitas++;
		return true;
	}
	bool End()
	{
		return cnt == program.size();
	}
	std::vector<string> RegToString()
	{
		std::vector<string> ret;
		for (int i = 0; i < 16; i++)
		{
			string s = "R" + to_string(i);
			s += ": " + to_string((int)reg[i]) + "\n";
			ret.push_back(s);
		}
		return ret;
	}
	std::vector<Cim> RegToVector()
	{
		std::vector<Cim> ret;
		for (int i = 0; i < 16; i++)
		{
			ret.push_back(reg[i]);
		}
		return ret;
	}



	~Processzor()
	{
		for (auto it = parancs.begin(); it != parancs.end(); it++)
			delete it->second;
		for (auto it = program.begin(); it != program.end(); it++)
			delete* it;
		for (auto it = modulok.begin(); it != modulok.end(); it++)
			delete* it;
	}
};

int StringToReg(std::string s);


#endif
