from struct import unpack
from time import sleep, time

from min import ThreadsafeTransportMINSerialHandler

MIN_PORT = '/dev/ttyUSB0'

class mainMINFrame:
    def __init__(self, min_id: int, payload: bytes):
        self.min_id = min_id
        self.payload = payload

def bytes_to_int32(data: bytes, big_endian=True) -> int:
    if len(data) != 4:
        raise ValueError("int32 should be exactly 4 bytes")
    if big_endian:
        return unpack('>I', data)[0]
    else:
        return unpack('<I', data)[0]

def wait_for_frames(min_handler: ThreadsafeTransportMINSerialHandler):
    while True:
        # The polling will generally block waiting for characters on a timeout
        # How much CPU time this takes depends on the Python serial implementation
        # on the target machine
        ################################################################
        # The following 5 lines can be uncommented if the user wants to send a
        # custom min_id and payload.
        # You must also comment out lines 45, 46, 57, 58, and 59.
        ################################################################
        # min_id = input("Enter MIN ID between 0-63 or 254 for RESET: ")
        # mainMINFrame.min_id = int(min_id)
        # payload = input("Enter payload: ")
        # mainMINFrame.payload = bytes(payload, encoding='ascii')
        # min_handler.queue_frame(min_id=mainMINFrame.min_id, payload=mainMINFrame.payload)
        frames = min_handler.poll()
        if frames:
            return frames


if __name__ == "__main__":
    min_handler = ThreadsafeTransportMINSerialHandler(port=MIN_PORT)
    mainMINFrame.min_id = 0x05
    while True:
        mainMINFrame.payload = bytes("hi STM32L476RG", encoding='ascii')
        min_handler.queue_frame(min_id=mainMINFrame.min_id, payload=mainMINFrame.payload)

        for frame in wait_for_frames(min_handler):
            if frame.min_id == mainMINFrame.min_id:
                print("(Payload received: '{}')".format(frame.payload))
                print("Payload length received = {}".format(len(frame.payload)))
                print("")
            elif frame.min_id == 0x33:
                print("(Time = {})".format(bytes_to_int32(frame.payload, big_endian=False)))
            elif frame.min_id == 0xFF:
                print("ACK received")
        mainMINFrame.min_id += 1
        if mainMINFrame.min_id > 0x09:
            mainMINFrame.min_id = 0x05
        sleep(2)