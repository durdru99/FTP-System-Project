## **Multi-Client File Transfer System**
This project implements a **multi-client** file transfer system using **TCP sockets** in C. It allows clients to:
- Authenticate with a username and password.
- List their files stored on the server.
- Upload new files to the server.
- Download their files from the server.
- Exit the session.

The **server supports multiple concurrent clients** using **multi-threading (pthread)**.

---

## **1. Features**
### ✅ **Phase 1: Single Client FTP System**
- User authentication via `user_data.txt`.
- Clients can **list, upload, and download** files.
- Access control ensures **users cannot access others' files**.
- The client can safely **exit the session**.

### ✅ **Phase 2: Multi-Client Support (Threaded Server)**
- Uses **multi-threading** (`pthread_create()`) to support multiple clients simultaneously.
- Each client runs in a **separate thread**, preventing blocking.
- Server handles **concurrent** uploads, downloads, and listings.
- Clients interact independently without affecting others.

---

## **2. Project Structure**
```
📂 CSCI4211_project
│── 📄 README.md              # Documentation (this file)
│── 📄 Makefile               # Compilation automation
│── 📄 user_data.txt          # Stores valid username-password pairs
│── 📄 CSCI4211PJ1_serve.c    # Server-side implementation
│── 📄 CSCI4211PJ1_client.c   # Client-side implementation
│── 📂 ServerDir/             # Directory where uploaded files are stored
│── 📄 alice_file1.txt        # Sample file for Alice
│── 📄 bob_file1.txt          # Sample file for Bob
│── 📄 carol_file1.txt        # Sample file for Carol
```

---

## **3. Installation and Compilation**
This project requires **GCC** and **Make**.

### **📌 Steps to Compile and Run**
1. **Navigate to the project directory**
   ```sh
   cd CSCI4211_project
   ```
2. **Compile the server and client**
   ```sh
   make all
   ```
   This compiles `CSCI4211PJ1_serve.c` and `CSCI4211PJ1_client.c`.

3. **Start the server**
   ```sh
   ./server
   ```

4. **Start the client (in a separate terminal)**
   ```sh
   ./client
   ```

---

## **4. How to Use the Client**
### **Login Process**
- The client will prompt:
  ```
  Enter username: alice
  Enter password: password123
  ```
- If credentials are **correct**, the client is authenticated.
- If credentials are **incorrect**, the connection is terminated.

### **Available Commands**
| **Command**       | **Description** |
|------------------|----------------------------|
| `list_server`   | Lists all files owned by the logged-in user on the server. |
| `upload`        | Uploads a file to the server. |
| `download`      | Downloads a file from the server. |
| `exit`          | Closes the client connection. |

### **Example Usage**
```
Enter command: list_server
Server files:
alice_file1.txt
alice_file2.txt
```
```
Enter command: upload
Enter filename: my_notes.txt
Enter file content: This is my project report.
Server Response: File uploaded successfully.
```
```
Enter command: download
Enter filename: alice_file1.txt
In alice_file1.txt >>> This is alice_file1.txt.
```

---

## **5. How the Server Works**
- The server listens for incoming client connections on **port 8080**.
- **Multi-threading** allows handling multiple clients **simultaneously**.
- Stores user credentials in `user_data.txt`.
- Manages files inside `ServerDir/`, where each user has their own files.

### **Server Start Message**
```
Server listening on port 8080...
Waiting for clients...
```
### **Example Server Log**
```
Client connected.
User authenticated: alice
Handling command: list_server
Handling command: upload
Handling command: download
Client disconnected.
```

---

## **6. Cleanup and Troubleshooting**
### **To Remove Compiled Files**
```sh
make clean
```

### **Common Errors and Fixes**
| **Error**                           | **Solution** |
|----------------------------------|--------------------------------------------|
| `Connection to the server failed…` | Ensure the server is running before starting the client. |
| `Invalid username or password`   | Use correct credentials from `user_data.txt`. |
| `File not found`                 | Check if the file exists in `ServerDir/`. |
| `Segmentation fault`              | Ensure all inputs are correctly formatted. |

---

## **7. Future Improvements (Optional)**
✅ Implement **file encryption** for security.
✅ Store passwords using **hashing** instead of plaintext.
✅ Add **logging** to track server activity.
✅ Implement **file sharing** between users.

---

## **8. Authors & Credits**
This project was developed for **CSCI 4211: Computer Networking**.
- **Author:** [Your Name]
- **Instructor:** [Instructor's Name]
- **Institution:** University of Minnesota

📌 **GitHub Repository:** [Your GitHub Link]

🚀 **Thank you for using this project!**

