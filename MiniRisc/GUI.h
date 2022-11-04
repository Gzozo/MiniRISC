#ifndef GUI_H
#define GUI_H

class Clickable
{
	public:
	//
	virtual void Callback() {};
	virtual bool Detection(size_t x, size_t y) = 0;
};

#endif // GUI_H
