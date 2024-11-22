import sys
import os
import time
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.animation import FuncAnimation

def read_data(filename):
    rectangles = []
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('<'):
                continue
            parts = line.split()
            if len(parts) != 5:
                continue
            name, x, y, x2, y2 = parts
            x = float(x)
            y = float(y)
            x2 = float(x2)
            y2 = float(y2)
            w = x2 - x
            h = y2 - y
            rectangles.append({'name': name, 'x': x, 'y': y, 'w': w, 'h': h})
    return rectangles

def draw_rectangles(ax, rectangles):
    ax.clear()
    if not rectangles:
        return
    max_x = max(rect['x'] + rect['w'] for rect in rectangles)
    max_y = max(rect['y'] + rect['h'] for rect in rectangles)
    
    for rect in rectangles:
        name = rect['name']
        x = rect['x']
        y = rect['y']
        w = rect['w']
        h = rect['h']
        rect_patch = patches.Rectangle((x, y), w, h, linewidth=1, edgecolor='blue', facecolor='skyblue', alpha=0.5)
        ax.add_patch(rect_patch)
        # ax.text(x + w/2, y + h/2, name, ha='center', va='center', fontsize=8)
    
    ax.set_xlim(0, max_x)
    ax.set_ylim(0, max_y) 
    ax.set_aspect('equal', adjustable='box')
    plt.xlabel('X AXIS')
    plt.ylabel('Y AXIS')
    plt.title('Block Diagram')

def update(frame, filename, ax, last_mtime):
    try:
        mtime = os.path.getmtime(filename)
    except FileNotFoundError:
        print(f"File {filename} not found.")
        return last_mtime
    if mtime != last_mtime[0]:
        rectangles = read_data(filename)
        draw_rectangles(ax, rectangles)
        plt.draw()
        last_mtime[0] = mtime
    return last_mtime

def main():
    if len(sys.argv) != 3:
        print("Usage: python draw_block.py <filename> <output>")
        sys.exit(1)
    
    filename = sys.argv[1]
    last_mtime = [0]

    fig, ax = plt.subplots()
    # ani = FuncAnimation(fig, update, fargs=(filename, ax, last_mtime), interval=1000)

    # plt.show()

    
    rectangles = read_data(filename)
    draw_rectangles(ax, rectangles)
    # Save the figure
    plt.savefig(sys.argv[2])
    plt.close()

if __name__ == '__main__':
    main()