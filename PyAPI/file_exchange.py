from config import RH_FILE,Temp_File
import glob

def write_RH_file(RH_value):
    with open(RH_FILE, "w") as f:
        f.write(f"{RH_value}\n")
    print(f"RH 已写入 {RH_FILE}: {RH_value}")

def read_all_zone_temperatures(Temp_File):
    results = []
    #读取所有zone的温度数据
    for filepath in sorted(glob.glob(f"{Temp_File}/zone_*.txt")):
        filename = filepath.split("/")[-1]
        try:
            time_part = filename.split("_")[1].split(".")[0]
            sim_time = float(time_part)
        except:
            sim_time = None

        #读取文件内容
        temp_data = []
        with open(filepath) as f:
            for line in f:
                parts = line.split()
                if len(parts) == 2:
                    idx, temp = parts
                    temp_data.append({"index": int(idx), "temperature": float(temp)})

        results.append({"time": sim_time, "data": temp_data})

    return results