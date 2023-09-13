# USB SPI Click Simulink
A USB SPI Click board is used to communicate with a MPU9250 Accelerometer. A C++ console program is used to handle the MCP2210 device and constantly read the 14 registers of the accelerometer then, boost managed shared memory is used to share the read data into a custom S-Function block in a desktop real time simulation in Simulink. The received data is constantly plotted with a scope.
