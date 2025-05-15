1. 學號：B11107051
2. 姓名：李品翰
3. 使用之程式語言：C++
4. 使用之編譯平台：g++ (Debian 12.2.0-14) 12.2.0
5. 檔案壓縮方式: tar zcvf b11107051-p3.tgz b11107051-p3
6. 各檔案說明：
    b11107051-p3/main.cpp    : 主程式、輸出 .out 檔案
    b11107051-p3/Class.h     : 各種 struct 的定義
    b11107051-p3/parser.h    : 解析 .in 檔案
    b11107051-p3/parser.cpp  : parser.h 的實作
    b11107051-p3/routing.h   : 繞線
    b11107051-p3/routing.cpp : routing.h 的實作
    b11107051-p3/makefile    : 編譯、指令巨集
    b11107051-p3/readme.txt  : 本檔案
7. 編譯方式說明：            
    在 b11107051-p3/ 這個資料夾下指令 : make
    即可在 b11107051-p3 產生 picRouting 的執行檔
8. 執行、使用方式說明：
    compile 完成後，在 b11107051-p3/ 目錄下會產生一個 picRouting 的執行檔
    執行檔的命令格式為 :
    ./picRouting [executable file name] [input file name] [output file name]

    ex: ./picRouting test/pic5x5.in test/pic5x5.out