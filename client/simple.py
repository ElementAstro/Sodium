import socket
import json
import threading

def receive_data(client_socket):
    while True:
        try:
            response = client_socket.recv(1024)
            if not response:
                print("Connection closed by server")
                break
            print(f"Received response from server: {response.decode('utf-8')}")
        except Exception as e:
            print(f"An error occurred while receiving data: {e}")
            break

def send_data(client_socket):
    while True:
        user_input = input("Enter JSON data to send (or 'exit' to quit): ")

        if user_input.lower() == 'exit':
            client_socket.close()  # Close the socket to signal the receiver to stop
            break

        try:
            json_data = json.loads(user_input)
            json_string = json.dumps(json_data)
        except json.JSONDecodeError as e:
            print(f"Invalid JSON data: {e}")
            continue

        try:
            client_socket.sendall(json_string.encode('utf-8'))
            print("Data sent to server")
        except Exception as e:
            print(f"An error occurred while sending data: {e}")
            break

def start_client(host, port):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        client_socket.connect((host, port))
        print(f"Connected to server at {host}:{port}")

        receiver_thread = threading.Thread(target=receive_data, args=(client_socket,))
        sender_thread = threading.Thread(target=send_data, args=(client_socket,))

        receiver_thread.start()
        sender_thread.start()

        sender_thread.join()
        receiver_thread.join()

    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        client_socket.close()
        print("Client socket closed")

if __name__ == "__main__":
    host = input("Enter server IP address: ")
    port = int(input("Enter server port: "))
    start_client(host, port)
