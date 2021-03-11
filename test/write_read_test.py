"""
Sends test commands to stdout
"""
import random
from time import sleep
from write_read_test_constants import *


def send_cmd(fd, cmd):
    fd.write(cmd + '\n')
    fd.flush()
    print(cmd, flush=True)


def write_randomly(fd, log2):
    ksi = random.randint(MIN_WRITE_READ_SIZE, MAX_WRITE_READ_SIZE)
    pos = 0
    fd_stdin = open(stdin_path, 'w')
    while pos + ksi <= TEST_SIZE:
        fd_stdin.seek(0)
        fd_stdin.truncate(0)
        for i in range(pos, pos + ksi):
            fd_stdin.write(chr(ord('0') + (i % 10)))
            log2.write(chr(ord('0') + (i % 10)))

        fd_stdin.flush()
        sleep(FLUSH_WAIT_TIME)

        send_cmd(fd, "write 0 " + stdin_path + " " + str(ksi))
        sleep(FLUSH_WAIT_TIME)

        pos += ksi
        ksi = random.randint(0, MAX_WRITE_READ_SIZE)

    fd_stdin.seek(0)
    fd_stdin.truncate(0)
    for i in range(pos, TEST_SIZE):
        fd_stdin.write(chr(ord('0') + (i % 10)))
        log2.write(chr(ord('0') + (i % 10)))

    fd_stdin.flush()
    sleep(FLUSH_WAIT_TIME)

    send_cmd(fd, "write 0 " + stdin_path + " " + str(TEST_SIZE - pos))
    sleep(FLUSH_WAIT_TIME)

    fd_stdin.close()


def read_randomly(fd):
    ksi = random.randint(0, MAX_WRITE_READ_SIZE)
    pos = 0
    while pos + ksi <= TEST_SIZE:
        send_cmd(fd, "read 0 " + stdout_path + " " + str(ksi))

        pos += ksi
        ksi = random.randint(0, MAX_WRITE_READ_SIZE)

    send_cmd(fd, "read 0 " + stdout_path + " " + str(TEST_SIZE - pos))


def test_seek_write_read(fd, log2):
    """
    Tests seek and write/read operation on single layer of nesting
    """
    send_cmd(fd, "destroy")
    send_cmd(fd, "init")
    send_cmd(fd, "touch /file")
    send_cmd(fd, "open /file")

    write_randomly(fd, log2)

    send_cmd(fd, "seek 0 0")

    read_randomly(fd)

    send_cmd(fd, "close /file")
    send_cmd(fd, "quit")


if __name__ == "__main__":
    open(stdout_path, 'w').close()

    fd = open(log_path, 'w')
    log2 = open(second_log_path, 'w')
    fd.truncate(0)
    log2.truncate(0)

    test_seek_write_read(fd, log2)

    fd.close()
    log2.close()
