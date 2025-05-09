process.stdin.on('data', function(data) {
    const request = data.toString();

    process.stdout.write(
        "HTTP/1.1 200 OK\r\n" +
        "Content-Type: text/plain\r\n" +
        "Connection: close\r\n" +
        "\r\n" +
        "Hello from Node js\n"
    );
});