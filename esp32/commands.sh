idf.py set-target esp32s3

idf.py menuconfig

idf.py build

cu.usbmodem5AAF2166111
cu.usbmodem5AAF2165531

python -m esptool --port "/dev/cu.usbmodem5AAF2166111" \
    --chip auto --baud 921600 \
    --before default_reset --after hard_reset write_flash \
    --flash_mode dio --flash_freq 40m --flash_size detect \
    0x0 ./build/bootloader/bootloader.bin \
    0x8000 ./build/partition_table/partition-table.bin \
    0x10000 ./build/flightbox.bin

python -m serial.tools.miniterm "/dev/cu.usbmodem5AAF2166111" 115200
