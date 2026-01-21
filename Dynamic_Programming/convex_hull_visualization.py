import matplotlib.pyplot as plt
import numpy as np

def get_intersection_x(k1, b1, k2, b2):
    """Находит x-координату пересечения двух прямых: k1*x + b1 = k2*x + b2"""
    if k1 == k2:
        return None
    return (b2 - b1) / (k1 - k2)

def plot_situation(ax, lines, title, removal_case):
    # Распаковка линий: (k, b, label, color, style)
    l1, l2, l3 = lines
    
    # Диапазон построения
    x = np.linspace(-4, 6, 400)
    
    # Функции линий
    y1_vals = l1[0] * x + l1[1]
    y2_vals = l2[0] * x + l2[1]
    y3_vals = l3[0] * x + l3[1]
    
    # Рисуем линии
    ax.plot(x, y1_vals, color=l1[3], linestyle=l1[4], label=l1[2], linewidth=2)
    ax.plot(x, y2_vals, color=l2[3], linestyle=l2[4], label=l2[2], linewidth=2)
    ax.plot(x, y3_vals, color=l3[3], linestyle=l3[4], label=l3[2], linewidth=2)
    
    # Вычисляем пересечения
    x_int_12 = get_intersection_x(l1[0], l1[1], l2[0], l2[1]) # Пересечение Предпоследней и Последней
    y_int_12 = l1[0] * x_int_12 + l1[1]
    
    x_int_23 = get_intersection_x(l2[0], l2[1], l3[0], l3[1]) # Пересечение Последней и Новой
    y_int_23 = l2[0] * x_int_23 + l2[1]
    
    # Рисуем точки пересечения
    ax.plot(x_int_12, y_int_12, 'o', color='black', markersize=8)
    ax.annotate(r'$I_{1-2}$', (x_int_12, y_int_12), textcoords="offset points", xytext=(0,10), ha='center')
    
    ax.plot(x_int_23, y_int_23, 'o', color='red', markersize=8)
    ax.annotate(r'$I_{2-3}$', (x_int_23, y_int_23), textcoords="offset points", xytext=(0,10), ha='center')

    # Визуализация нижней огибающей (заливка)
    envelope = np.minimum(np.minimum(y1_vals, y2_vals), y3_vals)
    ax.fill_between(x, -15, envelope, color='gray', alpha=0.1, label='Нижняя огибающая')

    # Пояснения
    status = "УДАЛЯЕМ L2" if removal_case else "ОСТАВЛЯЕМ L2"
    ax.set_title(f"{title}\n{status}", fontsize=12, fontweight='bold')
    ax.legend(loc='upper right')
    ax.grid(True, alpha=0.3)
    ax.set_ylim(-5, 10)

# Настройка фигуры
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 7))

# --- Сценарий 1: УДАЛЕНИЕ (Пересечение с новой левее старого пересечения) ---
# Прямые отсортированы по убыванию наклона k: 2, 0.5, -1
# L1: Крутая (синяя)
# L2: Пологая (оранжевая) - она будет удалена
# L3: Убывающая (зеленая) - "подрезает" L2 слишком рано
lines_bad = [
    (2, 0, 'L1 (Предпоследняя)', 'blue', '-'),     # y = 2x
    (0.5, -1, 'L2 (Последняя)', 'orange', '-'),    # y = 0.5x - 1
    (-1, 0, 'L3 (Новая)', 'green', '--')           # y = -x
]
# Проверка:
# I_12: 2x = 0.5x - 1 => 1.5x = -1 => x = -0.66
# I_23: 0.5x - 1 = -x => 1.5x = 1 => x = 0.66
# Здесь I_23 > I_12, это нормальный случай... стоп, мне нужен случай удаления.
# Чтобы удалить L2, I_23 должен быть ЛЕВЕЕ I_12 (или L3 должна пройти ниже).
# Изменим L3.
lines_removal = [
    (2, 2, 'L1 (k=2)', 'blue', '-'),         # y = 2x + 2
    (0.5, 0, 'L2 (k=0.5)', 'orange', '-'),   # y = 0.5x
    (-1, -4, 'L3 (k=-1)', 'green', '--')     # y = -x - 4
]
# I_12: 2x+2 = 0.5x => 1.5x = -2 => x = -1.33
# I_23: 0.5x = -x-4 => 1.5x = -4 => x = -2.66
# Теперь x(2,3) < x(1,2). Точка пересечения с новой прямой левее предыдущей. L2 бесполезна.
plot_situation(ax1, lines_removal, "Случай 1: $x_{2-3} < x_{1-2}$", removal_case=True)


# --- Сценарий 2: СОХРАНЕНИЕ (Пересечение с новой правее старого пересечения) ---
lines_keep = [
    (2, 2, 'L1 (k=2)', 'blue', '-'),         # y = 2x + 2
    (0.5, 0, 'L2 (k=0.5)', 'orange', '-'),   # y = 0.5x
    (-1, 4, 'L3 (k=-1)', 'green', '--')      # y = -x + 4
]
# I_12 тот же: x = -1.33
# I_23: 0.5x = -x+4 => 1.5x = 4 => x = 2.66
# Теперь x(2,3) > x(1,2). Все линии формируют огибающую.
plot_situation(ax2, lines_keep, "Случай 2: $x_{2-3} > x_{1-2}$", removal_case=False)

plt.tight_layout()
plt.show()

# нижняя огибающая ломаная (стек/дек прямых)
def lower_convex_hull_broken_line():
    # Диапазон x
    x = np.linspace(-5, 8, 400)
    
    # Уравнения прямых
    y1 = 2 * x
    y2 = 0.5 * x - 2
    y3 = -1 * x + 4
    
    # Вычисление нижней огибающей (минимум в каждой точке)
    y_envelope = np.minimum(np.minimum(y1, y2), y3)
    
    plt.figure(figsize=(10, 6))
    
    # Рисуем исходные прямые (пунктиром)
    plt.plot(x, y1, '--', label='y = 2x', alpha=0.5)
    plt.plot(x, y2, '--', label='y = 0.5x - 2', alpha=0.5)
    plt.plot(x, y3, '--', label='y = -x + 4', alpha=0.5)
    
    # Рисуем нижнюю огибающую (жирная линия)
    plt.plot(x, y_envelope, 'k-', linewidth=3, label='Нижняя огибающая')
    
    plt.title('Нижняя огибающая множества прямых')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.legend()
    plt.grid(True)
    plt.ylim(-10, 10)
    plt.show()
