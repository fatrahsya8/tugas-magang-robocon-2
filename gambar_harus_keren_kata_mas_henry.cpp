// ===== [1] INCLUDE =====
#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "turtlesim/srv/set_pen.hpp"            
#include "turtlesim/srv/teleport_absolute.hpp"  

using namespace std::chrono_literals;

struct Point
{
  double x;
  double y;
};

// ===== [2] CLASS =====
class CoolSNode : public rclcpp::Node
{
public:
  // ===== [3] CONSTRUCTOR: daftarkan 2 client service =====
  CoolSNode()
  : Node("cool_s_node")
  {
    pen_client_ = this->create_client<turtlesim::srv::SetPen>("/turtle1/set_pen");
    teleport_client_ =
      this->create_client<turtlesim::srv::TeleportAbsolute>("/turtle1/teleport_absolute");

    RCLCPP_INFO(this->get_logger(), "cool_s_node siap");
  }

  // ===== [4] LOGIKA UTAMA: urutan menggambar =====
  void draw()
  {
    if (!wait_for_turtlesim()) {
      return;
    }

    // Tahap 1: 6 garis lurus (vertikal)
    draw_line({3.5, 6.5}, {3.5, 8.5});  // atas kiri
    draw_line({5.5, 6.5}, {5.5, 8.5});  // atas tengah
    draw_line({7.5, 6.5}, {7.5, 8.5});  // atas kanan
    draw_line({3.5, 2.5}, {3.5, 4.5});  // bawah kiri
    draw_line({5.5, 2.5}, {5.5, 4.5});  // bawah tengah
    draw_line({7.5, 2.5}, {7.5, 4.5});  // bawah kanan

    // Tahap 2: sambungkan garis atas ke garis bawah
    draw_line({3.5, 6.5}, {5.5, 4.5});  // atas kiri  -> bawah tengah
    draw_line({5.5, 6.5}, {7.5, 4.5});  // atas tengah -> bawah kanan

    // Tahap 3: tutup atas & bawah dengan bentuk segitiga
    draw_line({3.5, 8.5}, {5.5, 10.0});  // atas kiri  -> puncak atas
    draw_line({5.5, 10.0}, {7.5, 8.5});  // puncak atas -> atas kanan
    draw_line({3.5, 2.5}, {5.5, 1.0});   // bawah kiri -> puncak bawah
    draw_line({5.5, 1.0}, {7.5, 2.5});   // puncak bawah -> bawah kanan

    // Tahap 4: garis yang "lewat bawah" (bawah kiri -> atas kanan),
    // digambar 2 potong, bagian tengahnya disembunyikan di antara 2 diagonal.
    // Titik potong dihitung dari perpotongan garis (lihat penjelasan).
    draw_line({3.5, 4.5}, {4.83, 5.17});
    draw_line({6.17, 5.83}, {7.5, 6.5});

    RCLCPP_INFO(this->get_logger(), "Selesai menggambar!");
  }

private:
  // Tunggu sampai turtlesim_node jalan
  bool wait_for_turtlesim()
  {
    while (!pen_client_->wait_for_service(1s) || !teleport_client_->wait_for_service(1s)) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(this->get_logger(), "Dihentikan sebelum turtlesim siap");
        return false;
      }
      RCLCPP_INFO(this->get_logger(), "Menunggu turtlesim...");
    }
    return true;
  }

  // Satu garis = angkat pena, lompat ke awal, turunkan pena, lompat ke akhir
  void draw_line(Point start, Point end)
  {
    set_pen(false);
    teleport(start);
    set_pen(true);
    teleport(end);
  }

  void set_pen(bool on)
  {
    auto request = std::make_shared<turtlesim::srv::SetPen::Request>();
    request->r = 0;     // warna hijau
    request->g = 255;
    request->b = 0;
    request->width = 3;
    request->off = on ? 0 : 1;  // 1 = pena diangkat

    auto future = pen_client_->async_send_request(request);
    rclcpp::spin_until_future_complete(this->get_node_base_interface(), future);
  }

  void teleport(Point p)
  {
    auto request = std::make_shared<turtlesim::srv::TeleportAbsolute::Request>();
    request->x = p.x;
    request->y = p.y;
    request->theta = 0.0;

    auto future = teleport_client_->async_send_request(request);
    rclcpp::spin_until_future_complete(this->get_node_base_interface(), future);
  }

  // ===== [5] MEMBER =====
  rclcpp::Client<turtlesim::srv::SetPen>::SharedPtr pen_client_;
  rclcpp::Client<turtlesim::srv::TeleportAbsolute>::SharedPtr teleport_client_;
};

// ===== [6] MAIN =====
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<CoolSNode>();
  node->draw();  // gambar sekali, lalu selesai (tidak perlu rclcpp::spin)
  rclcpp::shutdown();
  return 0;
}