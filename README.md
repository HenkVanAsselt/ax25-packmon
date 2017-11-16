# ax25-packmon

This is a Radio Amateur (HAM) project from 1994, written in Assembler and C.

It was intended to work with a Baycom modem, which would use a TCM3105 chip, and convert AX.25 audio to 
signal changes of the DTR and CTS lines of a serial interface.

Software running on a PC was monitoring these signal changes and would eventually create human readable text.

This project made use of HELPPC (see http://docs.huihoo.com/help-pc) to create a helpfiles.
It already made use of 4DOS in that time, and suprise, suprise, the 4DOS command files (with the extension .BTM) do still work in 2017,
but now using the free TCC/LE CMD replacement (see https://jpsoft.com/products/tcc-le.html)

Have fun,
Henk