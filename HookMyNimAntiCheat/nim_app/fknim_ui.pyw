import tkinter as tk
from tkinter import messagebox

def nim_sum(piles):
    return piles[0] ^ piles[1] ^ piles[2]  # 计算三堆石子的 Nim-sum

def find_winning_move(piles):
    total = nim_sum(piles)
    if total == 0:
        return None  # 当前无法获胜，Nim-sum 为 0
    for i, pile in enumerate(piles):
        target = pile ^ total  # 计算目标石子数
        if target < pile:
            return i, pile - target  # 返回堆号和需要取走的石子数
    return None

class NimGame:
    def __init__(self, root):
        self.root = root
        self.root.title("Nim 游戏计算器")
        
        # 初始化石子堆
        self.piles = [0, 0, 0]
        
        # 创建界面元素
        self.label = tk.Label(root, text="请输入三堆石子的初始数量：")
        self.label.pack(pady=10)
        
        self.entries = []
        for i in range(3):
            frame = tk.Frame(root)
            frame.pack(pady=5)
            label = tk.Label(frame, text=f"第 {i+1} 堆石子数量:")
            label.pack(side=tk.LEFT)
            entry = tk.Entry(frame)
            entry.pack(side=tk.LEFT)
            self.entries.append(entry)
        
        self.start_button = tk.Button(root, text="开始游戏", command=self.start_game)
        self.start_button.pack(pady=10)
        
        self.move_label = tk.Label(root, text="")
        self.move_label.pack(pady=10)
        
        self.opponent_frame = tk.Frame(root)
        self.opponent_frame.pack(pady=10)
        
        self.heap_label = tk.Label(self.opponent_frame, text="堆号 (1-3):")
        self.heap_label.pack(side=tk.LEFT)
        self.heap_entry = tk.Entry(self.opponent_frame, width=5)
        self.heap_entry.pack(side=tk.LEFT)
        
        self.count_label = tk.Label(self.opponent_frame, text="取走的石子数:")
        self.count_label.pack(side=tk.LEFT)
        self.count_entry = tk.Entry(self.opponent_frame, width=5)
        self.count_entry.pack(side=tk.LEFT)
        
        self.opponent_button = tk.Button(root, text="确认对方操作", command=self.process_opponent_move)
        self.opponent_button.pack(pady=10)
        
        self.status_label = tk.Label(root, text="")
        self.status_label.pack(pady=10)
        
        # 初始化游戏状态
        self.game_started = False
    
    def start_game(self):
        try:
            self.piles = [int(entry.get()) for entry in self.entries]
            if any(pile < 0 for pile in self.piles):
                messagebox.showerror("错误", "石子数不能为负数！")
                return
            self.game_started = True
            self.update_status()
            self.calculate_move()
        except ValueError:
            messagebox.showerror("错误", "请输入有效的整数！")
    
    def update_status(self):
        self.status_label.config(text=f"当前堆的石子数: {self.piles}")
    
    def calculate_move(self):
        if not self.game_started:
            return
        
        move = find_winning_move(self.piles)
        if move is None:
            self.move_label.config(text="当前局面无法保证获胜，Nim-sum 为 0。")
        else:
            heap, count = move
            self.move_label.config(text=f"为了获胜，你需要从第 {heap + 1} 堆取走 {count} 个石子。")
            self.piles[heap] -= count
            self.update_status()
            self.check_game_over()
    
    def process_opponent_move(self):
        if not self.game_started:
            return
        
        try:
            heap = int(self.heap_entry.get())
            count = int(self.count_entry.get())
            
            if heap < 1 or heap > 3:
                messagebox.showerror("错误", "堆号无效，请输入 1、2 或 3。")
                return
            if count <= 0:
                messagebox.showerror("错误", "取走的石子数必须大于 0。")
                return
            if self.piles[heap - 1] < count:
                messagebox.showerror("错误", f"第 {heap} 堆只有 {self.piles[heap - 1]} 个石子，无法取走 {count} 个。")
                return
            
            self.piles[heap - 1] -= count
            self.update_status()
            self.check_game_over()
            self.calculate_move()
        except ValueError:
            messagebox.showerror("错误", "输入格式错误，请按照格式输入整数。")
    
    def check_game_over(self):
        if all(pile == 0 for pile in self.piles):
            if self.game_started:
                messagebox.showinfo("游戏结束", "游戏结束！你取走了最后的石子，你获胜！")
                self.game_started = False

if __name__ == "__main__":
    root = tk.Tk()
    game = NimGame(root)
    root.mainloop()
