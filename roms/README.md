# Eight Ball ROM Info

The files "my-rom-U2.bin" and "my-rom-U6.bin" represent the contents of the ROMs from my Bally Eight Ball machine.

## Power Issues
To read the ROMs, I used a Pi Pico wired to an EPROM socket.
Note that the old ROMs are spec'd for a 5V supply, but the Pico is a 3.3V device.
I found that if I ran a separate wire to the ROM power pin and set the power supply to 3.5V, they would read consistently.

__Warning: Do not exceed 3.6V to the ROM power pin or else the Pico will be destroyed!__

## ROM Contents

The files "my_rom_U*.bin" contain the data that I read from my ROMs.

ROM files can also be found elsewhere on the web.
There are two varieties: 'with-tilt-bug' and 'tilt-bug-fixed'.
Note that only U2 differs for the tilt-bug or no-tilt-bug versions: U6 is identical.
It would appear that U6 was the 'operating system' ROM, and U2 was set up on a per-game
basis.

My U2 ROM matches the 'with-tilt-bug' version of U2.

The tilt bug is not worth worrying about: it is an edge case that one is unlikely to encounter in real life.

## ROM Addressing Notes

The 9316 ROMs must be 2Kx8 ROM versions of a 2716 EPROM, hence the xx16 in its name.
The schematic confirms this. Pins A0 through A10 (2K bytes) are connected to address pins.
A11 is used to generate CS signals for the U2 and U6 ROMs:
- U6 pin 20 is labeled !CS and is wired to !A11
- U2 pin 20 is labeled !CS and is wired to A11 (if E3 and E4 are jumpered, as they are in my machine)

This means:
- When A11 is '1', U6 is selected, and U2 is deselected
- When A11 is '0', U6 is deselected, and U2 is selected
