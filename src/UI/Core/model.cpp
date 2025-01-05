#include "model.h"
#include "presenter.h"

#define NOTIFY_ALL_PRESENTERS(func)                                                                                    \
	for (auto presenter : m_presenters)                                                                                \
	{                                                                                                                  \
		presenter->func();                                                                                             \
	}

void Model::newHeaterData()
{
	NOTIFY_ALL_PRESENTERS(newHeaterData);
}
