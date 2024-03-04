#ifndef TESTCHANDELIERMODULECOMMAND_H
#define TESTCHANDELIERMODULECOMMAND_H

#include "MyCommand.h"

using namespace std;

class TestChandelierModuleCommand : public MyCommand
{

public:
    TestChandelierModuleCommand(uint32_t deviceId, int moduleId, int chandelierModuleId): MyCommand(deviceId,moduleId), ChandelierModuleID(chandelierModuleId){

    }
    const int ChandelierModuleID;

private:
};

#endif //TESTCHANDELIERMODULECOMMAND_H