"""
Checks if written and read correctly
"""
from write_read_test_constants import *


def check(fd):
    buf = fd.readline()

    log = open(log_path, 'a')

    for i in range(0, TEST_SIZE):
        if buf[i] != chr(ord('0') + (i % 10)):
            log.write("ERROR\n")
            log.write(str(i) + " " + buf[i] + " != " + chr(ord('0') + (i % 10)) + "\n")

            check_seq(log, buf)
            exit(1)

    log.write("OK\n")

    log.close()


def check_seq(log, buf):
    for i in range(0, TEST_SIZE - 1):
        if (ord(buf[i]) + 1 != ord(buf[i + 1])) and ((ord(buf[i]) != ord('9')) or (ord(buf[i + 1]) != ord('0'))):
            log.write(str(i) + " " + buf[i] + " " + buf[i + 1] + "\n")


if __name__ == "__main__":
    fd = open(stdout_path, 'r')

    check(fd)

    fd.close()
