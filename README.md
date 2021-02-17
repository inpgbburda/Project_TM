# Project of meteorological station

Hardware used for the project:
 - Stm 32 microcontroler
 - Kamami shield for stm 32

![Hardware and wiring](/images/Wiring.png)

Full functionality of the station:
 - measurement of tempereture, pressure and light intensity
 - ability to display live all values on 4 x 7 segment display
 - controll using buttons built in to kamami and stm 32
 - Transsmision to PC using UART protocol

Image below shows displayed tempereture with accuracy to 0,1 deg

![Displayed tempereture](/images/Temp.png)


UART comunication is working through COM ports. Data is sent in frames, using whitespace as seperators

![UART frames](/images/UART.png)

Data is collectd using Matlab script and displayed as shown below

![MAtlab script](/images/Data.png)
