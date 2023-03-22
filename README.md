# CFD-B
- 実行時ディレクトリ構成  
    /cfd-b/cfd/cases/\<case\>内にcase.text, job.sh, boundary.txt, output/を配置．  

- 実行コマンド  
    /cfd-b/cfd/cases/\<case\>/output  
    にて，  
    mpirun -np \<プロセス数\> ../../../Release/abmac2d ../case.txt  
    または  
    pjsub ../job.sh  
- メッシュファイル  
    配布のメッシュファイルはcfd-bの外に置く
