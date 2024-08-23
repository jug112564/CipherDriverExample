# CipherDriverExample
드라이버와 응용어플리케이션이 이용해 통신하는 프로젝트

- windows10 x64 테스트 환경 전용
- AES128사용중
- 영어만 지원중

## folder
- AES128 : AES128 암호화를 수행하는 코드 파일이 들어가있음

- AppTest : 통신을 쉽게 할 수 있도록 도와주는 응용 어플리케이션 프로젝트

- CipherApi : AES128을 이용하기 위한 라이브러리 프로젝트 (*어플리케이션과 커널 둘다 사용하기 위해서는 서로 다른 빌드 방법으로 빌드해야함)

- TestCipher : 문자를 받은뒤 받은 문자를 그대로 암호화해서 보내는 통신 기능이 있는 드라이버 프로젝트

## Using Example
시작 화면

![image](https://github.com/user-attachments/assets/165222ab-91de-46a5-a07c-fb6888edb85b)

문자열 입력 및 전송 성공 화면
*성공을 위해 반드시 driver 설치 및 필요

![image](https://github.com/user-attachments/assets/6c49c5b4-68b7-44d3-93d5-7d087f2705f3)

문자열 받기 성공 화면

![image](https://github.com/user-attachments/assets/f0807183-aff1-44dd-91d8-b1233de114de)

받은 후 결과 출력 화면
암호화된 문자열과 암호화된 문자열을 바탕으로 해독한 결과가 동시에 출력됨

![image](https://github.com/user-attachments/assets/1596babc-db17-4067-a3c3-f1245e2ab893)
