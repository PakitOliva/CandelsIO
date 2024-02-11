#ifndef WALLET_H
#define WALLET_H

#include "Timeout.h"
#include "Module.h"

class Wallet : public Module
{

public:
    String Name(){return "Wallet";}
    static String GetName(){return "Wallet";}

    int const PIN_GPIO;
    Timeout timeOut;

    Wallet(const Device &localDevice, int localModuleId, int pinGPIO);

    void AddMoney(int value = 1);

    bool isReady()
    {
        if (amount <= 0)
            return false;
        return timeOut.time_over();
    }
    int GetAndClean()
    {
        int aux = amount;
        amount = 0;
        return aux;
    }

    std::function<void()> addMoneyLambda = [this]() -> void
    {
        this->AddMoney();
    };

    String UrlStatus() override
    {
         String resultado = "/wallet.html?id=" + LocalDeviceID;
        resultado.concat("&num=" + LocalModuleID);
        return resultado;
    }

private:
    int amount = 0;

    // Función global que llamará al método de la clase
    static void addMoneyWrapper(void *walletPtr)
    {
        Wallet *wallet = static_cast<Wallet *>(walletPtr);
        if (wallet)
        {
            wallet->AddMoney();
        }
    }
};

#endif