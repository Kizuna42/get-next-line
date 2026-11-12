# get_next_line

42 の課題 get_next_line の実装です。ファイルディスクリプタから 1 行ずつ読み込みます。

```c
int get_next_line(const int fd, char **line);
```

| 戻り値 | 意味 |
| --- | --- |
| `1` | 1 行読み込んだ（`*line` に改行を除いた行が入る） |
| `0` | EOF に到達した（`*line` に最後の行、なければ空文字列が入る） |
| `-1` | エラー（不正な fd、`line` が `NULL`、読み込み・メモリ確保の失敗など） |

`*line` は呼び出し側で `free` してください。

## ファイル構成

| ファイル | 内容 |
| --- | --- |
| `get_next_line.c` / `get_next_line_utils.c` / `get_next_line.h` | 必須部分 |
| `get_next_line_bonus.c` / `get_next_line_utils_bonus.c` / `get_next_line_bonus.h` | ボーナス（複数 fd 対応。fd は `0` 以上 `MAX_FD`（既定 1024）未満） |
| `main_bonus.c` | ボーナスの動作確認用の簡易 main |
| `tests/` | 自動テスト |

## ビルド

`BUFFER_SIZE` はコンパイル時に指定します（未指定時は 42）。

```sh
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 your_main.c get_next_line.c get_next_line_utils.c
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 main_bonus.c get_next_line_bonus.c get_next_line_utils_bonus.c
```

## テスト

```sh
./tests/run_tests.sh
```

必須・ボーナスの両方を複数の `BUFFER_SIZE`（1〜10000000）でビルドし、`tests/files/` の入力に対する出力を検証します。
valgrind がインストールされていればメモリリークも検査します（`NO_VALGRIND=1` で無効化）。
GitHub Actions では Norminette とこのテストを実行します。
