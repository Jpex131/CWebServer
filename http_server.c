#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>


int main() {
	FILE *html_data;
	html_data = fopen("index.html", "rb");
	if (html_data == NULL) {
		perror("Failed to open index.html");
		return 1;
	}

	//Seek to the end of the file to determine its size
	fseek(html_data, 0, SEEK_END);
	long bytes_read = ftell(html_data);
	rewind(html_data); // Go back to the end of the file

	char *response_data = malloc(bytes_read + 1);
	fread(response_data, bytes_read, 1, html_data);
	response_data[bytes_read] = '\0'; //NUll-terminate the buffer
	fclose(html_data);

	char http_header[2048];
	int header_length = snprintf(http_header, sizeof(http_header), 
												"HTTP/1.1 200 OK\r\n"
												"Content-Length: %ld\r\n"
												"Content-Type: text/html\r\n"
												"\r\n", bytes_read);
	

	// create a socket
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	

	//define the address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(8001);
	server_address.sin_addr.s_addr = INADDR_ANY;

	//bind  socket to a port 
	bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
	listen(server_socket, 5);
	
	while(1) {
		int client_socket = accept(server_socket, NULL, NULL);
		send(client_socket, http_header, header_length, 0);
		send(client_socket, response_data, bytes_read, 0);
		close(client_socket);
	}
	free(response_data);
	close(server_socket);

	return 0;
}
