# 🧠 MiniRedis - 簡易 Key-Value TCP 伺服器

這是一個用 C++ 實作的簡單 Redis 類似系統，支援基本的 Key-Value 指令（`SET`, `GET`, `DEL`, `EXIT`），分為 **伺服器端** 和 **客戶端**，使用於 **Windows** 平台。
新增linux 版本。

> 🔰 適合用於學習 socket 程式設計、thread 同步、字串處理、client-server 架構等。

---

## 📦 功能列表

- ✅ 支援指令：`SET key value`、`GET key`、`DEL key`、`EXIT`
- ✅ TCP socket 通訊
- ✅ 記憶體儲存 key-value（用 `unordered_map`）
- ✅ 支援單 client 模式（可擴充多 client）
- ✅ Windows 和 Linux 雙版本可用
- ✅ 客戶端互動介面（輸入指令並接收回應）

---

### ⚙️ Windows編譯方式（Visual Studio）

1. 分別開啟 ˋclient.sln`、ˋserver.slnˋ
2. 使用 `Ctrl+F5` 執行（避免 console 自動關閉）

### ⚙️ Linux編譯方式
# 編譯 server
g++ src/server.cpp -o build/server -pthread

# 編譯 client
g++ src/client.cpp -o build/client

▶️ 執行方式
# Terminal 1
./build/server

# Terminal 2
./build/client


### 📜 測試指令

```txt
SET name Alice
GET name
DEL name
EXIT
