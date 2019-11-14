let MemoryMap = require('nrf-intel-hex');
let memoryMap = new MemoryMap();

uicr_regout0_address = 0x10001304;                            // Location of UICR.REGOUT0
uicr_regout0_value   = Uint8Array.of(0xFD, 0xFF, 0xFF, 0xFF); // 0xFFFFFFFD -> "5" -> 3.3V

memoryMap.set(uicr_regout0_address, uicr_regout0_value);

let hexString = memoryMap.asHexString();
process.stdout.write(hexString);
