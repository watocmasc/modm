#include <iostream>
#include <cstdlib>
#include <modbus/modbus.h>

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
    // modm -> MODbus Master
    // func: read / write
    // write: modm write device id baud parity data stop addr value 
    // read:  modm read  device id baud parity data stop addr_start count_addr size_arr
    if ( argc == 10 ) {
        
        // modbus_t* ctx = modbusInit ( "/dev/cu.usbserial-FTB6SPL3", 1, 9600, 'N', 8, 1 );
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

    } else {
        std::cout << "Error";
    }

    return 0;
}
