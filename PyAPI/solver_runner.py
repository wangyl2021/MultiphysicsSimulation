import subprocess

def run_solver():

    print("启动OpenFOAM求解器...")

    cmd_clean = (
        'wsl bash -c "'
        'source /home/dym/OpenFOAM/OpenFOAM-v2506/etc/bashrc && '
        'cd /home/dym/OpenFOAM/dym-v2506/run/inoutput && '
        './Allclean'
        '"'
    )

    cmd_run = (
        'wsl bash -c "'
        'source /home/dym/OpenFOAM/OpenFOAM-v2506/etc/bashrc && '
        'cd /home/dym/OpenFOAM/dym-v2506/run/inoutput && '
        './Allrun'
        '"'
    )

    subprocess.run(cmd_clean, shell=True)
    subprocess.run(cmd_run, shell=True)

    print("求解器完毕...")