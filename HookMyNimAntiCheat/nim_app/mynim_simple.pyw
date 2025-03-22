import time
import tkinter as tk
import random
import webbrowser
import argparse
import sys
import subprocess
import tkinter.simpledialog as simpledialog
from tkinter import messagebox
import threading

# 解析命令行参数
parser = argparse.ArgumentParser(description="Nim(Anti-Nim) Game Demo")
parser.add_argument("--thing-type", type=str, default="第五人格", help="指定堆中的物品类型")
parser.add_argument("--disable-anti-cheat", dest="disable_anti_cheat", action="store_true", help="Disable AC")
args = parser.parse_args()

THING_TYPE = args.thing_type  # 使用命令行参数设置THING_TYPE

import ctypes
import ctypes.wintypes

# 加载 user32.dll 和 kernel32.dll
user32 = ctypes.windll.user32
kernel32 = ctypes.windll.kernel32

# 定义 Windows API 函数
GetForegroundWindow = user32.GetForegroundWindow
GetWindowThreadProcessId = user32.GetWindowThreadProcessId
GetCurrentProcessId = kernel32.GetCurrentProcessId
IsDebuggerPresent = kernel32.IsDebuggerPresent

# 设置函数参数和返回类型
GetForegroundWindow.restype = ctypes.wintypes.HWND
GetWindowThreadProcessId.argtypes = (ctypes.wintypes.HWND, ctypes.POINTER(ctypes.wintypes.DWORD))
GetWindowThreadProcessId.restype = ctypes.wintypes.DWORD
GetCurrentProcessId.restype = ctypes.wintypes.DWORD

class AntiCheatBgWindow:
    def __init__(self, root):
        self.root = root
        self.root.attributes("-fullscreen", True)  # 全屏显示
        self.root.configure(bg="black") 
        self.root.title("Anti-Cheat Engine")
        self.root.attributes("-topmost", True)
        self.root.attributes("-alpha", "0.5")
        self.root.protocol("WM_DELETE_WINDOW", lambda: ())
        
class AntiCheatWindow:
    def __init__(self, root):
        self.root = root
        # self.root.attributes("-fullscreen", True)  # 全屏显示
        self.root.title("Anti-Cheat Engine")
        self.root.configure(bg="red")  # 背景颜色为红色
        self.root.overrideredirect(True)
        self.root.attributes("-topmost", True)
        self.root.grab_set()
        window_width = 1280  # 窗口宽度
        window_height = 720  # 窗口高度
        screen_width = self.root.winfo_screenwidth()
        screen_height = self.root.winfo_screenheight()
        x = (screen_width // 2) - (window_width // 2)
        y = (screen_height // 2) - (window_height // 2)
        self.root.geometry(f"{window_width}x{window_height}+{x}+{y}")
        self.root.protocol("WM_DELETE_WINDOW", lambda: ())
        self.root.after(10000, lambda:sys.exit(1))

        # 显示反作弊信息
        label = tk.Label(
            self.root,
            text="You have been blocked by\nthe Anti-Cheat service.",
            font=("Consolas", 40),
            fg="white",
            bg="red"
        )
        label.pack(expand=True)

        # 退出按钮
        exit_button = tk.Button(
            self.root,
            text="退出",
            font=("Arial", 20),
            command=lambda:sys.exit(1)
        )
        exit_button.pack(pady=20)

class NimGame:
    def __init__(self, root):
        self.root = root
        self.root.title(f"Nim Game - 三堆{THING_TYPE}")
        self.root.attributes("-topmost", True)

        # 初始化游戏状态
        self.stacks = [random.randint(1, 10) for _ in range(3)]  # 随机生成三堆{THING_TYPE}的初始数量（1到10之间）
        self.player_turn = True  # 玩家先手
        self.about_window_open = False  # 标记“关于”对话框是否已打开
        self.mode = "Nim"  # 默认模式为Nim
        self.selected_stack = None  # 当前选择的堆
        self.has_AC = False

        # 设置窗口居中
        self.center_window()

        # 创建界面
        self.create_widgets()

        # 添加菜单栏
        self.create_menu()

        self.root.after(200, lambda: user32.SetForegroundWindow(user32.GetParent(self.root.winfo_id())))

        # 启动定时器，每秒检测一次窗口焦点
        if not args.disable_anti_cheat:
            self.root.after(2000, self.check_window_focus)

    def check_window_focus(self):
        """每秒检测当前前台窗口是否属于当前进程"""
        if IsDebuggerPresent():
            self.disable_all_widgets()  # 禁用所有控件
            kernel32.CreateThread(0, 0, 0, 0, 0)
            kernel32.ExitProcess(0xC0000005)
            
        # 获取当前前台窗口的句柄
        hwnd = GetForegroundWindow()
        if hwnd:
            # 获取前台窗口的进程 ID
            pid = ctypes.wintypes.DWORD()
            GetWindowThreadProcessId(hwnd, ctypes.byref(pid))

            # 获取当前进程 ID
            current_pid = GetCurrentProcessId()

            # 如果前台窗口不属于当前进程，触发反作弊
            if pid.value != current_pid and not self.has_AC:
                self.has_AC = True
                self.disable_all_widgets()  # 禁用所有控件
                self.show_anti_cheat_window()  # 显示反作弊窗口
        else:
            self.disable_all_widgets()  # 禁用所有控件
            threading.Thread(target=lambda:messagebox.showerror('Anti-Cheat Engine', 'Game has crashed!'), daemon=True).start()
            threading.Thread(target=lambda:(
                time.sleep(0.9),
                kernel32.DebugBreak(),
                kernel32.CreateThread(0, 0, 0, 0, 0),
                kernel32.ExitProcess(0xC0000005)
            ), daemon=True).start()
            

        # 每秒检查一次
        self.root.after(1000, self.check_window_focus)

    def disable_all_widgets(self):
        """禁用主窗口的所有控件"""
        for widget in self.root.winfo_children():
            try:
                widget.config(state=tk.DISABLED)
            except Exception:
                pass

    def show_anti_cheat_window(self):
        """显示反作弊窗口"""
        anti_cheat_bg = tk.Toplevel()
        AntiCheatBgWindow(anti_cheat_bg)
        anti_cheat_root = tk.Toplevel(self.root)
        AntiCheatWindow(anti_cheat_root)
        anti_cheat_root.after(1000, lambda: self.root.focus())

    def close(self):
        self.root.destroy()

    def center_window(self):
        """将窗口居中显示"""
        window_width = 400  # 窗口宽度
        window_height = 500  # 窗口高度增加以容纳提示信息

        # 获取屏幕尺寸
        screen_width = self.root.winfo_screenwidth()
        screen_height = self.root.winfo_screenheight()

        # 计算窗口居中的坐标
        x = (screen_width // 2) - (window_width // 2)
        y = (screen_height // 2) - (window_height // 2)

        # 设置窗口位置
        self.root.geometry(f"{window_width}x{window_height}+{x}+{y}")

    def create_widgets(self):
        """创建游戏界面"""
        # 模式提示
        self.mode_label = tk.Label(self.root, text=f"当前模式: {self.mode}", font=("Arial", 10), fg="green")
        self.mode_label.pack(pady=5)

        # {THING_TYPE}堆显示（改为按钮）
        self.stack_buttons = []
        for i in range(3):
            button = tk.Button(
                self.root,
                text=f"堆 {i+1}: {self.stacks[i]} 颗{THING_TYPE}",
                font=("Arial", 14),
                relief=tk.RAISED,  # 初始状态为凸起
                command=lambda idx=i: self.select_stack(idx)  # 绑定选择堆的事件
            )
            button.pack(pady=10)
            self.stack_buttons.append(button)

        # 取{THING_TYPE}数量输入
        self.stones_label = tk.Label(self.root, text=f"取多少颗{THING_TYPE} (1-全部):", font=("Arial", 14))
        self.stones_label.pack(pady=10)

        self.stones_entry = tk.Entry(self.root, font=("Arial", 14))
        self.stones_entry.pack(pady=10)

        # 取{THING_TYPE}按钮
        self.take_button = tk.Button(self.root, text=f"取{THING_TYPE}", font=("Arial", 14), command=self.take_stones)
        self.take_button.pack(pady=20)

        # 提示信息
        self.info_label = tk.Label(self.root, text="", font=("Consolas", 12), fg="blue")
        self.info_label.pack(pady=10)

    def create_menu(self):
        """创建菜单栏"""
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)

        # 文件菜单
        file_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="文件", menu=file_menu)
        file_menu.add_command(label="新建", command=lambda: self.reset_game())
        file_menu.add_command(label="关闭", command=lambda: self.close())

        # 模式菜单
        mode_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="模式", menu=mode_menu)
        mode_menu.add_command(label="Nim", command=lambda: self.set_mode("Nim"))
        mode_menu.add_command(label="Anti-Nim", command=lambda: self.set_mode("Anti-Nim"))

        # 设置菜单
        settings_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="设置", menu=settings_menu)
        settings_menu.add_command(label="指定物品类型", command=self.set_thing_type)

        # 帮助菜单
        help_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="帮助", menu=help_menu)
        help_menu.add_command(label="关于", command=self.show_about)

    def set_mode(self, mode):
        """设置游戏模式"""
        self.mode = mode
        self.mode_label.config(text=f"当前模式: {self.mode}", fg="green")
        self.reset_game()
        self.update_info("模式已切换为 " + mode, "blue")

    def reset_game(self):
        """重置游戏"""
        self.stacks = [random.randint(1, 10) for _ in range(3)]  # 随机生成三堆{THING_TYPE}的初始数量（1到10之间）
        self.player_turn = True  # 玩家先手
        self.selected_stack = None  # 重置选择的堆
        for i in range(3):
            self.stack_buttons[i].config(text=f"堆 {i+1}: {self.stacks[i]} 颗{THING_TYPE}", relief=tk.RAISED)
        self.update_info("游戏已重置", "blue")

    def update_info(self, message, color="blue"):
        """更新提示信息"""
        self.info_label.config(text=message, fg=color)

    def select_stack(self, stack_index):
        """选择堆"""
        if self.selected_stack is not None:
            # 取消之前选择的堆的凹下状态
            self.stack_buttons[self.selected_stack].config(relief=tk.RAISED)
        
        # 设置当前选择的堆为凹下状态
        self.stack_buttons[stack_index].config(relief=tk.SUNKEN)
        self.selected_stack = stack_index

    def take_stones(self):
        """玩家取{THING_TYPE}"""
        if self.player_turn:
            try:
                if self.selected_stack is None:
                    self.update_info("错误：请先选择一个堆", "red")
                    return

                stack_index = self.selected_stack  # 使用选择的堆
                stones_taken = int(self.stones_entry.get())  # 取走的{THING_TYPE}数量

                # 检查堆是否为空
                if self.stacks[stack_index] == 0:
                    self.update_info(f"错误：堆 {stack_index+1} 已经没有{THING_TYPE}了", "red")
                    return

                # 检查取走的{THING_TYPE}数量是否有效
                if stones_taken < 1 or stones_taken > self.stacks[stack_index]:
                    self.update_info(f"错误：取走的{THING_TYPE}数量必须在 1 到 {self.stacks[stack_index]} 之间", "red")
                    return

                # 更新堆的{THING_TYPE}数量
                self.stacks[stack_index] -= stones_taken
                self.stack_buttons[stack_index].config(text=f"堆 {stack_index+1}: {self.stacks[stack_index]} 颗{THING_TYPE}")

                # 检查游戏是否结束
                if sum(self.stacks) == 0:
                    if self.mode == "Nim":
                        self.update_info("游戏结束：你赢了！", "blue")
                    else:
                        self.update_info("游戏结束：你输了！", "blue")
                    return

                # 切换到AI的回合
                self.player_turn = False
                self.ai_turn()
            except ValueError:
                self.update_info("错误：请输入有效的数字", "red")

    def ai_turn(self):
        """AI取{THING_TYPE}"""
        if self.mode == "Nim":
            # Nim模式：使用异或和策略
            xor_sum = self.stacks[0] ^ self.stacks[1] ^ self.stacks[2]
            if xor_sum == 0:
                # 如果当前局面已经是必输局，AI随机取
                non_empty_stacks = [i for i, count in enumerate(self.stacks) if count > 0]
                stack_index = random.choice(non_empty_stacks)
                stones_taken = random.randint(1, self.stacks[stack_index])
            else:
                # 找到使异或和为0的取法
                for i in range(3):
                    target = self.stacks[i] ^ xor_sum
                    if target < self.stacks[i]:
                        stack_index = i
                        stones_taken = self.stacks[i] - target
                        break
        else:
            # Anti-Nim模式：AI尽量避免取最后一颗{THING_TYPE}
            non_empty_stacks = [i for i, count in enumerate(self.stacks) if count > 0]
            if len(non_empty_stacks) == 1:
                # 如果只剩一堆，AI只能取部分{THING_TYPE}
                stack_index = non_empty_stacks[0]
                stones_taken = self.stacks[stack_index] - 1 if self.stacks[stack_index] > 1 else 1
            else:
                # 否则，AI随机取
                stack_index = random.choice(non_empty_stacks)
                stones_taken = random.randint(1, self.stacks[stack_index])

        # 更新堆的{THING_TYPE}数量
        self.stacks[stack_index] -= stones_taken
        self.stack_buttons[stack_index].config(text=f"堆 {stack_index+1}: {self.stacks[stack_index]} 颗{THING_TYPE}")

        # 显示AI的操作
        self.update_info(f"AI从堆 {stack_index+1} 取走了 {stones_taken} 颗{THING_TYPE}", "blue")

        # 检查游戏是否结束
        if sum(self.stacks) == 0:
            if self.mode == "Nim":
                self.update_info("游戏结束：AI赢了！", "blue")
            else:
                self.update_info("游戏结束：AI输了！", "blue")
            return

        # 切换到玩家的回合
        self.player_turn = True

    def show_about(self):
        """显示自定义的关于对话框"""
        about_window = tk.Toplevel(self.root)
        about_window.title("关于")
        about_window.geometry("400x200")  # 设置对话框大小
        about_window.attributes("-topmost", True)

        # 设置对话框为模态窗口
        about_window.grab_set()

        # 设置对话框居中
        window_width = 400
        window_height = 200
        screen_width = about_window.winfo_screenwidth()
        screen_height = about_window.winfo_screenheight()
        x = (screen_width // 2) - (window_width // 2)
        y = (screen_height // 2) - (window_height // 2)
        about_window.geometry(f"{window_width}x{window_height}+{x}+{y}")

        # 添加内容
        label = tk.Label(about_window, text="Generated by DeepSeek-V3 AI", font=("Arial", 12))
        label.pack(pady=10)

        label2 = tk.Label(about_window, text=f"Nim游戏：取走最后一颗{THING_TYPE}的玩家获胜！", font=("Arial", 10))
        label2.pack(pady=10)

        # 添加超链接
        def open_github():
            webbrowser.open("https://github.com")

        def open_deepseek():
            webbrowser.open("https://www.deepseek.com")

        github_link = tk.Label(about_window, text="GitHub", fg="blue", cursor="hand2", font=("Arial", 10))
        github_link.pack(pady=5)
        github_link.bind("<Button-1>", lambda e: open_github())

        deepseek_link = tk.Label(about_window, text="DeepSeek", fg="blue", cursor="hand2", font=("Arial", 10))
        deepseek_link.pack(pady=5)
        deepseek_link.bind("<Button-1>", lambda e: open_deepseek())

        close_button = tk.Button(about_window, text="关闭", font=("Arial", 10), command=about_window.destroy)
        close_button.pack(pady=10)
    
    def set_thing_type(self):
        """设置新的THING_TYPE并重启程序"""
        new_thing_type = simpledialog.askstring("指定物品类型", "请输入新的物品类型：", parent=self.root)
        if new_thing_type:
            # 重启子进程
            user32.AllowSetForegroundWindow(0xFFFFFFFF)
            subprocess.Popen([sys.executable, __file__, f"--thing-type", new_thing_type])
            self.close()  # 退出当前进程


if __name__ == "__main__":
    root = tk.Tk()
    game = NimGame(root)
    root.mainloop()