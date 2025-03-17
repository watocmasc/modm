////////////////////////////////
// modm -> MODbus Master      //
//////////////////////////////// 
//█▄─▀█▀─▄█─▄▄─█▄─▄▄▀█▄─▀█▀─▄█//
//██─█▄█─██─██─██─██─██─█▄█─██//
//▀▄▄▄▀▄▄▄▀▄▄▄▄▀▄▄▄▄▀▀▄▄▄▀▄▄▄▀//
////////////////////////////////

#include <cstdlib>
#include <modbus/modbus.h>
#include <set>
#include <iostream>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <glob.h>
#endif

std::vector<std::string> listSerialPorts() {
    std::set<std::string> uniquePorts;  // Используем set, чтобы избежать дубликатов

#ifdef _WIN32
    // Windows: проверяем COM1-COM255
    for (int i = 1; i <= 255; ++i) {
        std::string portName = "COM" + std::to_string(i);
        std::string fullPortName = "\\\\.\\" + portName;

        HANDLE hSerial = CreateFileA(fullPortName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        if (hSerial != INVALID_HANDLE_VALUE) {
            uniquePorts.insert(portName);  // Добавляем только уникальные порты
            CloseHandle(hSerial);
        }
    }
#else
    // macOS и Linux: используем glob() для поиска портов
    glob_t glob_result;
    const char* patterns[] = {
    #ifdef __APPLE__
        "/dev/tty.*", "/dev/cu.*"
    #else
        "/dev/ttyS*", "/dev/ttyUSB*", "/dev/ttyACM*"
    #endif
    };

    for (const char* pattern : patterns) {
        if (glob(pattern, GLOB_NOSORT, nullptr, &glob_result) == 0) {
            for (size_t i = 0; i < glob_result.gl_pathc; ++i) {
                uniquePorts.insert(glob_result.gl_pathv[i]);  // Добавляем в set, исключая дубликаты
            }
        }
    }
    globfree(&glob_result);
#endif

    return std::vector<std::string>(uniquePorts.begin(), uniquePorts.end());
}

modbus_t*
modbusInit ( const char *port, uint16_t id, int baud, char parity, uint16_t data, uint16_t stop ) {
    
    modbus_t *ctx = modbus_new_rtu(port, baud, parity, data, stop);

    if ( ctx == nullptr ) { 

        std::cerr << "Error: not complete create context of modbus\n";
    }

    if ( modbus_connect ( ctx ) == -1 ) {

        std::cerr << "Error of connection: " << modbus_strerror ( errno ) << std::endl; 
        modbus_free ( ctx );
    }

    if ( modbus_set_slave ( ctx, id ) == -1 ) {

        std::cerr << "Error of slave ID: " << modbus_strerror ( errno ) << std::endl;
        modbus_close ( ctx );
        modbus_free  ( ctx );
    }

    return ctx;
}

int
modbusWriteRegister ( modbus_t *ctx, uint16_t addr, int value ) {
    int rc = modbus_write_register ( ctx, addr, value );

    if ( rc == -1 ) {

        std::cout << "Error write register: " << modbus_strerror ( errno ) << std::endl;
        return -1;
    } else {

        std::cout << "Complete!";
    }

    modbus_close ( ctx );
    modbus_free  ( ctx );

    return 0;
}

int
modbusReadRegister ( modbus_t *ctx, uint16_t startRegister, uint16_t countRegisters, uint16_t arrSize ) {

    uint16_t registers [ countRegisters ];

    int rc = modbus_read_registers(ctx, startRegister, countRegisters, registers);
    if (rc == -1) {
        std::cerr << "Error reading of registers: " << modbus_strerror(errno) << std::endl;
        return -1;
    } else {
        std::cout << "Прочитано " << rc << " регистров:\n";
        for (int i = 0; i < rc; i++) {
            std::cout << "Регистр[" << i << "] = " << registers[i] << std::endl;
        }
    }

    modbus_close ( ctx );
    modbus_free  ( ctx );

    return 0;
}

int 
main ( int argc, char *argv[] ) {
    if ( argc == 10 ) {
        
        modbus_t* ctx = modbusInit ( argv[2], 1, 9600, 'N', 8, 1 );
        if ( std::string(argv[1]) == "write" ) {

            modbus_t* ctx = modbusInit ( argv[2], 
                                         std::atoi(argv[3]), 
                                         std::atoi(argv[4]), 
                                         *(argv[5]), 
                                         std::atoi ( argv[6] ),
                                         std::atoi ( argv[7] ) 
                                        );

            modbusWriteRegister ( ctx, std::atoi(argv[8]), std::atoi(argv[9]) );
        }

    } else if ( argc == 11 ) {
        if ( std::string(argv [1]) == "read" ) {
            modbus_t* ctx = modbusInit ( argv[2], 
                                        std::atoi(argv[3]), 
                                        std::atoi(argv[4]), 
                                        *(argv[5]), 
                                        std::atoi ( argv[6] ),
                                        std::atoi ( argv[7] ) 
                                    );

            modbusReadRegister ( ctx, std::atoi(argv[8]), std::atoi(argv[9]), std::atoi ( argv[10] ) );
        }   

    } else if ( argc == 2 && std::string ( argv[1] ) == "devices" ) {
        std::vector<std::string> ports = listSerialPorts ();

        if ( ports.empty () ) {
            std::cout << "COM-ports not found\n";
        } else {
            std::cout << "Found COM-ports:\n";
            for ( const auto &port : ports ) {
                std::cout << port << std::endl;
            }
        }
    } else {
        std::cout 
        << "\n[ write ] modm write device id baud parity data stop addr value"
        << "\n[ read  ] modm read  device id baud parity data stop addr_start count_addr size_arr\n";
    }

    return 0;
}
