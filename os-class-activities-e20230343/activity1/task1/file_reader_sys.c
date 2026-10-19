/* file_reader_sys.c */
#include <fcntl.h>
#include <unistd.h>

int main() {
    char buffer[256];
    ssize_t bytesRead;

    // 1. Open "output.txt" for reading
    int fd = open("output.txt", O_RDONLY);
    if (fd < 0) {
        const char *err = "Error opening file\n";
        write(2, err, 19);   // fd 2 = stderr
        return 1;
    }

    // 2-3. Read a chunk, write that chunk to the terminal; read() returns 0 at end of file
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        write(1, buffer, bytesRead);
    }

    // 4. Close the file
    close(fd);
    return 0;
}
