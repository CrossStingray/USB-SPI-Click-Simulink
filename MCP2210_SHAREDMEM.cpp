#include <boost/interprocess/managed_shared_memory.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <string>
#include <windows.h>
#include "mcp2210_dll_um.h"

using namespace boost::interprocess;

#define BAUD_RATE 10000000

class MCP2210 {
public:
    void* handle;

    void error_checker(int isOk);
    int spi_write(unsigned char reg, unsigned char tx);
    int spi_read(unsigned char reg, unsigned char* rx);
    
    ~MCP2210() { Mcp2210_Close(this->handle); }
};

void MCP2210::error_checker(int isOk) {
    if (isOk != 0) {
        std::cout << "ERROR: " << isOk << std::endl;
        Mcp2210_Close(handle);
        exit(-1);
    }
}

int MCP2210::spi_write(unsigned char reg, unsigned char tx) {
    int isOk;
    unsigned int pbaudRate = BAUD_RATE;
    unsigned int ptxfersize;

    unsigned char register2send = reg;
    unsigned char data2send = tx;
    unsigned char rx[2];

    isOk = Mcp2210_xferSpiData(handle, &register2send, rx, &pbaudRate, &ptxfersize, 0x10000001);
    isOk = Mcp2210_xferSpiData(handle, &data2send, rx, &pbaudRate, &ptxfersize, 0x10000001);

    return isOk;
}

int MCP2210::spi_read(unsigned char reg, unsigned char* rx) {
    int isOk;
    unsigned int pbaudRate = BAUD_RATE;
    unsigned int ptxfersize;

    unsigned char register2read = reg;

    isOk = Mcp2210_xferSpiData(handle, &register2read, rx, &pbaudRate, &ptxfersize, 0x10000001);

    return isOk;
}

int Init_Accel(MCP2210& SPI_Click) {
    wchar_t version[8];
    wchar_t devPath[128];
    unsigned long devPathsize;
    int len;
    int isOk = 0;
    unsigned short vid = 0x4D8;
    unsigned short pid = 0xDE;
    //SPI Transfer options
    unsigned int pbaudRate = BAUD_RATE;
    unsigned int pidleCsVal = 0x1ff;        // Todos los GPIO son 1 en un estado normal CS8...CS0
    unsigned int pactiveCsVal = 0x1fe;      // CS activo en bajo con CS0 
    unsigned int pcsToDataDly = 0;
    unsigned int pdataToDataDly = 0;
    unsigned int pdataToCsDly = 0;
    unsigned int ptxferSize = 2;            //Bytes a transferir
    unsigned char pspiMd = MCP2210_SPI_MODE0;

    len = Mcp2210_GetLibraryVersion(version);

    if (len > 0) {
        std::wcout << "Version DLL: " << version << std::endl;
    }
    else {
        std::cout << "Error con el DLL!\n" << std::endl;
        return -1;
    }

    int connectedDevices = Mcp2210_GetConnectedDevCount(vid, pid);

    if (connectedDevices > 0) {
        std::cout << "Dispositivos MCP2210 encontrados: " << connectedDevices << std::endl;

        SPI_Click.handle = Mcp2210_OpenByIndex(vid, pid, connectedDevices - 1, devPath, &devPathsize);
        isOk = Mcp2210_GetLastError();

        if (isOk == 0) {
            //Continua codigo...
            std::cout << "Handle obtenido: " << SPI_Click.handle << std::endl << std::endl;

            unsigned short pvid, ppid;
            unsigned char ppwrSrc[8];
            unsigned char prmtWkup[8];
            unsigned short pcurrentLd;

            isOk = Mcp2210_GetUsbKeyParams(SPI_Click.handle, &pvid, &ppid, ppwrSrc, prmtWkup, &pcurrentLd);
            SPI_Click.error_checker(isOk);

            printf("Vendor ID: 0x%x\n", pvid);
            printf("Product ID: 0x%x\n", ppid);
            std::cout << "Maxima corriente por el USB: " << pcurrentLd << "mA" << std::endl;

            wchar_t manufacturerStr[32];
            isOk = Mcp2210_GetManufacturerString(SPI_Click.handle, manufacturerStr);
            SPI_Click.error_checker(isOk);

            std::cout << "String del fabricante: " << manufacturerStr << std::endl;

            wchar_t productStr[32];
            isOk = Mcp2210_GetProductString(SPI_Click.handle, productStr);
            SPI_Click.error_checker(isOk);

            std::cout << "String del producto: " << productStr << std::endl;

            wchar_t serialStr[32];
            isOk = Mcp2210_GetSerialNumber(SPI_Click.handle, serialStr);
            SPI_Click.error_checker(isOk);

            std::cout << "Numero serie: " << serialStr << std::endl;

            // Inicializar configuraci�n de transferencia SPI

            isOk = Mcp2210_SetSpiConfig(SPI_Click.handle, MCP2210_VM_CONFIG, &pbaudRate, &pidleCsVal, &pactiveCsVal, &pcsToDataDly, &pdataToCsDly, &pdataToDataDly, &ptxferSize, &pspiMd);
            SPI_Click.error_checker(isOk);

            unsigned char res[2];
            unsigned char resetReg = 0x6B;
            unsigned char resetTx = 0x80;
            unsigned char disI2CReg = 0x6A;
            unsigned char disI2CTx = 0x10;
            unsigned char MPU9265_SETUP_ACC_SCALE = 28;
            unsigned char MPU9265_SETUP_GYRO_SCALE = 27;
            unsigned char GYRO_FULL_SCALE_2000_DPS = 0x18;
            unsigned char ACC_FULL_SCALE_16_G = 0x18;

            isOk = SPI_Click.spi_write(resetReg, resetTx);
            SPI_Click.error_checker(isOk);

            isOk = SPI_Click.spi_write(disI2CReg, disI2CTx);
            SPI_Click.error_checker(isOk);

            unsigned char whoAmI = (0x75 | (1 << 7));
            // Sets a 1 on the MSB of the reg byte, it indicates a read operation on the MPU9250

            isOk = SPI_Click.spi_read(whoAmI, res);
            SPI_Click.error_checker(isOk);
            printf("Who am I: 0x%x\n", res[1]);

            // Write config Register for accelerometer
            isOk = SPI_Click.spi_write(MPU9265_SETUP_ACC_SCALE, ACC_FULL_SCALE_16_G);
            SPI_Click.error_checker(isOk);

            // Write config register for gyroscope
            isOk = SPI_Click.spi_write(MPU9265_SETUP_GYRO_SCALE, GYRO_FULL_SCALE_2000_DPS);
            SPI_Click.error_checker(isOk);

            // Sets transfer option to 15 bytes per transfer
            ptxferSize = 15;            // 14 registros, el primero toma 2 bytes para empezar a leer

            isOk = Mcp2210_SetSpiConfig(SPI_Click.handle, MCP2210_VM_CONFIG, &pbaudRate, &pidleCsVal, &pactiveCsVal, &pcsToDataDly, &pdataToCsDly, &pdataToDataDly, &ptxferSize, &pspiMd);
            SPI_Click.error_checker(isOk);
        }
        else {
            std::cerr << "Error obteniendo handle. ERROR: " << isOk << std::endl;
            std::cout << std::endl << "Presiona ENTER para salir" << std::endl;
            std::cin.ignore();
            return -1;
        }
    }
    else {
        std::cout << "No se encontro dipositivos MCP2210" << std::endl;
        std::cout << std::endl << "Presiona ENTER para salir" << std::endl;
        std::cin.ignore();
    }

    return isOk;
}

int main()
{
    MCP2210 SPI_Click;

    int isOk = Init_Accel(SPI_Click);

    unsigned int pbaudRate = BAUD_RATE;
    unsigned char Buf[15];
    unsigned int xfer;
    unsigned char register2read[15] = { 0x3B | (1 << 7) };

    // Borra la instancia de la memoria (si existe)
    shared_memory_object::remove("AccelMemory");

    // Crea la instancia en la memoria
    managed_shared_memory segment(open_or_create, "AccelMemory", 1024);
    
    // Aloja el espacio para los 7 valores del acelerometro
    int16_t* data = segment.find_or_construct<int16_t>("AccelData")[7]();

    // Crea un bit de control para detener la aplicacion
    bool* isSimulinkOpen = segment.find_or_construct<bool>("isSimulinkRunning")();

    std::cout << "Memoria compartida abierta exitosamente" << std::endl << std::endl;
    std::cout << "\t\t\t\tENVIANDO DATOS A SIMULINK" << std::endl;

    printf("\nPresiona ESC para salir!\n ---------------------------------------------------------------------------------------\n");
    Sleep(1000);

    bool is_ESC_Key_Pressed = false;
    *isSimulinkOpen = true;
    while ( !is_ESC_Key_Pressed && *isSimulinkOpen ) {

        if (GetAsyncKeyState(VK_ESCAPE)) {
            is_ESC_Key_Pressed = true;
        }

        isOk = Mcp2210_xferSpiData(SPI_Click.handle, register2read, Buf, &pbaudRate, &xfer, 0x10000001);
        SPI_Click.error_checker(isOk);

        // Convert accelerometer registers
        int16_t ax = Buf[1] << 8 | Buf[2];
        int16_t ay = Buf[3] << 8 | Buf[4];
        int16_t az = Buf[5] << 8 | Buf[6];

        int16_t temp = Buf[7] << 8 | Buf[8];
        // Convert gyroscope registers
        int16_t gx = Buf[9] << 8 | Buf[10];
        int16_t gy = Buf[11] << 8 | Buf[12];
        int16_t gz = Buf[13] << 8 | Buf[14];

        //printf("\33[?25l");  // Hides cursor
        printf("\rax = %-6d, ay = %-6d, az = %-6d\tgx = %-6d, gy = %-6d, gz = %-6d\t t = %-6d",
            ax, ay, az, gx, gy, gz, temp);
        //printf("\x1b[A");   // Clear current line

        data[0] = ax;
        data[1] = ay;
        data[2] = az;
        data[3] = gx;
        data[4] = gy;
        data[5] = gz;
        data[6] = temp;

    }
    Mcp2210_Close(SPI_Click.handle);

    return 0;
}
