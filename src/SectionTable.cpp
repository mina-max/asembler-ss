#include "SectionTable.h"

bool SectionTable::existSymbol(string name)
{
	for (std::vector<Section*>::iterator i = table.begin(); i != table.end(); ++i)
	{
		if ((*i)->name == name)
			return true;
	}
	return false;
}

void SectionTable::addSection(Section* s)
{
	table.push_back(s);
}
