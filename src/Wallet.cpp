#include "Wallet.h"
#include "MeshComm.h"

Wallet::Wallet(const Device &localDevice, int localModuleId, int pinGPIO) : Module(localDevice,localModuleId), PIN_GPIO(pinGPIO)
{
    // Interrupt connected  to PIN D2 executing IncomingImpuls function when signal goes from HIGH to LOW
    if(PIN_GPIO>0){
        attachInterruptArg(this->PIN_GPIO, addMoneyWrapper, this, FALLING);
    }
}

void Wallet::AddMoney(int value)
{
    amount = amount + value;
    timeOut.start(1000);
}