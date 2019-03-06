# NTUEE光舞教學
> 註:這是從零開始的最基礎教學，會包含一些git的基礎指令，如果不熟的建議慢慢看完~~~
## 教學內容索引 

[TOC]

### git 小知識
光舞的程式碼在github上，要把它clone下來才能在你的電腦上運作。建議對git不熟的人可以先看這個連結:https://youtu.be/HVsySz-h9r4 ，大概30分鐘，他把git的基本觀念講的清楚易懂，不只光舞，日後幫助也很大，或許以後還會回來翻這部影片呢!!
    
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
   進去editor資料夾，在terminal輸入"python translate.py"，可能會出現類似當掉的樣子，沒關西，ctrl-c就好。接著打開index.html，就可以在你的瀏覽器看到光舞模擬的樣子和音樂。
### 硬體教學
### 如何讓光舞真正運作?

    

