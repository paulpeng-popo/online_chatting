## Program design
This program was written in C. With the access to the UNIX socket library, my [`TcpLib.h`] and my [`Huffman.h`], I implemented a simple but failed ftp program. As you can see below.

### Client Side
[`Client.c`] here contain a poor GUI, but functions ok during a ftp client.

In the first page, we can see the Ip-address and port to specify.
Second page to select an operation.
Third page to choose one file to upload.
and the last could upload file using None, Fixed Huffman, Variable Huffman.

### Server Side:

[`Server.c`] is rather a simple program, providing the ability to be made connection with and receive files from the client. When using Huffman encoding, the program first receives the encoded file and the codewords, then uses them to retrieve the original file. When the connection is closed by the client, the program proceeds to listen for another connection request.

### Huffman Encoding:

It's so hard. I hate Huffman.
