#ifndef MODULS_H
#define MODULS_H

#include "Const.h"
#include "GUI.h"

class Moduls
{
	public:
	Processzor* cpu;
	Cim kezdo, hossz; //hossz talán integer?
	Moduls(Processzor* cpu, Cim kezdo, Cim hossz) :cpu(cpu), kezdo(kezdo), hossz(hossz) {}
	virtual void Callback() {} //Lehet nem kell
	virtual void Write(Cim addr, Cim ertek);
	virtual Cim Read(Cim addr);
	virtual void Draw(size_t x, size_t y) {}
	virtual ~Moduls() {}
};
class Regiszter :public Moduls
{
	public:
	Regiszter(Processzor* cpu) : Moduls(cpu, 0, 0) {}
	virtual void Draw(size_t x, size_t y);
};
class Memoria : public Moduls
{
	//Hogy valósítsam meg?
	//Felesleges ezeket kezelni, csak grafikánál talán fontos hogy külön legyen
	//De MiniRisc buszai mindig ugyanazok, szóval felesleges bővíthetővé csinálni
	//Esetleg csak a grafikai Draw fv legyen benne, ne Action?
	//Először legyen meg a logika, aztán a grafika
	public:
	Memoria(Processzor* cpu, Cim kezdo, Cim hossz) :Moduls(cpu, kezdo, hossz) {}
	virtual void Draw(size_t x, size_t y);
};
class Led :public Moduls
{
	public:
	Led(Processzor* cpu, Cim kezdo) :Moduls(cpu, kezdo, 1) {}
	virtual void Draw(size_t x, size_t y);
};
class Seg7 :public Moduls
{
	size_t db;
	public:
	Seg7(Processzor* cpu, Cim kezdo, Cim hossz) :db(hossz), Moduls(cpu, kezdo, hossz) {}
	virtual void Draw(size_t x, size_t y);
};
class Matrix :public Moduls
{
	size_t db;
	public:
	Matrix(Processzor* cpu, Cim kezdo, Cim hossz, size_t db) :db(db), Moduls(cpu, kezdo, hossz) {}
	virtual void Draw(size_t x, size_t y);
};
class Buttons :public Moduls, public Clickable
{
	Cim buttons;
	int db;
	int tavolsag = 45, radius = 15;
	size_t sx = 0, sy = 0;
	//int previous = -1; //előző gomb, vector, hogy többet le lehessen nyomni egyszerre
	public:
	Buttons(Processzor* cpu, Cim kezdo, int db) :db(db), Moduls(cpu, kezdo, 1), Clickable()
	{
		buttons = 0x00;
	}
	virtual void Draw(size_t x, size_t y);
	virtual void Write(Cim addr, Cim ertek) {}
	virtual void Callback();
	virtual void Action(int bt);
	virtual bool Detection(size_t x, size_t y);
};
class Switch :public Moduls, public Clickable
{
	Cim sw;
	int db;
	int mag = 50, szel = 20, tav = 30;
	size_t sx = 0, sy = 0;
	public:
	Switch(Processzor* cpu, Cim kezdo, int db) :db(db), Moduls(cpu, kezdo, 1), Clickable()
	{
		sw = 0x00;
	}
	virtual void Draw(size_t x, size_t y);
	virtual void Write(Cim addr, Cim ertek) {}
	virtual void Action(int bt);
	virtual bool Detection(size_t x, size_t y);
};

#endif