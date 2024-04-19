#include "packet.h"

int main(){
// Open input file in binary mode for reading
FILE *fp = fopen("input.txt", "rb");
if (fp == NULL)
{
    die("fopen()");
}

// Open output file in binary mode for writing
FILE *output_fp = fopen("output.txt", "wb");
if (output_fp == NULL)
{
    die("fopen()");
}

// Read from input file and write to output file
char buffer[1024];
size_t bytes_read;
while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0)
{
    // Write the read data to the output file
    size_t bytes_written = fwrite(buffer, 1, bytes_read, output_fp);
    if (bytes_written != bytes_read)
    {
        die("fwrite()");
    }
}
fread(packet.payload, 1, PACKET_SIZE, fp);
fwrite(rcv_pkt.payload, 1, recv_len, fp);
// Close files
fclose(fp);
fclose(output_fp);
}