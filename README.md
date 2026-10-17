# Gambar bebas pakai turtlesim

## Video penjelasan
https://youtube.com/shorts/6YqGDeaxNrw?si=93rl5P22HRFNldyn

## Cara kerja
Setiap garis digambar dengan:
1. Angkat pena (`/turtle1/set_pen`)
2. Teleport ke titik awal (`/turtle1/teleport_absolute`)
3. Turunkan pena
4. Teleport ke titik akhir

## Environment
- Ubuntu 22.04
- ROS2 Humble

## Cara menjalankan
```bash
# Terminal 1
ros2 run turtlesim turtlesim_node

# Terminal 2
cd ~/ros2_ws
colcon build --packages-select gambar_harus_keren_kata_mas_henry
source install/setup.bash
ros2 run gambar_harus_keren_kata_mas_henry cool_s_node
```
