"""
Checks if written and read correctly
"""


stdout_path = "std.out"
log_path = "log.txt"

TEST_SIZE = 500


def check(fd):
    buf = fd.readline()

    log = open(log_path, 'a')

    for i in range(0, TEST_SIZE):
        if buf[i] != chr(ord('0') + (i % 10)):
            log.write("ERROR\n")
            log.write(str(i) + " " + buf[i] + " != " + chr(ord('0') + (i % 10)) + "\n")
            exit(1)

    log.write("OK\n")

    log.close()


if __name__ == "__main__":
    fd = open(stdout_path, 'r')

    check(fd)

    fd.close()
