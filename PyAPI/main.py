import asyncio
import json
import websockets

from file_exchange import *
from solver_runner import run_solver
from config import *

latest_data = {}

async def websocket_client():
    #地址
    uri = "ws://external-api.com/ws"  

    async with websockets.connect(uri) as ws:

        print("已连接外部WebSocket服务器")

        while True:

            try:
                msg = await ws.recv()
                data = json.loads(msg)
                msg_type = data.get("type")

                # 读写数据
                if msg_type == "data":

                    latest_data.update(data)

                    write_RH_file(data.get("RH"))

                    await ws.send(json.dumps({
                        "status": "data_received"
                    }))


                elif msg_type == "start":

                    # 启动求解器
                    run_solver()

                    # 读取温度txt
                    result = read_all_zone_temperatures(Temp_File)

                    print("读取温度:", result)

                    # 返回给服务器
                    await ws.send(json.dumps(result))


                else:

                    await ws.send(json.dumps({
                        "error": "unknown type"
                    }))

            except Exception as e:

                print("WebSocket错误:", e)
                break


if __name__ == "__main__":

    asyncio.run(websocket_client())