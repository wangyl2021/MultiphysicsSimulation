import json
from fastapi import FastAPI, WebSocket
import uvicorn
from file_exchange import *
from solver_runner import run_solver
from config import *

app = FastAPI()

latest_data = {}  

# websocket端点
@app.websocket("/ws")
async def websocket_endpoint(ws: WebSocket):
    await ws.accept()
    print("客户端已连接")

    while True:
        try:
            msg = await ws.receive_text()
            data = json.loads(msg)
            msg_type = data.get("type")

            #读写数据
            if msg_type == "data":
                latest_data.update(data)
                write_RH_file(data.get("RH"))
                await ws.send_text(json.dumps({"status": "data_received"}))

            elif msg_type == "start":
                # 启动求解器
                run_solver()
                # 读取温度txt
                result = read_all_zone_temperatures(Temp_File)
                print("读取温度:", result)
                # 返回给客户端
                await ws.send_text(json.dumps(result))

            else:
                await ws.send_text(json.dumps({"error": "unknown type"}))

        except Exception as e:
            print("WebSocket错误:", e)
            break


if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)