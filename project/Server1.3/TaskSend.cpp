#include "TaskSend.h"

TaskSend::TaskSend(Client* pClient, DataHeader* header) :_pClient(pClient), _header(header)
{
}

void TaskSend::doTask()
{
	_pClient->SendData(_header);
	//std::cout << "��������+1" << std::endl;
	delete _header;
}
