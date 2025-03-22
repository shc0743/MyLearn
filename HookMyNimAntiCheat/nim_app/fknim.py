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

def get_valid_input(piles):
    while True:
        try:
            opponent_move = input("\n请输入对方的操作（格式：<第n堆石子>,<对方取走的数量>，例如 '1,2'）：")
            heap, count = map(int, opponent_move.split(','))
            if heap < 1 or heap > 3:
                print("堆号无效，请输入 1、2 或 3。")
            elif count <= 0:
                print("取走的石子数必须大于 0。")
            elif piles[heap - 1] < count:
                print(f"第 {heap} 堆只有 {piles[heap - 1]} 个石子，无法取走 {count} 个。")
            else:
                return heap, count
        except ValueError:
            print("输入格式错误，请按照格式 '<堆号>,<数量>' 输入。")

def main():
    print("欢迎来到 Nim 计算器！")
    piles = []
    for i in range(3):
        while True:
            try:
                pile = int(input(f"请输入第 {i+1} 堆石子的初始数量: "))
                if pile < 0:
                    print("石子数不能为负数，请重新输入。")
                else:
                    piles.append(pile)
                    break
            except ValueError:
                print("输入无效，请输入一个整数。")
    
    while True:
        print("\n当前堆的石子数:", piles)
        
        # 计算你的获胜策略
        move = find_winning_move(piles)
        if move is None:
            print("当前局面无法保证获胜，Nim-sum 为 0。")
        else:
            heap, count = move
            print(f"为了获胜，你需要从第 {heap + 1} 堆取走 {count} 个石子。")
            piles[heap] -= count  # 更新堆的石子数
            print(f"取完后，堆的石子数更新为: {piles}")
        
        # 检查游戏是否结束
        if all(pile == 0 for pile in piles):
            print("\n游戏结束！你取走了最后的石子，你获胜！")
            break
        
        # 输入对方的操作（带验证）
        heap, count = get_valid_input(piles)
        piles[heap - 1] -= count  # 更新堆的石子数
        
        # 检查游戏是否结束
        if all(pile == 0 for pile in piles):
            print("\n游戏结束！对方取走了最后的石子，对方获胜。")
            break

if __name__ == "__main__":
    main()
    input('ENTER>')
