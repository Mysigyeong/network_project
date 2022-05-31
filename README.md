# Network Team Project - Uno Simulator

> Team 3 - 김상민 김성준 김진우 박지예 우병수 이준석
> 

## 소스코드 위치

## Uno & Uno’s Rule

## 사용법

### Uno Scratch

게임 참가자 수는 2~10인이다.

```python
./waf --run "uno --Num_Client={게임 참가자 수}"
```

게임 참가자 수를 명시한 후 실행하면, scratch는 게임 참가자 수에 맞게 client node들을 생성한 후, client node에는 UnoClientHelper를 이용하여 UnoClient application을 설치하고, server node에는 UnoServerHelper를 이용하여 UnoServer application을 설치한다.

![Untitled](Network%20Team%20Project%20-%20Uno%20Simulator%20d49a6a3ccbba4f0a91174065ec4f0b91/Untitled.png)
