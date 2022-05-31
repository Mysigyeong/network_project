# Network Team Project - Uno Simulator

> Team 3 - 김상민 김성준 김진우 박지예 우병수 이준석
> 

# 소스코드 위치

- ns-3.29/scatch/uno.cc
- ns-3.29/src/applications/uno/*

# Uno & Uno’s Rule

50년이 넘는 역사를 자랑하는 게임으로 1971년에 멀 로빈스가 개발. 현재까지도 인기있는 카드 게임

## 구성

- (빨강, 노랑, 초록, 파랑) 4가지 색상의 카드. 각 색깔별로 1장의 0번 카드와 2장씩의 1~9번 카드, 그리고 2장씩 3종의 특수 카드 존재
- 특수 카드 - 순서 건너뛰기, 순서 뒤집기, 다음 사람 카드 2장 뽑게 하기
- 검은색으로 표시되는 2종의 와일드 카드는 각각 4장씩 존재 - 카드 색깔 바꾸기, 다음 사람 카드 4장 뽑게 하기

<img src="https://user-images.githubusercontent.com/50205887/171220797-ac0bafe6-10d8-4e8a-a39a-a7a52eb858c8.png" width="400" height="400"/>

## Rule

기본적인 게임 방식은 순서대로 카드를 낼 수 있을 때 내고 낼 수 없으면 덮어둔 카드더미에서 카드를 한 장 가져가다가 패를 다 낸 사람이 승리하는 것

- 시작 시 각각 7장의 카드를 뽑는다. 남은 카드는 덮어두고 그 중 맨 위의 카드를 덱 옆에 한 장 뒤집어 보이도록 놔두면 게임이 시작된다
- 시계방향의 다음 사람부터 순서대로, 자신의 차례가 오면 현재 앞면인 가장 위쪽의 카드와 같은 색깔이나 기호(숫자 혹은 특수 카드 마크)의 카드를 한 장 내서 앞면으로 맨 위에 놔두고 넘어간다. 혹은 언제든 낼 수 있는 와일드 카드를 내거나. 만약 그런 카드가 없으면 덮어둔 카드더미에서 한 장을 가져간다.
- 이런 식으로 플레이를 하다가 자기 패에 카드가 2장 있는데 1장을 내면서 1장만 남게 되는 상황이 되면, 내기 직전 또는 낸 후에 '우노'를 외쳐야 한다. 만약 내고 나서 다른 사람이 먼저 외쳤을 경우 2장을 먹어야 한다.

### Special Card Rule

- 스킵 카드 : 자신의 다음 사람을 건너뛰는 것
- 리버스 카드 : 현재 순서가 시계 방향이었으면 시계 반대 방향으로, 시계 반대 방향이었으면 시계 방향으로 순서가 바뀌게 된다
- 드로우 2 카드(+2) : 다음 사람이 카드 2장을 뽑고 턴을 넘겨야 한다
- 와일드 : 낸 다음 색깔 하나를 선언하면 그 다음 사람은 와일드 카드 아니면 그 색깔의 카드를 내야 한다
- 와일드 드로우 4 : 다음 사람은 카드 4장을 뽑고 턴을 넘겨야 한다. 덤으로 이 카드를 낼 때 와일드 카드와 동일하게 색깔을 선언하며, 그 다음 플레이어는 그 색깔의 카드를 내야 한다.

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

ns-3.29/src/applications/uno/model/uno-card.cc, uno-card.h

uno card deck 은 0~9까지의 숫자 중 하나와, 빨강, 노랑, 파랑, 초록 중 한가지의 색상의 쌍으로 이루어져 있는 카드와, 스페셜 카드, 와일드 카드로 구성되어 있다.

색깔 별로 0은 1장, 1부터 9는 2장씩, (19x4)

색깔 별로 스페셜 카드(SKIP, REVERSE, DRAW_TWO) 2장씩,(6x4)

와일드 카드(WILD, WILD_DRAW_FOUR) 4장씩, (2x4) 

→ 총 108장의 카드가 deck에 존재한다.

Uno class안에는 총 4가지의 function이 존재한다.

1. **void Uno::Shuffle()**: deck을 random하게 shuffle 한다.
2. **card Uno::Draw()**: deck에서 맨 위의 카드 한장을 뽑아 return 한다.
3. **void Uno::Collect_Trash(card c)**: 플레이어가 제출한 카드 c를 trash_deck에 넣는다.
4. **void Uno::Trash2Deck()**: trash_deck을 Shuffle()하여 deck으로 바꿔준다.

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

### UnoServer::UnoPacketCreate(uint32_t uid)

user에게 최초에 7장의 카드를 나눠주는 패킷 제작

- uid: 게임 참가자의 고유한 user id

```cpp
UnoPacket up;
up.seq=seq_num++;
up.gameOp=GameOp::INIT;
up.numOfCards=7;
up.uid=uid;

for(int j=0;j<7;j++) {
	up.cards[j]=unogame.Draw();
}
```

### UnoClient::Answer(Ptr<Packet> packet)

들어온 패킷에 대해서 client의 응답을 패킷으로 만들어 반환

들어온 패킷의 game operation이 INIT인 경우, 패킷 그대로 반환. 이후 이 패킷을 서버로 답장

```cpp
case GameOp::INIT:
       {...}
		ret_packet=packet;
		break;
```

### UnoServer::PacketRead(uint32_t uid)

클라이언트가 보낸 INIT에 대한 답장을 받으면 read_player 수를 늘리고, 이 수가 플레이어 수와 같아지면 게임 시작

```cpp
case GameOp::INIT:
		{...}
		ready_player++;
		cout<<"player "<<uno_packet->uid<<" is ready"<<endl<<endl;
		if(ready_player==unogame.player_No){
			cout<<"All player ready to start!!"<<endl<<endl;
		{...}
		unogame.turn++;
		m_sendEvent = Simulator::Schedule (Seconds(1.), &UnoServer::Send, this, unogame.playing);
		}
		break;
```

클라이언트가 보낸 TURN과 DRAW에 대해 각각에 맞는 동작을 실행한다. 

- TURN : 플레이어가 취하는 행동

해당 플레이어의 동작에 따라 게임을 진행한다.

```cpp
case GameOp::TURN:
		//user가 play 했으면,
		if(uno_packet->userOp==UserOp::PLAY) {
			// Shout UNO
			if(uno_packet->numOfCards == 1){
				/*** Implementation  ***/
			}
			else if (uno_packet->numOfCards == 0) {
				/*** Implementation  ***/
			}
			else {
				HandleCardEffect();
			}
		}
		//user가 draw 했으면,
		else if(uno_packet->userOp==UserOp::DRAW){
			/*** Implementation  ***/
		}
    break;
```

- DRAW : 플레이어가 서버의 덱으로부터 카드를 성공적으로 받아, 턴을 차례로 넘길 수 있음

```cpp
case GameOp::DRAW:
        cout<<"Card draw Success"<<endl<<endl;
        ChangingTurn();
        m_sendEvent = Simulator::Schedule (Seconds(1.), &UnoServer::Send, this, unogame.playing);
        break;
```

## Broadcasting

![그림1](https://user-images.githubusercontent.com/50205887/171222989-b878686f-93a1-4137-bd3f-8ae19ed523f4.png)

1. 우노 서버에서 게임 Initialize가 이루어지면, 초기 패킷을 Uno Client에 전달한다.
2. 클라이언트에서는 패킷 체크 후 Answer 함수를 호출한다.
3. Answer 함수는 서버로 다시 ‘게임 준비’가 되었다는 패킷을 전송한다.
4. 게임이 시작된다.
5. 이 후 클라이언트의 행동과 게임 상황과 관련된 정보가 지속적으로 Broadcasting 된다.
6. Broadcasting 된 정보에 따라 클라이언트는 Answer에서 이를 처리해 패킷을 생성하고, 다시 Server로 전송한다. 

해당 어플리케이션의 Broadcasting은 크게 두 가지로 나눌 수 있다.

1. 게임 시작 전 전체에게 Ready를 묻는 Broadcasting

```cpp
for(uint32_t i=0;i<num;i++) {
		Address localAddress;
		m_socket[i]->GetSockName (localAddress);
		m_socket[i]->Send (UnoPacketCreate(i));
}
```

2. 게임 시작 후 클라이언트의 동작에 따라 게임 상황을 업데이트 한 후 다음 턴에 해당하는 유저에게 Broadcasting

```cpp
case <GameOP>:
		cout<<"<Game Situation>"<<endl<<endl;
		ChangingTurn();
		m_sendEvent = Simulator::Schedule (Seconds(1.), &UnoServer::Send, this, unogame.playing);
```

## Special Cases

UnoServer::HandleCardEffect(void)에서 unogame.front.number, 즉 게임 상에서 맨위의 카드의 숫자에 따라 어떤 특수 행동을 진행할 지 결정한다. 카드의 색을 바꾸는 경우에는 UnoServer::PacketRead(Ptr<Packet> packet)에서 현재 게임 상의 color를 player가 정한 color로 바꾸어준다. 

### SKIP

SKIP의 경우 다음 player를 건너뛰게 된다. 현재 게임 상의 턴 수를 1 감소함으로써 다음 player를 건너뛸 수 있다. 

```cpp
case SKIP:
        ChangingTurn();
        unogame.turn--;
        cout << "Pass Player " << unogame.playing << "'s turn" << endl;
        break;
```

### REVERSE

REVERSE의 경우 게임의 방향이 반대로 바뀐다. 

```cpp
bool change_order=false

case REVERSE:
        change_order = !change_order;
        cout << "Change order" << endl;
        break;
```

REVERSE가 불리게 되고, 진행 방향이 반시계방향(원래는 시계방향)이라면, 게임 상의 player_NO를 감소시키는 방향으로 진행한다.

```cpp
void
UnoServer::ChangingTurn(void)
{
    unogame.turn++;
    if (!change_order){
        unogame.playing = (unogame.playing + 1) % unogame.player_No;
    }
    else {
        if (unogame.playing == 0) {
            unogame.playing = unogame.player_No - 1;
        }
        else {
            unogame.playing = unogame.playing - 1;
        }
    }
}
```

### DRAW 2

DRAW 2의 경우 다음 player는 카드 2장을 받게 되고, 차례를 건너뛰게 된다. SKIP에서처럼 turn 수를 감소함으로써 차례를 건너뛰고, 다음 player에게 2장의 카드를 보낸다.

```cpp
case DRAW_TWO:
        ChangingTurn();
        unogame.turn--;
        cout << "Player " << unogame.playing << " draws 2 cards" << endl;
        m_socket[unogame.playing]->Send (DrawCardPacketCreate(unogame.playing, 2, false));
        break;
```

### WILD

WILD의 경우 player가 카드의 색깔을 바꿀 수 있다. UnoServer::PacketRead(Ptr<Packet> packet)에서 게임 상에서 진행 중인 색깔을 player가 선택한 색깔로 바꾸어 준다. 

```cpp
case GameOp::TURN:
        //user가 play 했으면,
        if(uno_packet->userOp==UserOp::PLAY) {
						{...}
            unogame.color = uno_packet->color;
```

### WILD DRAW 4

WILD DRAW 4의 경우 player가 색깔을 바꾸는 동시에 다음 player는 카드 4장을 받고, 차례를 건너뛰게 된다. WILD와 같은 방법으로 색깔을 바꾸어주고, DRAW 2와 같은 방법으로 다음 player에게 4장의 카드를 보내며, SKIP과 같은 방법으로 차례를 건너뛴다. 

```cpp
case WILD_DRAW_FOUR:
        cout << "Player " << unogame.playing << " selected color " << PrintColor(unogame.color) << endl;
        ChangingTurn();
        unogame.turn--;
        cout << "Player " << unogame.playing << " draws 4 cards" << endl;
        m_socket[unogame.playing]->Send (DrawCardPacketCreate(unogame.playing, 4, false));
        break;
```

## Uno

이 게임에 기본적으로 카드를 쓰는 방법은 카드를 PLAY 하는 것 밖에 없다. 그러므로 PLAY 단계에서 카드가 1장이 되면 모든 플레이어 에게 우노 패킷을 보내서, 우노를 외칠 수 있게 한다. 이 패킷을 받은 client는 우노 패킷으로 답장을 하게 되는데 이를 받은 서버가 송신자를 확인해서 우노 패널티를 처리하게 된다. (실제 게임 환경에서는 먼저 눌러야 되는 것이지만, 이 프로그램은 가상 시뮬레이션이므로 피치 못하게 이런 방법을 쓰게 됐다)

매번 랜덤하게 우노를 외칠 수 있게 패킷 순서를 랜덤하게 보낸다

## End of Game

UNO와 비슷한 방식으로 플레이어가 PLAY를 했을때 그 사람의 카드가 0장이 된다면 그 사람의 승리로 게임이 끝나게 된다.
