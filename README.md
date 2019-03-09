# NTUEE光舞教學

> 註:這是從零開始的最基礎教學，會包含一些git的基礎指令，如果不熟的建議慢慢看完~~~
> 
> 對git, node.js, python都有概念的人可以直接跳到[如何讓光舞真正運作?](#%E5%A6%82%E4%BD%95%E8%AE%93%E5%85%89%E8%88%9E%E7%9C%9F%E6%AD%A3%E9%81%8B%E4%BD%9C)

## 教學內容索引 

- [NTUEE光舞教學](#ntuee%E5%85%89%E8%88%9E%E6%95%99%E5%AD%B8)
  - [教學內容索引](#%E6%95%99%E5%AD%B8%E5%85%A7%E5%AE%B9%E7%B4%A2%E5%BC%95)
    - [git 小知識](#git-%E5%B0%8F%E7%9F%A5%E8%AD%98)
    - [如何下載程式碼?](#%E5%A6%82%E4%BD%95%E4%B8%8B%E8%BC%89%E7%A8%8B%E5%BC%8F%E7%A2%BC)
    - [安裝環境](#%E5%AE%89%E8%A3%9D%E7%92%B0%E5%A2%83)
    - [認識各個資料夾&重要檔案說明](#%E8%AA%8D%E8%AD%98%E5%90%84%E5%80%8B%E8%B3%87%E6%96%99%E5%A4%BE%E9%87%8D%E8%A6%81%E6%AA%94%E6%A1%88%E8%AA%AA%E6%98%8E)
    - [如何使用光舞的網頁模擬?](#%E5%A6%82%E4%BD%95%E4%BD%BF%E7%94%A8%E5%85%89%E8%88%9E%E7%9A%84%E7%B6%B2%E9%A0%81%E6%A8%A1%E6%93%AC)
    - [硬體教學](#%E7%A1%AC%E9%AB%94%E6%95%99%E5%AD%B8)
    - [如何讓光舞真正運作?](#%E5%A6%82%E4%BD%95%E8%AE%93%E5%85%89%E8%88%9E%E7%9C%9F%E6%AD%A3%E9%81%8B%E4%BD%9C)
      - [Linux](#linux)


### git 小知識
光舞的程式碼在github上，要把它clone下來才能在你的電腦上運作。建議對git不熟的人可以先看[這個連結](https://youtu.be/HVsySz-h9r4)，大概30分鐘，他把git的基本觀念講的清楚易懂，不只光舞，日後幫助也很大，或許以後還會回來翻這部影片呢!!
    
會用到的指令大概就git pull, git add, git commit, git push等，其實官網的文字說明也很清楚喔！要編輯和push上去的話，記得請負責人把你的github帳號設為Collaborators喔~~
### 如何下載程式碼?
首頁大概長這樣
![](https://i.imgur.com/6ynHEBS.png)
點選右上角綠色的 "clone or download"，選擇 Use HTTPS比較方便![](https://i.imgur.com/fyeUILi.png)

像這樣，接著複製網址，在你想要的路徑打開terminal或是cmd(記得裝好git)，輸入 "git clone 網址" ，等一下子就好了~~
### 安裝環境
要安裝python3, Node.js，搜尋他們的官網，基本上照著指示一路按「是」就會裝好了，只是要記得安裝python3的時候，要add path，如下圖所示
![](https://i.imgur.com/aevcgbc.jpg)

### 認識各個資料夾&重要檔案說明
認識各個資料夾&重要檔案說明
![](https://i.imgur.com/mrr0Lxl.png)

比較需要編輯更動的檔案幾乎都在editor；music想當然爾是放音樂檔的；其他的說明?
editor中
 1. EE_night.in, EE_night.pos分別是紀錄各部位光條亮的時間和人員位置
 2. sim.js 裡面有光舞模擬假人的樣貌，要請會javascript的同學修改成當下服裝樣式
 3. translate.py 則是把 (1)的input檔們輸出成js檔，如果input檔的格式和先前稍微不一樣的話，可能在parse()部分要小小修改。
 4. light.js, pos.js 分別是(3)的輸出檔，基本上不需要更動
 5. 其他的幾乎不會動到
### 如何使用光舞的網頁模擬?
進去editor資料夾，如果是windows就在terminal輸入
```
python translate.py
```
如果是linux或Mac OS就輸入
```
python3 translate.py
```
可能會出現類似當掉的樣子，沒關西，ctrl-c就好。接著打開index.html，就可以在你的瀏覽器看到光舞模擬的樣子和音樂。

### 硬體教學
~~很複雜欸，自己想辦法~~

先寫個簡易版，以後再補充

- 確保linkit 7688上已經有compile好的control和config.json和client.js，/etc/rc.local上寫好開機自動執行的腳本:
```shell
cd /root/
node client.js
exit 0
```

- common.h裡的SERVER_ADDR和config.json裡的server_ip都是電腦的ip
- 確定linkit 7688能連上你的路由器
- 設定ip的方法：
  1. 連到你的路由器，把區網的閘道設成192.168.XXX.1
  2. 電腦連上路由器，打開網路介面卡設定(ipv4)
  3. 原本是自動/DHCP的，改成手動設定
  4. ip改成192.168.XXX.2(XXX和閘道的XXX相同)
  5. 子網路遮罩設成255.255.255.0
  6. 閘道設成192.168.XXX.1
  7. DNS不填也可以，或隨便填個8.8.8.8之類
  8. 上面提過的server_ip和SERVER_ADDR都改成192.168.XXX.2
- 在電腦端想連上linkit 7688的話，請善用arp-scan或ip scanner等軟體找到linkit 7688的ip再用ssh連它

### 如何讓光舞真正運作?

#### Linux
安裝套件
```shell
$ sudo apt-get install nodejs
$ sudo apt-get install npm
$ git clone https://github.com/andyh0913/NTUEE_light_dance.git
$ cd NTUEE_light_dance/command_center
$ npm install
```
開一個終端執行以下指令
```shell
$ cd NTUEE_light_dance/command_center
$ node server.js
$ generate
$ all upload
$ all controllocal
```
開另一個終端執行以下指令
```shell
$ cd NTUEE_light_dance/server
$ python3 server.py
```
按enter之後系統會開始倒數，五秒後音樂會開始放