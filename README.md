# Network Team Project - Uno Simulator

> Team 3 - 김상민 김성준 김진우 박지예 우병수 이준석
> 

# 소스코드 위치

> ns-3.29/scatch/uno.cc
> ns-3.29/src/applications/uno/*

# Uno & Uno’s Rule

# 사용법

## Uno Scratch

게임 참가자 수는 2~10인이다.

```python
./waf --run "uno --Num_Client={게임 참가자 수}"
```

게임 참가자 수를 명시한 후 실행하면, scratch는 게임 참가자 수에 맞게 client node들을 생성한 후, client node에는 UnoClientHelper를 이용하여 UnoClient application을 설치하고, server node에는 UnoServerHelper를 이용하여 UnoServer application을 설치한다.

![image](https://user-images.githubusercontent.com/50205887/171147367-d22586f7-979a-46c1-8ead-39e75d1bffd2.png)

## UnoClientHelper::UnoClientHelper(uint16_t port)

게임 참가자 node의 port를 전달해주면 UnoClient는 해당 port에 UDP socket을 만들어놓고, UnoServer로부터 정보를 받을 준비를 한다.

## UnoServerHelper::UnoServerHelper(uint32_t num, Address* addressList)

- num: 게임 참가자 수. 2~10인으로 제한
- addressList: 게임 참가자들이 사용하는 주소 및 port의 리스트로, InetSocketAddress를 이용하여 아래와 같이 정의하여 사용할 수 있다.

```cpp
Address addressList[3] = { Address(InetSocketAddress(Ipv4Address("10.1.1.1"), port)),
                           Address(InetSocketAddress(Ipv4Address("10.1.1.2"), port)),
                           Address(InetSocketAddress(Ipv4Address("10.1.1.3"), port)) };
```

# 구현

## Uno Card

## Init Game

우노 1 게임을 시작하기 위해서 우노 1 게임을 만들고 초기화 시키는 부분이다. 우노 게임의 진행된 턴 수, 현재 플레이 중인 플레이어 , 플레이어 수를 초기화 시킨다. 이후, 카드를 섞고 카드 7장씩을 패킷에 담아 플레이어들에게 보낸다. 각 플레이어들은 이 패킷에 대해 답장을 서버로 보내고 서버는 모든 플레이어들이 준비가 되면, 게임을 시작한다.

### UnoServer::InitUno(uint32_t num)

- num: 게임 참가자 수. 2~10인으로 제한

```cpp
unogame.front=unogame.Draw();

{....}

for(uint32_t i=0;i<num;i++) {
	Address localAddress;
	m_socket[i]->GetSockName (localAddress);
	m_socket[i]->Send (UnoPacketCreate(i));
}
```
