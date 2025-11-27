# MZ-2500 USB キーボード変換機

![board](/pictures/board.jpg)

## はじめに

ヤフオクで MZ-2500 を入手したのですが、キーボードが付いていませんでした。
仕方ないので、USB キーボードを接続できるようにします。

## 回路

いつものように Raspberry Pi Pico2 を使って 5V 信号に直結することで、部品数を減らします。
必要なパーツは以下の通りです。

- Raspberry Pi Pico2 (または互換機)
- Mini DIN 8ピンコネクタ
- USB-OTG ケーブル(変換機)

Pico2 W での動作は未確認です。
USBキーボードは Pico2 の USB 端子に OTG ケーブルを介して接続します。

Connections

```
GP0: RSTN (MiniDIN 2 Pin)
GP1: KD0  (MiniDIN 3 Pin)
GP2: KD1  (MiniDIN 4 Pin)
GP3: KD2  (MiniDIN 5 Pin)
GP4: KD3  (MiniDIN 6 Pin)
GP5: KD4  (MiniDIN 7 Pin)
GP6: MPX  (MiniDIN 8 Pin)

VBUS: +5V (MiniDIN 1 Pin)
GND: FG (MiniDIN のシールドケース)
```

GND は MiniDIN コネクタのピンではではなく、シールドケースからとります。
なお市販の MiniDIN コネクタに付属のケースを装着すると MZ-2500 に刺さらないと思いますので、ご注意ください。
自分は中身だけ使ってテープで巻いています。

## Gallary

![test](/pictures/testing.jpg)

## 参考資料

https://eaw.app/mz25key/