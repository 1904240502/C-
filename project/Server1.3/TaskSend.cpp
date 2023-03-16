#include "TaskSend.h"

TaskSend::TaskSend(Client* pClient, DataHeader* header) :_pClient(pClient), _header(header)
{
}

void TaskSend::doTask()
{
	_pClient->SendData(_header);
	delete _header;
}
