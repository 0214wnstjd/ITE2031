# How to run
1. `python server.py`
2. `python client1.py`
3. `python client2.py`
4. `python client3.py`


# Specification

## Goal
**멀티 프로세스 소켓 기반의 서버-클라이언트 제작**

## Environment
MacOS     
Python3

# Implementation

## Server.py

- socket을 이용해 serverSocket 생성 후 bind(), listen()
- os.fork()이용하여 3개의 자식 프로세스 생성

- 각 자식 프로세스에서 accept()로 클라이언트 들과 연결

- 클라이언트와 연결이 종료될때까지 recv 이용하여 메시지 받고 출력 후 클라이언트에게 수신 확인 메시지 send

- 클라이언트와 연결이 종료(if not data)되면 connectionSocket을 close 후 os.exit(0)

- 부모 프로세스는 세 번의 os.wait()으로 자식 프로세스 모두 종료될때까지 대기

- 그 후 serverSocket을 close함

## Client.py

- clientSocket 생성 후 connect, 연결 성공시 연결 성공 메시지 출력

- 사용자에게 반복적으로 입력받아 send를 통해 서버에 전송, 서버로부터 수신 확인 메시지 recv

- quit입력시 종료 후 clientSocket를 close함
