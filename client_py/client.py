import socket
import struct

SOCK_PATH = "/tmp/video.sock"
CMD_LIST = 2

sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
sock.connect(SOCK_PATH)

payload = struct.pack("i256s", CMD_LIST, b"")
sock.sendall(payload)

data = sock.recv(256)
print("Server:", data.decode(errors="ignore"))

sock.close()
