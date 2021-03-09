"""
Sends test commands to stdout
"""
import sys


log_path = "log.txt"


def send_cmd(fd, cmd):
    fd.write(cmd + '\n')
    print(cmd)


def test_seek_write_read(fd):
    """
    Tests seek and write/read operation on single layer of nesting
    """
    send_cmd(fd, "destroy")
    send_cmd(fd, "init")
    send_cmd(fd, "ls /")
    send_cmd(fd, "quit")


if __name__ == "__main__":
    fd = open(log_path, 'w')
    fd.truncate(0)

    test_seek_write_read(fd)

    fd.close()
