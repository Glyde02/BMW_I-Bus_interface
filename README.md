# BMW_I-Bus_interface

# I-Bus interface
____

- :white_check_mark: create a data bus communication with a laptop via a usb connection
- :white_check_mark: read and analyze the I-BUS data bus
- :white_check_mark: attempt to write to the I-BUS
- :white_check_mark: create a circuit board to communicate with the data bus
- :black_square_button: change the connection scheme in order to increase stability
- :black_square_button: create a WinAPI application, read data from a virtual COM port
- :black_square_button: add functions to receive data from the car
- :black_square_button: add functions to send data to the car
- :black_square_button: add technical documentation to the application about the interconnected nodes of the car
____

### Сonnection of nodes in **BMW** E-series

In BMW, the tire is used to establish communication between safety equipment, for engine control or
for entertainment devices (the famous IBus).

![I-Bus](https://i.ibb.co/GFCjKjM/ibus.jpg "I-Bus")

The I-Bus is a single wire bus. That means it uses only one wire to send and receive data.
In the car you can find the I-BUS wire at the CD changer connector in the rear, the Navigation system CD-ROM
unit connector, the phone connector in the center console, etc.
____

# Сircuit board

For communication, a printed circuit on two transistors was assembled. The scheme does not allow you to physically solve the problem with collisions. This should be done programmatically. However, the stability of the work is low. Therefore, it is better to use this scheme only for reading data from the bus.

The components of the printed circuit board are shown in the picture.

![Circuit](https://i.ibb.co/SVHnbNq/f44028328238.png "Curcuit")

For correct operation, a circuit based on the TH3122.4 chip will be assembled.
____

# To be continued.
