这篇文档内容主要是关于游戏设计框架的，将系统地说明整个代码设计的结构，以及数据编写的格式问题。


事件
游戏中存在一些事件，。它们有以下几类：
被 [] 包围的语句是注释

None
纯命令：

pause
暂停事件，然后退出事件，有格式：
    pause

One
一对一式：一个命令配合一个描述

delay
指定延时，用于增加命令之间的时间间隔
    delay time

map
此事件将指定事件在哪张地图上执行，按照格式：
    map name

talk
此事件将会开启一段对话，按照格式：
    talk 对话内容
需要注意，对话内容必须是连续的，不能有空格或换行

tip 
此事件发起提示，按照格式：
    tip 提示内容

goto
此事件跳过行，直到遇见指定标记为止，按照格式：
    goto done   // 则会一直跳过，直到遇见 done

back
此事件将会向前回溯，直到遇见指定标记为止，按照格式：
    back command
一般会选择返回到之前执行的命令（此命令较为复杂，因为返回难度比较大，暂不考虑）

question
此事件发起询问，如果回答 yes 执行下一行，否则跳过下一行，一般配合 goto 使用；按照格式：
    // 如果不想继续，可以退出询问，从而可以反复进入
    question 提问内容
    goto yes  // yes
    goto no  // no

    ...

    // 跳过上面，直到遇见 yes
    yes
    ...
    goto done   // 为了不执行 no ，再次跳过

    no
    ...

    done
如此便得到一个分支结构

delete
此命令将销毁一个物体，按照 id 识别，有格式：
    delete id
 
open
此命令打开指定的门，按照 id 识别，有格式：
    open id

test 
此命令检测某个物体是否存在，按照 id 识别，其使用方法和 question 类似：
    test id
    goto yes  // yes
    goto no  // no

    ...

    // 跳过上面，直到遇见 yes
    yes
    ...
    goto done   // 为了不执行 no ，再次跳过

    no
    ...

    done

background
此命令更换背景，按照背景名，有格式：
    background name

music 
此命令播放音乐，按照音乐名，有格式：
    music name

link
按照 id 链接其它事件，然后退出此事件，进入该事件，有格式：
    link id

cancel
取消指定物体的事件，有格式：
    cancel id

lock
此命令锁定指定物体的运动状态，如果 id 为 0 则表示锁定绘图原点
    lock id

unlock
此命令取消对物体的锁定，如果 id 为 0 则表示取消锁定绘图原点
    unlock id

origin
此命令移动绘图原点，到指定位置，用于改变视野
    origin x y

Two
一对二：一个命令需要两个描述

bind
为指定物体绑定事件，有格式：
    bind id eId

Three
一对三：一个命令需要三个描述

move
此命令移动一个物体到指定位置，按照 id 识别，有格式：
    move id x y

create
此命令在指定位置创建一个物体，按照 name 识别，有格式：
    create name x y
    
buff
此命令给予指定物体加成，有格式：
    buff id 属性 加成

debuff
此命令给予减损，有格式：
    debuff id 属性 减损