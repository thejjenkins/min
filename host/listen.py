"""
Simple example program that sends a MIN frame and waits for a response frame
"""
from time import sleep

from .min import MINTransportSerial, bytes_to_hexstr

# Linux USB serial ports are of the form '/dev/ttyACM*'
# macOS USB serial ports are of the form '/dev/tty.usbmodem*'
# Windows randomly assigns COM ports, depending on the driver for the USB serial chip.
# Genuine FTDI chips tend to end up at the same port between reboots. YMMV.
MIN_PORT = 'COM4'


def wait_for_frames(min_handler: MINTransportSerial):
    while True:
        # The polling will generally block waiting for characters on a timeout
        # How much CPU time this takes depends on the Python serial implementation
        # on the target machine
        frames = min_handler.poll()
        if frames:
            return frames


if __name__ == "__main__":
    # One approach to autodiscovering a port with a MIN device on the other end is to open
    # all the ports on the machine and listen for a heartbeat: the chances of random data
    # appearing as a valid MIN frame with a magic number in the payload are virtually zero.
    min_handler = MINTransportSerial(port=MIN_PORT)

    payload = bytes("hello world", encoding='ascii')
    min_id = 0x01
    while True:
        # Send a frame on the serial line
        print("Sending frame: min ID={}, payload={}".format(min_id, bytes_to_hexstr(payload)))
        min_handler.send_frame(min_id=min_id, payload=bytes("hello world"))

        # Wait for one or more frames to come back from the serial line and print them out
        for frame in wait_for_frames(min_handler):
            print("Frame received: min ID={}, payload={}".format(frame.min_id, bytes_to_hexstr(frame.min_payload)))

        # Wait a little bit so we don't flood the other end
        sleep(0.5)
