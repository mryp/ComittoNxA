# ComittoNxA

[ComittoNxAのフォーク](https://github.com/yuma211-lab/ComittoNxA)のフォーク。

個人で楽しむために作成しています。  
ソースの再利用は自由です。  
利用しているライブラリはそれぞれのライセンスに従ってください。  

## 変更履歴

### Ver1.65A20B2 (2021/11/13)
  - yuma211-lab/ComittoNxA Ver1.65A20C2 のコードをマージ

### Ver1.65A20B1 (2021/11/10)

  - yuma211-lab/ComittoNxAからフォークしたソースコードをベースにして作成
  - ファイル名に一・二・三や上・中・下の文字が含まれている名前のあるフォルダを表示したとき、並び替え処理時に
    IllegalArgumentException例外が発生しアプリが強制終了するフォルダが存在するため、これらの文字の並び替え時の特別処
    理を削除しました。  
    これにより一・二・三や上・中・下のファイル名は文字コード順に並び替えられるようになります。
