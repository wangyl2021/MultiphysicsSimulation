import asyncio
import websockets
import json

async def main():
    uri = "ws://localhost:8000/ws"  # WebSocket 地址
    async with websockets.connect(uri) as ws:
        print("已连接到服务器")

        # 1️⃣ 发送 RH 数据
        rh_data = {"type": "data", "RH": 55}
        await ws.send(json.dumps(rh_data))
        response = await ws.recv()
        print("服务器回复:", response)

        # 2️⃣ 请求启动求解器
        start_data = {"type": "start"}
        await ws.send(json.dumps(start_data))
        result = await ws.recv()
        print("服务器返回温度数据:", result)

        # 可以继续发送更多数据或关闭连接
        await ws.close()
        print("已关闭连接")

# 启动 asyncio
asyncio.run(main())