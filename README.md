# Systems-Software
## Assignment 1
Project Requirements:

- Create a daemon to continually manage the operation of the requirements listed by the CTO above.

- Identify new or modified site content and log details of who made the changes, this should be generated as a text file report and stored on the server.

- Maintain a list of site updates (file)

- Update the live site every night (After the backup completes).

- When the backup/transfer begins no user should be able to modify and site content.

- It must be possible to ask the daemon to backup and transfer at any time.

- A message queue should be setup to allow all processes to report in on completion of a task. (success or failure)

- Error logging and reporting should be included (log to file)

- Create a makefile to manage the creation of the executables


## Assignment 2
Project Requirements:

- Create a Server Socket program to run on the same server and the Intranet site.

- Create a Client program to connect to the server socket program.

- The system must be capable of handling multiple clients and transfers simultaneously.

- Transfer restrictions should be controlled using Real and Effective ID’s functionality.

- The files transferred should be attributed to the transfer user. The file transferred to the server should show the transfer owner as its owner.

- The client must take a file name and path via console and transfer this to the server to be stored. The following directoriesare where files can be transferred to:
    - Root dir ( / ) of the Intranet Website
    - Sales
    - Promotions
    - Offers
    - Marketing

- The server must inform the client if the transfer was successful or no
