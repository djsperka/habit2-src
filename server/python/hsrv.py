#!/usr/bin/env python3

import socketserver
import sys

class MyTCPHandler(socketserver.BaseRequestHandler):
    """
    The RequestHandler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def handle(self):
        # self.request is the TCP socket connected to the client
        #self.data = self.request.recv(1024).strip()
        print("connection from {}:".format(self.client_address[0]))

        # send hello
        self.request.sendall(b'hello')
        
        # now prompt for commands
        while True:
            cmd = input("Enter command: ")
            if 'q' == cmd[0]:
                break
            self.request.sendall(cmd.encode('utf8'))
            

if __name__ == "__main__":
    HOST, PORT = "localhost", 5254

    # Create the server, binding to localhost on port 9999
    server = socketserver.TCPServer((HOST, PORT), MyTCPHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    #server.serve_forever()
    server.handle_request()